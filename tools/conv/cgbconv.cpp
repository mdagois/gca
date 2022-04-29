#include <cassert>
#include <cstdint>
#include <iostream>
#include <png.h>
#include <vector>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////

enum : uint32_t
{
	kTileSize = 8,
	kRowsPerTile = 8,
	kColorsPerPalette = 4,
	kPaletteMaxCount = 8,
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

struct Palette
{
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

static ColorBGR555 ConvertColor(ColorRGBA rgba)
{
	const uint8_t red = rgba.r / 8;
	const uint8_t green = rgba.g / 8;
	const uint8_t blue = rgba.b / 8;
	assert(red < 32 && green < 32 && blue < 32);
	return (blue << 10) | (green << 5) | red;
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
	const ColorRGBA* getPixels() const;

private:
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

const ColorRGBA* Image::getPixels() const
{
	return m_pixels;
}

////////////////////////////////////////////////////////////////////////////////

struct TileFlip
{
	union
	{
		uint16_t rows[kRowsPerTile];
		uint64_t values[kRowsPerTile * sizeof(uint16_t) / sizeof(uint64_t)];
	};
};

enum : uint32_t
{
	kTileFlip_None,
	kTileFlip_Horizontal,
	kTileFlip_Vertical,
	kTileFlip_Both,
	kTileFlip_Count,
};

struct Tile
{
	TileFlip flips[kTileFlip_Count];
};

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

	PaletteSet palette_set = {};

	// TODO Extract palettes (up to 8)
	// - Build an ordered list of four BGRA555 colors for each tile
	// - Merge the lists into 8 palettes
	// - Build a palette set

	// TODO Extract tiles
	// - Build an ordered list of four BGRA555 colors and find the best matching palette
	// - Compute the four flipped variations of the tile based on the palette
	// - Build a tile set (map that assigns the same tile index to each flip of a tile)

	// TODO Extract maps
	// For each map
	// - Extract each tile flip
	// - Find the flip into the tileset
	// - Build the index and parameter maps

	// TODO Output one palette file (.pal), one tileset (.chr) and several tilemaps (.tm1 and .tm2)
	// - Palette set into .pal
	// - First tile of tile description into .chr
	// - Indices into .tm1
	// - Parameters into .tm2

	return 0;
}

