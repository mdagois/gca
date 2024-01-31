#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "third_party/stb_image.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Color
////////////////////////////////////////////////////////////////////////////////

enum : uint32_t
{
	kMaxColorCount = 4,
};

struct Color
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

typedef vector<Color> ColorSet;

static double computeBrightness(const Color color)
{
	return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
}

static void sort(ColorSet& color_set)
{
	sort(
		color_set.begin(), color_set.end(),
		[](const Color lhs, const Color rhs)
		{
			return computeBrightness(lhs) > computeBrightness(rhs);
		});
}

static bool checkColorSetCompatibility(const ColorSet& lhs, const ColorSet& rhs)
{
	auto hasColor= [](const ColorSet& color_set, const Color color)
	{
		for(ColorSet::const_iterator it = color_set.cbegin(); it != color_set.cend(); ++it)
		{
			if(it->rgba == color.rgba)
			{
				return true;
			}
		}
		return false;
	};

	if(lhs.size() < rhs.size())
	{
		for(ColorSet::const_iterator it = lhs.cbegin(); it != lhs.cend(); ++it)
		{
			if(!hasColor(rhs, *it))
			{
				return false;
			}
		}
	}
	else
	{
		for(ColorSet::const_iterator it = rhs.cbegin(); it != rhs.cend(); ++it)
		{
			if(!hasColor(lhs, *it))
			{
				return false;
			}
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
// Tiles
////////////////////////////////////////////////////////////////////////////////

enum : uint32_t
{
	kTileSize = 8,
	kBytesPerTile = 16,
	kTileSetTilesCount = 384,
	kFirstTilemapTileIndex = 128,
};

struct Tile
{
	uint8_t byte[kBytesPerTile];
};

static bool operator==(const Tile& lhs, const Tile& rhs)
{
	return memcmp(&lhs, &rhs, sizeof(Tile)) == 0;
}

typedef vector<Tile> TileSet;
typedef uint8_t TileIndex;
typedef vector<TileIndex> Tilemap;

static void extractTileFromPixels(Tile& out_tile, const Color* pixels, uint32_t row_pitch, const ColorSet& color_set)
{
	auto getColorIndex = [](const ColorSet& color_set, const Color color)
	{
		for(size_t i = 0; i < color_set.size(); ++i)
		{
			if(color_set[i].rgba == color.rgba)
			{
				return static_cast<uint32_t>(i);
			}
		}

		assert(false);
		return 0U;
	};

	for(uint32_t j = 0; j < kTileSize; ++j)
	{
		const Color* row_pixels = pixels + j * row_pitch;
		uint32_t indices[kTileSize];
		for(uint32_t i = 0; i < kTileSize; ++i)
		{
			indices[i] = getColorIndex(color_set, row_pixels[i]);
			assert(indices[i] < kMaxColorCount);
		}

		uint8_t* bytes = out_tile.byte + j * 2;
		bytes[0] =
			((indices[0] << 7) & 0x80) | ((indices[1] << 6) & 0x40) | ((indices[2] << 5) & 0x20) | ((indices[3] << 4) & 0x10) |
			((indices[4] << 3) & 0x08) | ((indices[5] << 2) & 0x04) | ((indices[6] << 1) & 0x02) | ((indices[7] << 0) & 0x01);
		bytes[1] =
			((indices[0] << 6) & 0x80) | ((indices[1] << 5) & 0x40) | ((indices[2] << 4) & 0x20) | ((indices[3] << 3) & 0x10) |
			((indices[4] << 2) & 0x08) | ((indices[5] << 1) & 0x04) | ((indices[6] << 0) & 0x02) | ((indices[7] >> 1) & 0x01);
	}
}

static bool getTileIndexInTileSet(TileIndex& out_index, const Tile& tile, const TileSet& tileset)
{
	assert(tileset.size() == kTileSetTilesCount);

	for(uint32_t i = kFirstTilemapTileIndex; i < tileset.size(); ++i)
	{
		if(tile == tileset[i])
		{
			out_index = static_cast<TileIndex>(i);
			return true;
		}
	}

	return false;
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

	uint32_t extractColors(ColorSet& out_color_set) const;
	uint32_t extractTiles(TileSet& out_tileset, const ColorSet& color_set) const;

private:
	Color* m_pixels;
	int32_t m_width;
	int32_t m_height;
};

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
	m_pixels = reinterpret_cast<Color*>(stbi_load(filename, &m_width, &m_height, &num_channels, 4));
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

uint32_t Image::extractColors(ColorSet& out_color_set) const
{
	auto insert = [](ColorSet& set, Color color)
	{
		const size_t size = set.size();
		for(size_t i = 0; i < size; ++i)
		{
			if(color.rgba == set[i].rgba)
			{
				return;
			}
		}
		set.push_back(color);
	};

	const uint32_t image_width = getWidth();
	for(uint32_t j = 0; j < getHeight(); ++j)
	{
		for(uint32_t i = 0; i < image_width; ++i)
		{
			const Color pixel = m_pixels[image_width * j + i];
			insert(out_color_set, pixel);
		}
	}

	sort(out_color_set);
	return static_cast<uint32_t>(out_color_set.size());
}

uint32_t Image::extractTiles(TileSet& out_tileset, const ColorSet& color_set) const
{
	assert(validateSize());

	const uint32_t image_width = getWidth();
	const uint32_t column_count = image_width / kTileSize;
	const uint32_t row_count = getHeight() / kTileSize;
	for(uint32_t j = 0; j < row_count; ++j)
	{
		for(uint32_t i = 0; i < column_count; ++i)
		{
			Tile tile;
			extractTileFromPixels(tile, m_pixels + (j * image_width + i) * kTileSize, image_width, color_set);
			out_tileset.push_back(tile);
		}
	}

	return static_cast<uint32_t>(out_tileset.size());
}

////////////////////////////////////////////////////////////////////////////////
// File output
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

static bool writeTileset(const TileSet& tileset, const char* output_filename)
{
	FILE* file = fopen(output_filename, "wb");
	if(!file)
	{
		return false;
	}

	const size_t written = fwrite(tileset.data(), sizeof(Tile), tileset.size(), file);
	fclose(file);
	return written == tileset.size();
}

static bool writeTilemap(const Tilemap& tilemap, const char* output_filename)
{
	FILE* file = fopen(output_filename, "wb");
	if(!file)
	{
		return false;
	}

	const size_t written = fwrite(tilemap.data(), sizeof(TileIndex), tilemap.size(), file);
	fclose(file);
	return written == tilemap.size();
}

////////////////////////////////////////////////////////////////////////////////
// Main logic
////////////////////////////////////////////////////////////////////////////////

typedef vector<const char*> FileList;

static bool extractTilesFromFile(TileSet& out_tileset, ColorSet& inout_color_set, const char* filename)
{
	Image image;
	if(!image.read(filename))
	{
		cout << "FAILED: cannot read file [" << filename << "]" << endl;
		return false;
	}

	const uint32_t image_width = image.getWidth();
	const uint32_t image_height = image.getHeight();
	if(!image.validateSize())
	{
		cout
			<< "FAILED: invalid file [" << filename << "]" << endl
			<< "The image width and height must be multiples of 8 ("
			<< image_width << "x" << image_height << ") in [" << filename << "]" << endl;
		return false;
	}

	if(inout_color_set.empty())
	{
		const uint32_t color_count = image.extractColors(inout_color_set);
		if(color_count > kMaxColorCount)
		{
			cout << "FAILED: too many colors (got " << color_count << " colors) in [" << filename << "]" << endl;
			return false;
		}
	}
	else
	{
		ColorSet image_color_set;
		const uint32_t color_count = image.extractColors(image_color_set);
		if(inout_color_set.size() < image_color_set.size())
		{
			cout
				<< "FAILED: the tileset has less colors than the tilemap ("
				<< inout_color_set.size() << " < " << image_color_set.size() << ") in ["
				<< filename << "]" << endl;
			return false;
		}

		if(!checkColorSetCompatibility(inout_color_set, image_color_set))
		{
			cout << "FAILED: the tileset and tilemap colors are not compatible in [" << filename << "]" << endl;
			return false;
		}
	}

	image.extractTiles(out_tileset, inout_color_set);
	return true;
}

static bool exportTileset(TileSet& out_tileset, ColorSet& out_color_set, const char* filename)
{
	if(!extractTilesFromFile(out_tileset, out_color_set, filename))
	{
		return false;
	}

	if(out_tileset.size() != kTileSetTilesCount)
	{
		cout << "FAILED: the tileset must contain " << kTileSetTilesCount << " tiles in [" << filename << "]" << endl;
		return false;
	}

	const string output_filename = getOutputFilename(filename, ".chr");
	if(!writeTileset(out_tileset, output_filename.c_str()))
	{
		cout << "FAILED: cannot write the tile file [" << output_filename << "]" << endl;
		return false;
	}

	return true;
}

static bool exportTilemaps(const FileList& filenames, const TileSet& tileset, ColorSet& tileset_colors)
{
	if(filenames.empty())
	{
		return true;
	}

	for(FileList::const_iterator it = filenames.cbegin(); it != filenames.cend(); ++it)
	{
		const char* input_filename = *it;

		TileSet tiles;
		if(!extractTilesFromFile(tiles, tileset_colors, input_filename))
		{
			return false;
		}

		Tilemap tilemap;
		for(size_t i = 0; i < tiles.size(); ++i) 
		{
			TileIndex index;
			if(!getTileIndexInTileSet(index, tiles[i], tileset))
			{
				cout << "FAILED: could not find the tile [" << i << "] in the tileset [" << input_filename << "]" << endl;
				return false;
			}
			tilemap.push_back(index);
		}

		const string output_filename = getOutputFilename(input_filename, ".tlm");
		if(!writeTilemap(tilemap, output_filename.c_str()))
		{
			cout << "FAILED: cannot write the tilemap file [" << output_filename << "]" << endl;
			return false;
		}
	}

	return true;
}

int main(int argc, const char** argv)
{
	if(argc < 2)
	{
		cout
			<< "USAGE" << endl
			<< argv[0] << " <tileset_png_filename> [tilemap_png ...]" << endl;
		return 0;
	}

	const char* input_tileset_filename = argv[1];
	FileList input_tilemap_filenames;
	{
		for(int32_t i = 2; i < argc; ++i)
		{
			input_tilemap_filenames.push_back(argv[i]);
		}
	}

	TileSet tileset;
	ColorSet tileset_colors;
	const bool success =
		exportTileset(tileset, tileset_colors, input_tileset_filename) &&
		exportTilemaps(input_tilemap_filenames, tileset, tileset_colors);
	return !success;
}

