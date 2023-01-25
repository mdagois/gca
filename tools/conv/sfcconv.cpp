#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <map>
#include <png.h>
#include <set>
#include <string>
#include <vector>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////

enum : uint32_t
{
	kTileSize = 8,
	kColorsPerPalette = 16,
	kPaletteMaxCount = 3,
	kPaletteFirstPaletteIndex = 4,
	kTilesMaxCount = 256,

	kColorIndex_Invalid = 0xFFFFFFFFU,
	kPaletteIndex_Invalid = 0xFFFFFFFFU,
};

enum : uint16_t
{
	kBGR555_Invalid = 0x8000U,
	kBGR555_ClearColor = 0x7C1FU,
};

////////////////////////////////////////////////////////////////////////////////
// File
////////////////////////////////////////////////////////////////////////////////

enum : uint32_t
{
	kExtensionLength = 4,
};

static string getOutputFilename(const char* filename, const char* extension)
{
	assert(strlen(extension) == kExtensionLength);

	string output = filename;
	if(output.find_last_of('.') == output.size() - kExtensionLength)
	{
		output.pop_back();
		output.pop_back();
		output.pop_back();
		output.pop_back();
	}
	output.append(extension);
	return output;
}

////////////////////////////////////////////////////////////////////////////////
// Color
////////////////////////////////////////////////////////////////////////////////

struct ColorRGBA
{
	union
	{
		uint32_t rgba;
		struct
		{
			uint8_t r;
			uint8_t g;
			uint8_t b;
			uint8_t a;
		};
	};
};

typedef uint16_t ColorBGR555;

static ColorBGR555 convertColor(ColorRGBA rgba)
{
	const uint8_t red = rgba.r / 8;
	const uint8_t green = rgba.g / 8;
	const uint8_t blue = rgba.b / 8;
	assert(red < 32 && green < 32 && blue < 32);
	return (blue << 10) | (green << 5) | red;
}

static double getLuminance(const ColorBGR555 color)
{
	const double red = ((color >> 10) & 0x1F) / 32.0;
	const double green = ((color >> 5) & 0x1F) / 32.0;
	const double blue = (color & 0x1F) / 32.0;
	return 0.2126 * red + 0.7152 * green + 0.0722 * blue;
}

static void sortColors(ColorBGR555* colors, uint32_t count)
{
	sort(
		colors + 1, colors + count - 1,
		[](const ColorBGR555 lhs, const ColorBGR555 rhs)
		{
			return getLuminance(lhs) < getLuminance(rhs);
		});
}

////////////////////////////////////////////////////////////////////////////////
// Image
////////////////////////////////////////////////////////////////////////////////

class Image
{
public:
	Image();
	~Image();

	bool read(const char* filename);
	bool validateSize() const;

	uint32_t getWidth() const;
	uint32_t getHeight() const;
	const char* getFilename() const;
	const ColorRGBA* getPixels() const;

private:
	string m_filename;
	png_image m_png;
	union
	{
		png_bytep m_buffer;
		ColorRGBA* m_pixels;
	};
};

typedef vector<Image> ImageList;

Image::Image()
: m_buffer(nullptr)
{
	memset(&m_png, 0, sizeof(png_image));
}

Image::~Image()
{
	if(m_buffer)
	{
		free(m_buffer);
	}
	png_image_free(&m_png);
}

bool Image::read(const char* filename)
{
	assert(filename);

	m_png.version = PNG_IMAGE_VERSION;
	if(png_image_begin_read_from_file(&m_png, filename) == 0)
	{
		return false;
	}

	m_png.format = PNG_FORMAT_RGBA;
	m_buffer = reinterpret_cast<png_bytep>(malloc(PNG_IMAGE_SIZE(m_png)));
	if(m_buffer == nullptr)
	{
		png_image_free(&m_png);
		return false;
	}

	if(png_image_finish_read(&m_png, nullptr, m_buffer, 0, nullptr) == 0)
	{
		free(m_buffer);
		png_image_free(&m_png);
		return false;
	}

	m_filename = filename;
	return true;
}

bool Image::validateSize() const
{
	return ((getWidth() % kTileSize) == 0) && ((getHeight() % kTileSize) == 0);
}

uint32_t Image::getWidth() const
{
	return m_png.width;
}

