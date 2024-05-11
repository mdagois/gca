# Tools

This folder contains all the custom tools created for the purpose of the book.
The tools consist of data converters, an HTML color picker, and the printer simulator.
This documentation explains how to build and use them.

## Conversion tools

There are five conversion tools: `gfxconv`, `cgbconv`, `sgbconv`, `sfcconv`, and `packetize`.
Windows binaries for all converters are available in the [bin](../bin) folder at the root of the repository.
For other operating systems, it is necessary to build the tools.

### Building the tools

A [cmake](https://cmake.org/) file is available in the [conv](conv) folder.
As each tool's code fits into a single source file, it should be straightforward to use any other build system.
The only dependency is the `stb_image.h` header, which is provided in the `third_party` folder.

### The `gfxconv` tool

The `gfxconv` tool converts PNG files into tilesets and tilemaps compatible with the DMG VRAM.

The tool takes one tileset PNG and any number of tilemap PNG files as parameters.
Here are a few examples of using `gfxconv`.

```
# Converting a tileset.
# The output file is tileset.chr.
gfxconv tileset.png

# Converting a tileset and a tilemap.
# The output files are tileset.chr and tilemap.tlm.
gfxconv tileset.png tilemap.png

# Converting a tileset and two tilemaps.
# The output files are tileset.chr, tilemap_0.tlm and tilemap_1.tlm.
gfxconv tileset.png tilemap_0.png tilemap_1.png
```

The tileset PNG must be the first parameter.
It is the only mandatory parameter.
A tileset PNG is expected to contain 384 tiles.
It is converted to a CHR file.
A CHR file contains tiles in the right format for the VRAM.

Any extra parameter after the tileset PNG is considered as a tilemap PNG.
Tilemap PNGs are expected to use the same colors as the tileset.
They are converted to TLM files.
TLM files contain tile indices referring to tiles inside the tileset.
They can be copied as is in VRAM.

### The `cgbconv` tool

The `cgbconv` tool converts PNG files into tilesets, palettes, tilemap indices and tilemap parameters compatible with the CGB VRAM.

The tool takes one tileset PNG and any number of tilemap PNG files as parameters.
Here are a few examples of using `cgbconv`.

```
# Converting a tileset.
# The output file is tileset.chr.
cgbconv tileset.png

# Converting a tileset and a tilemap.
# The output files are tileset.chr, tileset.pal, tilemap.idx and tilemap.prm.
cgbconv tileset.png tilemap.png

# Converting a tileset and two tilemaps.
# The output files are tileset.chr, tileset.pal, tilemap_0.idx, tilemap_0.prm,
# tilemap_1.idx and tilemap_1.prm.
cgbconv tileset.png tilemap_0.png tilemap_1.png
```

The tileset PNG must be the first parameter.
It is the only mandatory parameter.
It can contain up to 512 tiles (256 for each VRAM bank).
It is converted into two files: a CHR file containing tiles and a PAL file containing palettes.

Any extra parameter after the tileset PNG is considered as a tilemap PNG.
Tilemap PNGs are expected to use the tiles and colors from the tileset.
They should be either 256x144 or 256x256 pixels.
They are converted to IDX and PRM files.
IDX files contain tile indices referring to tiles inside the tileset.
They are meant to be copied into bank 0 of the VRAM.
PRM files contain tile parameters to control the tiles rendering (palette number, flip, etc.).
They are meant to be copied into bank 1 of the VRAM.

### The `sgbconv` tool

The `sgbconv` tool converts PNG files into tilesets, palettes, tilemaps and screen tile attributes compatible with the SGB.

The tool takes one tileset PNG and any number of tilemap PNG files as parameters.
Here are a few examples of using `sgbconv`.

```
# Converting a tileset and a tilemap.
# The output files are tileset.chr, tileset.pal, tilemap.tlm and tilemap.atr.
sgbconv tileset.png tilemap.png

# Converting a tileset and two tilemaps.
# The output files are tileset.chr, tileset.pal, tilemap_0.tlm, tilemap_0.atr,
# tilemap_1.tlm and tilemap_1.atr.
sgbconv tileset.png tilemap_0.png tilemap_1.png
```

The tileset PNG must be the first parameter.
It is the only mandatory parameter.
It can contain up to 256 tiles.
It is converted into two files: a CHR file containing tiles and a PAL file containing four 4-color palettes.
Palettes are meant to be part of SGB color commands.

Any extra parameter after the tileset PNG is considered as a tilemap PNG.
Tilemap PNGs are expected to use the tiles and colors from the tileset.
They should be 256x144.
They are converted to TLM and ATR files.
TLM files contain tile indices referring to tiles inside the tileset.
They are meant to be copied into VRAM.
ATR files contain screen tile attributes.
They are meant to be part of a `ATTR_CHR` or `ATTR_TRN` command.

### The `sfcconv` tool

The `sfcconv` tool converts PNG files into tilesets, palettes and tilemaps compatible with the SGB border graphics (SNES format).

The tool takes one tileset PNG and one tilemap PNG file as parameters.
Here is an example of using `sfcconv`.

```
# Converting a tileset and a tilemap.
# The output files are tileset.chr, tileset.pal and tilemap.tlm.
sfcconv tileset.png tilemap.png
```

The tileset PNG must be the first parameter.
It can contain up to 256 tiles.
It is converted into two files: a CHR file containing tiles and a PAL file containing three 16-color palettes.
Tiles are meant to be part of a `CHR_TRN` command, while palettes are meant to be part of a `PCT_TRN` command.

The tilemap PNG is expected to use the tiles and colors from the tileset.
It should be 256x224.
It is converted to a TLM file.
A TLM file contains tile indices referring to tiles inside the tileset.
It is meant to be part of a `PCT_TRN` command.

### The `packetize` tool

The `packetize` tool converts tiles into print packet data.

The tool takes one tileset CHR file as parameter.

```
# Converting a tileset.
# The output file is tileset.pkt.
packetize tileset.chr
```

The data from the CHR file is converted into print packet data for the Game Boy Printer.

## Color picker

The [color picker](tools/color_picker.html) is a simple HTML tool to convert RGB888 color to BGR555.
Open it in any browser to use it.

## Printer simulator

The Game Boy Printer Simulator is available in the [printer](printer) folder.
Check the [README](printer/README.md) file to know more about the simulator.

## GBBS

The [gbbs.mk](gbbs.mk) file is a complete build system from the [GB tools](https://github.com/mdagois/gbtools) repository.
It is used to build the samples, the printer simulator, and the games in the repository.
It handles dependencies, multiple projects and much more.
Check it out to manage complex projects.

