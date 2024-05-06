# Game Boy Coding Adventure

Companion repository for the book [**Game Boy Coding Adventure**](https://mdagois.gumroad.com/l/CODQn) by [Maximilien Dagois](https://mdagois.gumroad.com/).

## License

Most of the content in the repository is licensed under the CC0 1.0 Universal license, available [here](LICENSE.txt).
This includes the [samples](samples), the [games](games), and the [tools](tools).

However, there are two exceptions.
First, the copy of GNU make, available [here](bin/make.exe), is licensed under the GPLv3+.
Second, the vast majority of the [assets](assets) are from [Oceans Dream](https://oceansdream.itch.io)'s [Nostalgia Base Pack](https://oceansdream.itch.io/nostalgia-pack).
You cannot use the assets in your own project without buying the pack first.
Some of the assets from the pack were modified to match the purpose of the samples.
There are assets from other sources as well.
They all belong to the public domain and were authored by [Armando Montero](https://opengameart.org/users/armm1998), [GX310](https://gx310.itch.io), [Patrick](https://opengameart.org/users/patvanmackelberg), and [Sebastian Riedel](https://opengameart.org/users/ba%C5%9Dto).


TODO Link to prebuilt samples + link to how to build them
TODO Link to prebuilt assets + link to how to build them
TODO Conversion tool prebuilt + how to build them + new built tool
TODO Printer emulator
TODO Games

## Samples

All the code samples referenced in the book are available in the [samples](samples) folder.

### Building the samples

#### RGBDS

To build the samples, you need the [RGBDS](https://rgbds.gbdev.io/) toolchain.
RGBDS executables (rgbasm, rgblink and rgbfix) are expected to be in your path for the provided build scripts to work.

The samples have been tested with [RGBDS v0.7.0](https://github.com/gbdev/rgbds/releases/tag/v0.7.0).
The samples will be updated if there are any compilation issues introduced with newer versions of the RGBDS toolchain.
Older versions of the toolchain might work, but they are not actively supported.

#### Scripts

There are build scripts in each sample folder.
For Windows, use the batch file named `build.bat`.
For other operating systems, use the shell script named `build.sh`.

There are also scripts to build all the samples at once.
They are located at the root of the repository: [build_samples.bat](build_samples.bat) for Windows and [build_samples.sh](build_samples.sh) (a bash script) for the other operating systems.
The built ROMs are gathered into the `generated/roms` folder at the root of the repository.

Finally, it is possible to build the samples with [GBBS](https://github.com/mdagois/gbtools/tree/main/gbbs).
Just type `make -j` at the root of the repository to build all samples.

## Assets

All the sample assets are available in the [assets](assets) folder.
The graphics assets (tilesets and tilemaps) are PNG files.

### Converting assets

Assets are already converted and ready to use in the samples.
However, it is possible to convert them again using the `gfxconv`, `cgbconv`, `sgbconv` and `sfcconv` tools.

### The gfxconv tool

The `gfxconv` tool converts PNG files into tilesets and tilemaps compatible with the DMG VRAM.

#### Usage

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

### The cgbconv tool

The `cgbconv` tool converts PNG files into tilesets, palettes, tilemap indices and tilemap parameters compatible with the CGB VRAM.

#### Usage

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

### The sgbconv tool

The `sgbconv` tool converts PNG files into tilesets, palettes, tilemaps and screen tile attributes compatible with the SGB.

#### Usage

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

### The sfcconv tool

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

### Tool binaries

Windows binaries for all converters are available in the [bin](bin) folder.
For other operating systems, it is necessary to build the tools.

#### Building the tools

A [cmake](https://cmake.org/) file is available in the [tools/conv](tools/conv) folder.
As each tool's code fits into a single source file, it should be straightforward to use any other build system.
The only dependency is the `stb_image.h` header, which is included in the `third_party` folder.

### Conversion script

A Windows batch file, [build_assets.bat](build_assets.bat), is available to rebuild all the assets.
It puts the converted files (tilesets, palettes, tilemaps, etc.) into the [generated](generated) folder.
The batch uses the converter binaries from the [bin](bin) folder, so there is no need to rebuild them.

For other operating systems, it is necessary to rebuild the conversion tools and write an equivalent script.

## Printer Simulator

The Game Boy Printer Simulator referenced in the book is available in the [tools/printer](tools/printer) directory.
Check the [README](tools/printer/README.md) file there to know more about the simulator.

## Games

Simple games are available in the [games](games) directory.
Check the [README](games/README.md) file there for more details.

## Makefile for custom project

A makefile is available in the [tools/build](tools/build) directory.
It is meant to make it easy to build ROMs.
It is used to build the [Printer Simulator](tools/printer).
Please follow the instructions in the makefile file itself to know how to leverage it, and use the Printer Simulator [makefile](tools/printer/makefile) as a reference.

A more complete build system, [GBBS](https://github.com/mdagois/gbtools/tree/main/gbbs), is available in the [GB tools](https://github.com/mdagois/gbtools) repository.
It is used to build the samples and [games](games) in the repository.
It handles dependencies, multiple projects and much more.
Check it out to manage complex projects.

## Support

Please report any repository issues [here](https://github.com/mdagois/gca/issues).
For other topics, please contact the support at support@codingadventures.xyz.