uint32_t Image::getHeight() const
{
	return m_png.height;
}

const char* Image::getFilename() const
{
	return m_filename.c_str();
}

const ColorRGBA* Image::getPixels() const
{
	return m_pixels;
}

static bool iterateImageTiles(const Image& image, function<bool(const ColorRGBA*, uint32_t, uint32_t)> tile_callback)
{
	const ColorRGBA* pixels = image.getPixels();
	const uint32_t tile_row_count = image.getHeight() / kTileSize;
	const uint32_t tile_column_count = image.getWidth() / kTileSize;
	for(uint32_t j = 0; j < tile_row_count; ++j)
	{
		for(uint32_t i = 0; i < tile_column_count; ++i)
		{
			const ColorRGBA* tile_pixels = pixels + (j * image.getWidth() * kTileSize) + (i * kTileSize);
			if(!tile_callback(tile_pixels, i, j))
			{
				return false;
			}
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// Palette
////////////////////////////////////////////////////////////////////////////////

struct Palette
{
	Palette()
	{
		for(int32_t i = 0; i < kColorsPerPalette; ++i)
		{
			colors[i] = kBGR555_Invalid;
		}
		colors[0] = kBGR555_ClearColor;
	}

	ColorBGR555 colors[kColorsPerPalette];
};

struct PaletteSet
{
	Palette palettes[kPaletteMaxCount];
};

static bool extractTilePalette(Palette& out_tile_palette, const ColorRGBA* pixels, uint32_t row_pitch)
{
	set<ColorBGR555> colors;
	for(uint32_t j = 0; j < kTileSize; ++j)
	{
		const ColorRGBA* row_pixels = pixels + (j * row_pitch);
		for(uint32_t i = 0; i < kTileSize; ++i)
		{
			colors.insert(convertColor(row_pixels[i]));
		}
	}
	if(colors.size() > kColorsPerPalette)
	{
		cout << "Too many colors (" << colors.size() << ") in tile" << endl;
		return false;
	}

	uint32_t c = 1;
	for(ColorBGR555 color : colors)
	{
		out_tile_palette.colors[c] = color;
		++c;
	}
	sortColors(out_tile_palette.colors, c);

	return true;
}

static bool mergePalettes(Palette& out_palette, const Palette lhs, const Palette rhs)
{
	set<ColorBGR555> colors;
	for(uint32_t i = 0; i < kColorsPerPalette; ++i)
	{
		if(lhs.colors[i] == kBGR555_Invalid)
		{
			break;
		}
		colors.insert(lhs.colors[i]);
	}
	for(uint32_t i = 0; i < kColorsPerPalette; ++i)
	{
		if(rhs.colors[i] == kBGR555_Invalid)
		{
			break;
		}
		colors.insert(rhs.colors[i]);
	}

	if(colors.size() > kColorsPerPalette)
	{
		return false;
	}

	uint32_t c = 1;
	for(ColorBGR555 color : colors)
	{
		out_palette.colors[c] = color;
		++c;
	}
	sortColors(out_palette.colors, c);
	return true;
}

static bool mergePaletteIntoSet(PaletteSet& out_palette_set, const Palette palette)
{
	for(uint32_t p = 0; p < kPaletteMaxCount; ++p)
	{
		Palette& set_palette = out_palette_set.palettes[p];
		if(mergePalettes(set_palette, palette, set_palette))
		{
			return true;
		}
	}

	return false;
}

static bool extractPalettes(PaletteSet& out_palette_set, const Image& image)
{
	return iterateImageTiles(
		image,
		[&image, &out_palette_set](const ColorRGBA* tile_pixels, uint32_t tile_column, uint32_t tile_row)
		{
			Palette tile_palette;
			if(!extractTilePalette(tile_palette, tile_pixels, image.getWidth()))
			{
				cout << "Could not extract tile palette for tile (" << tile_column << "," << tile_row << ")" << endl;
				return false;
			}
			if(!mergePaletteIntoSet(out_palette_set, tile_palette))
			{
				cout << "Could not merge palette for tile (" << tile_column << "," << tile_row << ")" << endl;
				return false;
			}
			return true;
		});
}

static bool writePaletteSet(const PaletteSet& palette_set, const char* filename)
{
	FILE* file = fopen(filename, "wb");
	if(!file)
	{
		cout << "Could not open file [" << filename << "]" << endl;
		return false;
	}

	const size_t written = fwrite(palette_set.palettes, sizeof(Palette), kPaletteMaxCount, file);
	fclose(file);
	return written == kPaletteMaxCount;
}

////////////////////////////////////////////////////////////////////////////////
// Tile
////////////////////////////////////////////////////////////////////////////////

enum TileFlipType : uint32_t
{
	kTileFlipType_None,
	kTileFlipType_Horizontal,
	kTileFlipType_Vertical,
	kTileFlipType_Both,
	kTileFlipType_Count,
};

struct TileFlip
{
	union
	{
		uint32_t rows[kTileSize];
		uint64_t values[kTileSize * sizeof(uint32_t) / sizeof(uint64_t)];
	};

	bool operator<(const TileFlip& other) const
	{
		return tie(values[0], values[1], values[2], values[3]) < tie(other.values[0], other.values[1], other.values[2], other.values[3]);
	}
};

struct Tile
{
	TileFlip flips[kTileFlipType_Count];
	uint32_t palette_index;
};

struct TileFlipIndex
{
	size_t index;
	TileFlipType flip_type;
};

struct Tileset
{
	vector<Tile> tiles;
	map<TileFlip, TileFlipIndex> flipToIndex;
};

static void generateTileFlips(Tile& out_tile)
{
	const TileFlip& none = out_tile.flips[kTileFlipType_None];
	{
		TileFlip& horizontal = out_tile.flips[kTileFlipType_Horizontal];
		for(uint32_t i = 0; i < kTileSize; ++i)
		{
			const uint8_t* src_bytes = reinterpret_cast<const uint8_t*>(none.rows + i);
			uint8_t* dst_bytes = reinterpret_cast<uint8_t*>(horizontal.rows + i);
			for(uint32_t b = 0; b < 4; ++b)
			{
				dst_bytes[b] =
					((src_bytes[b] & 0x01) << 7) |
					((src_bytes[b] & 0x02) << 5) |
					((src_bytes[b] & 0x04) << 3) |
					((src_bytes[b] & 0x08) << 1) |
					((src_bytes[b] & 0x10) >> 1) |
					((src_bytes[b] & 0x20) >> 3) |
					((src_bytes[b] & 0x40) >> 5) |
					((src_bytes[b] & 0x80) >> 7);
			}
		}
	}
	{
		TileFlip& vertical = out_tile.flips[kTileFlipType_Vertical];
		for(uint32_t i = 0; i < kTileSize; ++i)
		{
			vertical.rows[(kTileSize - 1) - i] = none.rows[i];
		}
	}
	{
		TileFlip& both = out_tile.flips[kTileFlipType_Both];
		const TileFlip& horizontal = out_tile.flips[kTileFlipType_Horizontal];
		for(uint32_t i = 0; i < kTileSize; ++i)
		{
			both.rows[(kTileSize - 1) - i] = horizontal.rows[i];
		}
	}
}

static bool isSuperPalette(const Palette& super_palette, const Palette& sub_palette)
{
	for(uint32_t i = 0; i < kColorsPerPalette; ++i)
	{
		const ColorBGR555 color = sub_palette.colors[i];
		if(color == kBGR555_Invalid)
		{
			break;
		}
		bool found = false;
		for(uint32_t j = 0; j < kColorsPerPalette; ++j)
		{
			if(color == super_palette.colors[j])
			{
				found = true;
				break;
			}
		}
		if(!found)
		{
			return false;
		}
	}
	return true;
}

static bool findMatchingPaletteInSet(uint32_t& out_palette_index, const PaletteSet& palette_set, const Palette& palette)
{
	for(uint32_t i = 0; i < kPaletteMaxCount; ++i)
	{
		if(isSuperPalette(palette_set.palettes[i], palette))
		{
			out_palette_index = i;
			return true;
		}
	}
	return false;
}

static bool extractTile(Tile& out_tile, const PaletteSet& palette_set, const ColorRGBA* pixels, uint32_t row_pitch)
{
	Palette tile_palette;
	{
		Palette palette;
		if(!extractTilePalette(palette, pixels, row_pitch))
		{
			cout << "Could not extract tile color" << endl;
			return false;
		}
		uint32_t palette_index;
		if(!findMatchingPaletteInSet(palette_index, palette_set, palette))
		{
			cout << "Could not find a matching palette in the palette set" << endl;
			return false;
		}
		tile_palette = palette_set.palettes[palette_index];
		out_tile.palette_index = palette_index;
	}

	auto getColorIndex = [&tile_palette](ColorBGR555 color)
	{
		for(uint32_t i = 0; i < kColorsPerPalette; ++i)
		{
			if(color == tile_palette.colors[i])
			{
				return i;
			}
		}
		return static_cast<uint32_t>(kColorIndex_Invalid);
	};

	TileFlip& none = out_tile.flips[kTileFlipType_None];
	for(uint32_t j = 0; j < kTileSize; ++j)
	{
		uint32_t& row_value = none.rows[j];
		row_value = 0;
		uint8_t* bytes = reinterpret_cast<uint8_t*>(&row_value);

		const ColorRGBA* row_pixels = pixels + (j * row_pitch);
		for(uint32_t i = 0; i < kTileSize; ++i)
		{
			const ColorBGR555 color = convertColor(row_pixels[i]);
			const size_t color_index = getColorIndex(color);
			if(color_index == kColorIndex_Invalid)
			{
				cout << "The color referenced by the tile was not found in the tile palette" << endl;
				return false;
			}
			assert(color_index < kColorsPerPalette);
			const uint32_t shift = (kTileSize - 1) - i;
			bytes[0] |= ((color_index >> 0) & 0x1) << shift;
			bytes[1] |= ((color_index >> 1) & 0x1) << shift;
			bytes[2] |= ((color_index >> 2) & 0x1) << shift;
			bytes[3] |= ((color_index >> 3) & 0x1) << shift;
		}
	}

	generateTileFlips(out_tile);
	return true;
}

static bool extractTileset(Tileset& out_tileset, const PaletteSet& palette_set, const Image& image)
{
	const bool success = iterateImageTiles(
		image,
		[&image, &palette_set, &out_tileset](const ColorRGBA* tile_pixels, uint32_t tile_column, uint32_t tile_row)
		{
			Tile tile;
			if(!extractTile(tile, palette_set, tile_pixels, image.getWidth()))
			{
				cout << "Could not extract tile (" << tile_column << "," << tile_row << ")" << endl;
				return false;
			}
			if(out_tileset.flipToIndex.find(tile.flips[kTileFlipType_None]) != out_tileset.flipToIndex.end())
			{
				return true;
			}

			const size_t tile_index = out_tileset.tiles.size();
			for(uint32_t i = 0; i < kTileFlipType_Count; ++i)
			{
				TileFlipIndex flip_index = {};
				flip_index.index = tile_index;
				flip_index.flip_type = static_cast<TileFlipType>(i);
				out_tileset.flipToIndex.insert(pair<TileFlip, TileFlipIndex>(tile.flips[i], flip_index));
			}
			out_tileset.tiles.push_back(tile);
			return true;
		});
	if(!success)
	{
		return false;
	}
	const size_t tile_count = out_tileset.tiles.size();
	if(tile_count > kTilesMaxCount)
	{
		cout << "Too many tiles in the tileset (" << tile_count << " > " << kTilesMaxCount << ")" << endl;
		return false;
	}
	return true;
}

static bool writeTileset(const Tileset& tileset, const char* filename)
{
	FILE* file = fopen(filename, "wb");
	if(!file)
	{
		cout << "Could not open file [" << filename << "]" << endl;
		return false;
	}
	bool success = true;
	const vector<Tile>& tiles = tileset.tiles;
	for(size_t i = 0; i < tiles.size(); ++i)
	{
		constexpr uint32_t kTileDataSize = kTileSize * sizeof(uint32_t);
		uint8_t tile_data[kTileDataSize];

		const TileFlip& flip = tiles[i].flips[kTileFlipType_None];
		for(uint32_t t = 0; t < kTileSize; ++t)
		{
			const uint32_t row = flip.rows[t];
			tile_data[t * 2 +  0] = (row >>  0) & 0x000000FF;
			tile_data[t * 2 +  1] = (row >>  8) & 0x000000FF;
			tile_data[t * 2 + 16] = (row >> 16) & 0x000000FF;
			tile_data[t * 2 + 17] = (row >> 24) & 0x000000FF;
		}

		const size_t written = fwrite(tile_data, kTileDataSize, 1, file);
		if(written != 1)
		{
			cout << "Could not write tile [" << i << "]" << endl;
			success = false;
			break;
		}
	}

	fclose(file);
	return success;
}

////////////////////////////////////////////////////////////////////////////////
// Tilemap
////////////////////////////////////////////////////////////////////////////////

struct Tilemap
{
	vector<uint16_t> parameters;
};

static bool extractTilemaps(Tilemap& out_tilemap, const Tileset& tileset, const PaletteSet& palette_set, const Image& image)
{
	out_tilemap.parameters.clear();

	const bool success = iterateImageTiles(
		image,
		[&image, &palette_set, &out_tilemap, &tileset](const ColorRGBA* tile_pixels, uint32_t tile_column, uint32_t tile_row)
		{
			Tile tile;
			if(!extractTile(tile, palette_set, tile_pixels, image.getWidth()))
			{
				cout << "Could not extract tile (" << tile_column << "," << tile_row << ")" << endl;
				return false;
			}

			const auto index_it = tileset.flipToIndex.find(tile.flips[kTileFlipType_None]);
			if(index_it == tileset.flipToIndex.end())
			{
				cout << "Could not find a corresponding tile in the tileset" << endl;
				return false;
			}

			const TileFlipIndex tile_flip_index = index_it->second;
			const uint8_t tile_index = tile_flip_index.index & 0xFF;
			const uint8_t palette_number = kPaletteFirstPaletteIndex + (tile.palette_index % kPaletteMaxCount);
			const uint8_t flip_type = tile_flip_index.flip_type & 0x3;

			const uint16_t parameters =
				tile_index |
				(palette_number << 10) |
				(flip_type << 14);
			out_tilemap.parameters.push_back(parameters);

			return true;
		});
	if(!success)
	{
		return false;
	}
	return true;
}

static bool writeTilemap(const Tilemap& tilemap, const char* filename)
{
	FILE* file = fopen(filename, "wb");
	if(!file)
	{
		cout << "Could not open file [" << filename << "]" << endl;
		return false;
	}

	const size_t data_size = tilemap.parameters.size() * sizeof(tilemap.parameters[0]);
	const size_t written = fwrite(tilemap.parameters.data(), 1, data_size, file);
	const bool success = written == data_size;
	if(!success)
	{
		cout << "Could not write the data to file [" << filename << "]" << endl;
	}
	fclose(file);
	return success;
}

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char** argv)
{
	if(argc < 2)
	{
		cout
			<< "USAGE" << endl
			<< argv[0] << " <border_png_filename>" << endl;
		return 0;
	}

	const char* filename = argv[1];
	Image image;
	if(!image.read(filename))
	{
		cout << "Could not read file [" << filename << "]" << endl;
		return 1;
	}
	if(!image.validateSize())
	{
		cout
			<< "The image size (" << image.getWidth() << "x" << image.getHeight() << ") is invalid "
			<< "for file [" << filename << "]" << endl;
		return 1;
	}

	PaletteSet palette_set = {};
	{
		if(!extractPalettes(palette_set, image))
		{
			cout << "Could not extract palettes for file [" << image.getFilename() << "]" << endl;
			return 1;
		}
		if(!writePaletteSet(palette_set, getOutputFilename(image.getFilename(), ".pal").c_str()))
		{
			cout << "Could not write the palette file" << endl;
			return 1;
		}
	}

	Tileset tileset;
	{
		if(!extractTileset(tileset, palette_set, image))
		{
			cout << "Could not extract the tiles for file [" << image.getFilename() << "]" << endl;
			return 1;
		}
		if(!writeTileset(tileset, getOutputFilename(image.getFilename(), ".chr").c_str()))
		{
			cout << "Could not write the tileset file" << endl;
			return 1;
		}
	}

	Tilemap tilemap;
	if(!extractTilemaps(tilemap, tileset, palette_set, image))
	{
		cout << "Could not extract the tiles for file [" << image.getFilename() << "]" << endl;
		return 1;
	}
	if(!writeTilemap(tilemap, getOutputFilename(image.getFilename(), ".tlm").c_str()))
	{
		cout << "Could not write the tilemap files for file [" << image.getFilename() << "]" << endl;
		return 1;
	}

	return 0;
}

