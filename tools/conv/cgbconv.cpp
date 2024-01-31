#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "third_party/stb_image.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////

enum : uint32_t
{
	kTileSize = 8,
	kColorsPerPalette = 4,
	kPaletteMaxCount = 8,
	kVramBankCount = 2,
	kTilesPerVramBank = 256,
	kTilesMaxCount = kTilesPerVramBank * kVramBankCount,
	kTilemapIndexMaxCount = 1024,

	kColorIndex_Invalid = 0xFFFFFFFFU,
	kPaletteIndex_Invalid = 0xFFFFFFFFU,
};

enum : uint16_t
{
	kBGR555_Invalid = 0x8000U,
};

////////////////////////////////////////////////////////////////////////////////
// Export parameters
////////////////////////////////////////////////////////////////////////////////

struct ExportParameters
{
	bool export_palettes;
	bool optimize_tileset;
	bool generate_tilemaps;
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
	const double blue = ((color >> 10) & 0x1F) / 32.0;
	const double green = ((color >> 5) & 0x1F) / 32.0;
	const double red = (color & 0x1F) / 32.0;
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
	ColorRGBA* m_pixels;
	int32_t m_width;
	int32_t m_height;
};

typedef vector<Image> ImageList;

Image::Image()
: m_pixels(nullptr)
, m_width(0)
, m_height(0)
{

}

Image::~Image()
{
	if(m_pixels == nullptr)
	{
		return;
	}
	stbi_image_free(m_pixels);
	m_pixels = nullptr;
}

bool Image::read(const char* filename)
{
	assert(filename);
	int32_t num_channels = 0;
	m_pixels = reinterpret_cast<ColorRGBA*>(stbi_load(filename, &m_width, &m_height, &num_channels, 4));
	m_filename = filename;
	return m_pixels != nullptr;
}

bool Image::validateSize() const
{
	return ((getWidth() % kTileSize) == 0) && ((getHeight() % kTileSize) == 0);
}

uint32_t Image::getWidth() const
{
	return m_width;
}

uint32_t Image::getHeight() const
{
	return m_height;
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
			for(uint32_t b = 0; b < 2; ++b)
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
			const uint32_t shift = (kTileSize - 1) - i;
			bytes[0] |= (color_index & 0x1) << shift;
			bytes[1] |= ((color_index >> 1) & 0x1) << shift;
		}
	}

	generateTileFlips(out_tile);
	return true;
}

static bool extractTileset(Tileset& out_tileset, const PaletteSet& palette_set, const Image& image, bool optimize_tileset)
{
	const bool success = iterateImageTiles(
		image,
		[&image, &palette_set, optimize_tileset, &out_tileset](const ColorRGBA* tile_pixels, uint32_t tile_column, uint32_t tile_row)
		{
			Tile tile;
			if(!extractTile(tile, palette_set, tile_pixels, image.getWidth()))
			{
				cout << "Could not extract tile (" << tile_column << "," << tile_row << ")" << endl;
				return false;
			}
			if(optimize_tileset && out_tileset.flipToIndex.find(tile.flips[kTileFlipType_None]) != out_tileset.flipToIndex.end())
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
		const Tile& tile = tiles[i];
		const size_t written = fwrite(&tile.flips[kTileFlipType_None], sizeof(TileFlip), 1, file);
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
	vector<uint8_t> indices;
	vector<uint8_t> parameters;
};

static bool extractTilemaps(Tilemap& out_tilemap, const Tileset& tileset, const PaletteSet& palette_set, const Image& image)
{
	out_tilemap.indices.clear();
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
			const size_t index = tile_flip_index.index;
			const uint8_t tile_index = index % kTilesPerVramBank;
			out_tilemap.indices.push_back(tile_index);

			const uint8_t palette_number = tile.palette_index & 0x7;
			const uint8_t vram_bank = (index / kTilesPerVramBank) & 0x1;
			const uint8_t flip_type = tile_flip_index.flip_type;

			const uint8_t parameters =
				palette_number |
				(vram_bank << 3) |
				(flip_type << 5);
			out_tilemap.parameters.push_back(parameters);
			return true;
		});
	if(!success)
	{
		return false;
	}
	const size_t index_count = out_tilemap.indices.size();
	assert(index_count == out_tilemap.parameters.size());
	if(index_count > kTilemapIndexMaxCount)
	{
		cout << "Too many indices in the tilemap (" << index_count << " > " << kTilemapIndexMaxCount << ")" << endl;
		return false;
	}
	return true;
}

