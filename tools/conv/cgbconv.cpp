#include <algorithm>
#include <cassert>
#include <cstdint>
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
	kColorsPerPalette = 4,
	kPaletteMaxCount = 8,

	kColorIndex_Invalid = 0xFFFFFFFFU,
	kPaletteIndex_Invalid = 0xFFFFFFFFU,
};

enum : uint16_t
{
	kBGR555_Invalid = 0x8000,
};

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
		colors, colors + count,
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
	}

	union
	{
		ColorBGR555 colors[kColorsPerPalette];
		uint64_t value;
	};
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

	uint32_t c = 0;
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

	uint32_t c = 0;
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

	assert(sizeof(Palette) == 8);
	const size_t written = fwrite(palette_set.palettes, sizeof(Palette), kPaletteMaxCount, file);
	fclose(file);
	return written == kPaletteMaxCount;
}

////////////////////////////////////////////////////////////////////////////////
// Tile
////////////////////////////////////////////////////////////////////////////////

enum : uint32_t
{
	kTileFlip_None,
	kTileFlip_Horizontal,
	kTileFlip_Vertical,
	kTileFlip_Both,
	kTileFlip_Count,
};

struct TileFlip
{
	union
	{
		uint16_t rows[kTileSize];
		uint64_t values[kTileSize * sizeof(uint16_t) / sizeof(uint64_t)];
	};

	bool operator<(const TileFlip& other) const
	{
		return tie(values[0], values[1]) < tie(other.values[0], other.values[1]);
	}
};

struct Tile
{
	TileFlip flips[kTileFlip_Count];
};

struct Tileset
{
	vector<Tile> tiles;
	map<TileFlip, size_t> flipToIndex;
};

static void generateTileFlips(Tile& out_tile)
{
	const TileFlip& none = out_tile.flips[kTileFlip_None];
	{
		TileFlip& horizontal = out_tile.flips[kTileFlip_Horizontal];
		for(uint32_t i = 0; i < kTileSize; ++i)
		{
			const uint16_t row = none.rows[i];
			horizontal.rows[i] =
				((row >> 14) & 0x3) |
				(((row >> 12) & 0x3) << 2) |
				(((row >> 10) & 0x3) << 4) |
				(((row >> 8) & 0x3) << 6) |
				(((row >> 6) & 0x3) << 8) |
				(((row >> 4) & 0x3) << 10) |
				(((row >> 2) & 0x3) << 12) |
				((row & 0x3) << 14);
		}
	}
	{
		TileFlip& vertical = out_tile.flips[kTileFlip_Vertical];
		for(uint32_t i = 0; i < kTileSize; ++i)
		{
			vertical.rows[(kTileSize - 1) - i] = none.rows[i];
		}
	}
	{
		TileFlip& both = out_tile.flips[kTileFlip_Both];
		const TileFlip& horizontal = out_tile.flips[kTileFlip_Horizontal];
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

	TileFlip& none = out_tile.flips[kTileFlip_None];
	for(uint32_t j = 0; j < kTileSize; ++j)
	{
		uint16_t& row_value = none.rows[j];
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
			bytes[0] |= (color_index & 0x1) << i;
			bytes[1] |= ((color_index >> 1) & 0x1) << i;
		}
	}

	generateTileFlips(out_tile);
	return true;
}

static bool extractTileset(Tileset& out_tileset, const PaletteSet& palette_set, const Image& image)
{
	return iterateImageTiles(
		image,
		[&image, &palette_set, &out_tileset](const ColorRGBA* tile_pixels, uint32_t tile_column, uint32_t tile_row)
		{
			Tile tile;
			if(!extractTile(tile, palette_set, tile_pixels, image.getWidth()))
			{
				cout << "Could not extract tile (" << tile_column << "," << tile_row << ")" << endl;
				return false;
			}
			if(out_tileset.flipToIndex.find(tile.flips[0]) != out_tileset.flipToIndex.end())
			{
				return true;
			}

			const size_t tile_index = out_tileset.tiles.size();
			for(uint32_t i = 0; i < kTileFlip_Count; ++i)
			{
				out_tileset.flipToIndex.insert(pair<TileFlip, size_t>(tile.flips[i], tile_index));
			}
			out_tileset.tiles.push_back(tile);
			return true;
		});
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
		const Tile& tile = tiles[i];
		const size_t written = fwrite(&tile.flips[kTileFlip_None], sizeof(TileFlip), 1, file);
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
// main
////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char** argv)
{
	if(argc < 2)
	{
		cout
			<< "USAGE" << endl
			<< argv[0] << " <tileset_png_filename> [tilemap_png ...]" << endl;
		return 0;
	}

	ImageList images;
	{
		images.resize(argc - 1);
		for(int32_t i = 0; i < images.size(); ++i)
		{
			const char* filename = argv[i + 1];
			Image& image = images[i];
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
		}
	}

	PaletteSet palette_set = {};
	{
		if(!extractPalettes(palette_set, images[0]))
		{
			cout << "Could not extract palettes for file [" << images[0].getFilename() << "]" << endl;
			return 1;
		}
		if(!writePaletteSet(palette_set, getOutputFilename(images[0].getFilename(), ".pal").c_str()))
		{
			cout << "Could not write the palette file" << endl;
			return 1;
		}
	}

	Tileset tileset;
	{
		if(!extractTileset(tileset, palette_set, images[0]))
		{
			cout << "Could not extract the tiles for file [" << images[0].getFilename() << "]" << endl;
			return 1;
		}
		if(!writeTileset(tileset, getOutputFilename(images[0].getFilename(), ".chr").c_str()))
		{
			cout << "Could not write the tileset file" << endl;
			return 1;
		}
	}

	{
		// TODO Extract maps
		// For each map
		// - Extract each tile flip
		// - Find the flip into the tileset
		// - Build the index and parameter maps
		// - Indices into .tm1
		// - Parameters into .tm2
	}

	return 0;
}
