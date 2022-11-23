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
	kPaletteMaxCount = 4,
	kTilesMaxCount = 256,
	kTilemapIndexMaxCount = 20 * 18,

	kColorIndex_Invalid = 0xFFFFFFFFU,
	kPaletteIndex_Invalid = 0xFFFFFFFFU,
};

enum : uint16_t
{
	kBGR555_Invalid = 0x8000U,
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

static bool extractTilePalette(Palette& out_tile_palette, const ColorRGBA* pixels, uint32_t row_pitch, const ColorBGR555 color0)
{
	set<ColorBGR555> colors;
	for(uint32_t j = 0; j < kTileSize; ++j)
	{
		const ColorRGBA* row_pixels = pixels + (j * row_pitch);
		for(uint32_t i = 0; i < kTileSize; ++i)
		{
			const ColorBGR555 c = convertColor(row_pixels[i]);
			if(c != color0)
			{
				colors.insert(c);
			}
		}
	}

	uint32_t color_offset = 0;
	if(color0 == kBGR555_Invalid)
	{
		if(colors.size() > kColorsPerPalette)
		{
			cout << "Too many colors (" << colors.size() << ") in tile" << endl;
			return false;
		}
	}
	else
	{
		out_tile_palette.colors[0] = color0;
		color_offset = 1;
		if(colors.size() > kColorsPerPalette - 1)
		{
			cout << "Too many colors (" << colors.size() + 1 << ") in tile" << endl;
			return false;
		}
	}

	uint32_t c = color_offset;
	for(ColorBGR555 color : colors)
	{
		out_tile_palette.colors[c] = color;
		++c;
	}
	sortColors(out_tile_palette.colors + color_offset, c - color_offset);

	return true;
}

static bool mergePalettes(Palette& out_palette, const Palette lhs, const Palette rhs, ColorBGR555 color0)
{
	set<ColorBGR555> colors;
	for(uint32_t i = 0; i < kColorsPerPalette; ++i)
	{
		if(lhs.colors[i] == kBGR555_Invalid)
		{
			break;
		}
		if(lhs.colors[i] == color0)
		{
			continue;
		}
		colors.insert(lhs.colors[i]);
	}
	for(uint32_t i = 0; i < kColorsPerPalette; ++i)
	{
		if(rhs.colors[i] == kBGR555_Invalid)
		{
			break;
		}
		if(rhs.colors[i] == color0)
		{
			continue;
		}
		colors.insert(rhs.colors[i]);
	}

	uint32_t color_offset = 0;
	if(color0 != kBGR555_Invalid)
	{
		color_offset = 1;
		out_palette.colors[0] = color0;
	}

	if(colors.size() > kColorsPerPalette - color_offset)
	{
		return false;
	}

	uint32_t c = color_offset;
	for(ColorBGR555 color : colors)
	{
		out_palette.colors[c] = color;
		++c;
	}
	sortColors(out_palette.colors + color_offset, c - color_offset);
	return true;
}

static bool mergePaletteIntoSet(PaletteSet& out_palette_set, const Palette palette)
{
	for(uint32_t p = 0; p < kPaletteMaxCount; ++p)
	{
		Palette& set_palette = out_palette_set.palettes[p];
		if(mergePalettes(set_palette, palette, set_palette, set_palette.colors[0]))
		{
			return true;
		}
	}

	return false;
}

static bool setColor0(PaletteSet& out_palette_set, const Image& image)
{
	map<ColorBGR555, uint32_t> colors;
	if(!iterateImageTiles(
		image,
		[&image, &colors](const ColorRGBA* tile_pixels, uint32_t tile_column, uint32_t tile_row)
		{
			Palette tile_palette;
			if(!extractTilePalette(tile_palette, tile_pixels, image.getWidth(), kBGR555_Invalid))
			{
				cout << "Could not extract tile palette for tile (" << tile_column << "," << tile_row << ")" << endl;
				return false;
			}
			uint32_t palette_size = 0;
			for(uint32_t i = 0; i < kColorsPerPalette; ++i)
			{
				const ColorBGR555 color = tile_palette.colors[i];
				if(color == kBGR555_Invalid)
				{
					break;
				}
				++palette_size;
			}
			for(uint32_t i = 0; i < kColorsPerPalette; ++i)
			{
				const ColorBGR555 color = tile_palette.colors[i];
				if(color == kBGR555_Invalid)
				{
					break;
				}
				auto colorIt = colors.find(color);
				if(colorIt == colors.end())
				{
					colors.insert(pair<ColorBGR555, uint32_t>(color, 1));
				}
				else
				{
					colorIt->second += palette_size;
				}
			}
			return true;
		}))
	{
		return false;
	}
	assert(colors.size() > 0);

	auto it = colors.begin();
	auto most_used_it = it;
	++it;
	for(; it != colors.end(); ++it)
	{
		if( (it->second > most_used_it->second) ||
			(it->second == most_used_it->second && getLuminance(it->first) < getLuminance(most_used_it->first)))
		{
			most_used_it = it;
		}
	}
	const ColorBGR555 most_used_color = most_used_it->first;
	assert(most_used_color != kBGR555_Invalid);

	for(uint32_t i = 0; i < kPaletteMaxCount; ++i)
	{
		out_palette_set.palettes[i].colors[0] = most_used_color;
	}
	return true;
}

static bool extractPalettes(PaletteSet& out_palette_set, const Image& image)
{
	if(!setColor0(out_palette_set, image))
	{
		return false;
	}
	return iterateImageTiles(
		image,
		[&image, &out_palette_set](const ColorRGBA* tile_pixels, uint32_t tile_column, uint32_t tile_row)
		{
			Palette tile_palette;
			if(!extractTilePalette(tile_palette, tile_pixels, image.getWidth(), out_palette_set.palettes[0].colors[0]))
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

struct Tile
{
	uint16_t rows[kTileSize];
	uint32_t palette_index;
};

struct Tileset
{
	vector<Tile> tiles;
};

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
		if(!extractTilePalette(palette, pixels, row_pitch, palette_set.palettes[0].colors[0]))
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

	for(uint32_t j = 0; j < kTileSize; ++j)
	{
		uint16_t& row_value = out_tile.rows[j];
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

			for(uint32_t i = 0; i < out_tileset.tiles.size(); ++i)
			{
				if(memcmp(&tile, &out_tileset.tiles[i], sizeof(Tile)) == 0)
				{
					return true;
				}
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

	const size_t written = fwrite(tileset.tiles.data(), sizeof(Tile), tileset.tiles.size(), file);
	fclose(file);
	return written == tileset.tiles.size();
}

////////////////////////////////////////////////////////////////////////////////
// Tilemap
////////////////////////////////////////////////////////////////////////////////

struct Tilemap
{
	vector<uint8_t> indices;
	vector<uint8_t> attributes;
};

static bool extractTilemaps(Tilemap& out_tilemap, const Tileset& tileset, const PaletteSet& palette_set, const Image& image)
{
	out_tilemap.indices.clear();
	out_tilemap.attributes.clear();

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

			bool found = false;
			uint8_t tile_index = 0;
			for(uint32_t i = 0; i < tileset.tiles.size(); ++i)
			{
				if(memcmp(&tile, &tileset.tiles[i], sizeof(Tile)) == 0)
				{
					tile_index = i;
					found = true;
					break;
				}
			}
			if(!found)
			{
				cout << "Could not find a corresponding tile in the tileset" << endl;
				return false;
			}

			out_tilemap.indices.push_back(tile_index);
			out_tilemap.attributes.push_back(tile.palette_index & 0x3);
			return true;
		});
	if(!success)
	{
		return false;
	}
	const size_t index_count = out_tilemap.indices.size();
	assert(index_count == out_tilemap.attributes.size());
	if(index_count > kTilemapIndexMaxCount)
	{
		cout << "Too many indices in the tilemap (" << index_count << " > " << kTilemapIndexMaxCount << ")" << endl;
		return false;
	}
	return true;
}

static bool writeTilemap(const Tilemap& tilemap, const char* index_filename, const char* attribute_filename)
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
		fclose(file);
		return true;
	};

	const size_t index_count = tilemap.indices.size();
	if(!writeData(tilemap.indices.data(), index_count * sizeof(tilemap.indices[0]), index_filename))
	{
		cout << "Could not write the index file [" << index_filename << "]" << endl;
		return false;
	}

	assert(tilemap.attributes.size() % 4 == 0);
	vector<uint8_t> attributes;
	for(uint32_t i = 0; i < tilemap.attributes.size(); i += 4)
	{
		attributes.push_back(
			(tilemap.attributes[i + 0] << 0) |
			(tilemap.attributes[i + 1] << 2) |
			(tilemap.attributes[i + 2] << 4) |
			(tilemap.attributes[i + 3] << 6));
	}
	size_t test = attributes.size() * sizeof(attributes[0]);
	if(!writeData(attributes.data(), attributes.size() * sizeof(attributes[0]), attribute_filename))
	{
		cout << "Could not write the parameter file [" << attribute_filename << "]" << endl;
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

	for(uint32_t i = 1; i < images.size(); ++i)
	{
		Tilemap tilemap;
		if(!extractTilemaps(tilemap, tileset, palette_set, images[i]))
		{
			cout << "Could not extract the tiles for file [" << images[i].getFilename() << "]" << endl;
			return 1;
		}
		if(!writeTilemap(tilemap, getOutputFilename(images[i].getFilename(), ".tlm").c_str(), getOutputFilename(images[i].getFilename(), ".atr").c_str()))
		{
			cout << "Could not write the tilemap files for file [" << images[i].getFilename() << "]" << endl;
			return 1;
		}
	}

	return 0;
}

