#include <cstdint>

enum : uint32_t
{
	kColorsPerPalette = 4,
	kPaletteMaxCount = 8,
	kRowsPerTile = 8,
};

struct Palette
{
	union
	{
		uint16_t colors[kColorsPerPalette];
		uint64_t value;
	};
};

struct PaletteSet
{
	Palette palettes[kPaletteMaxCount];
};

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
	// TODO Check CLI arguments (tileset.png + several tilemap.png)

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