static bool writeTilemap(const Tilemap& tilemap, const char* index_filename, const char* parameter_filename)
{
	auto writeData = [](const uint8_t* data, size_t data_size, const char* filename)
	{
		FILE* file = fopen(filename, "wb");
		if(!file)
		{
			cout << "Could not open file [" << filename << "]" << endl;
			return false;
		}
		const size_t written = fwrite(data, 1, data_size, file);
		if(written != data_size)
		{
			cout << "Could not write the data to file [" << filename << "]" << endl;
			fclose(file);
			return false;
		}
		const size_t padding_size = kTilemapIndexMaxCount - data_size;
		for(uint32_t i = 0; i < padding_size; ++i)
		{
			const uint8_t padding = 0;
			const size_t written = fwrite(&padding, sizeof(padding), 1, file);
			if(written != 1)
			{
				cout << "Could not write the padding to file [" << filename << "]" << endl;
				fclose(file);
				return false;
			}
		}
		fclose(file);
		return true;
	};

	const size_t index_count = tilemap.indices.size();
	if(!writeData(tilemap.indices.data(), index_count * sizeof(tilemap.indices[0]), index_filename))
	{
		cout << "Could not write the index file [" << index_filename << "]" << endl;
		return false;
	}
	if(!writeData(tilemap.parameters.data(), index_count * sizeof(tilemap.parameters[0]), parameter_filename))
	{
		cout << "Could not write the parameter file [" << parameter_filename << "]" << endl;
		return false;
	}
	return true;
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
			<< argv[0] << " [-mpt] <tileset_png_filename> [tilemap_png ...]" << endl
			<< "Options" << endl
			<< "\tm\tGenerate tilemaps" << endl
			<< "\tp\tOutput a palette file" << endl
			<< "\tt\tOptimize the tileset" << endl;
		return 0;
	}

	ExportParameters parameters = {};

	int32_t first_filename = 1;
	if(argv[1][0] == '-')
	{
		const char* options = argv[1];
		const size_t option_len = strlen(options);
		for(size_t i = 1; i < option_len; ++i)
		{
			const char o = options[i];
			switch(o)
			{
				case 'm':
				{
					parameters.generate_tilemaps = true;
					break;
				}
				case 'p':
				{
					parameters.export_palettes = true;
					break;
				}
				case 't':
				{
					parameters.optimize_tileset = true;
					break;
				}
				default:
				{
					cout << "Unknown option: [" << o << "]" << endl;
					break;
				}
			}
		}
		first_filename = 2;
	}

	ImageList images;
	{
		images.resize(argc - first_filename);
		for(int32_t i = 0; i < images.size(); ++i)
		{
			const char* filename = argv[i + first_filename];
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
		if(parameters.export_palettes && !writePaletteSet(palette_set, getOutputFilename(images[0].getFilename(), ".pal").c_str()))
		{
			cout << "Could not write the palette file" << endl;
			return 1;
		}
	}

	Tileset tileset;
	{
		if(!extractTileset(tileset, palette_set, images[0], parameters.optimize_tileset))
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

	if(parameters.generate_tilemaps)
	{
		for(uint32_t i = 1; i < images.size(); ++i)
		{
			Tilemap tilemap;
			if(!extractTilemaps(tilemap, tileset, palette_set, images[i]))
			{
				cout << "Could not extract the tiles for file [" << images[i].getFilename() << "]" << endl;
				return 1;
			}
			if(!writeTilemap(tilemap, getOutputFilename(images[i].getFilename(), ".idx").c_str(), getOutputFilename(images[i].getFilename(), ".prm").c_str()))
			{
				cout << "Could not write the tilemap files for file [" << images[i].getFilename() << "]" << endl;
				return 1;
			}
		}
	}

	return 0;
}

