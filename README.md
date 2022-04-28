# Game Boy Coding Adventure

Companion repository for the book [**Game Boy Coding Adventure**](https://mdagois.gumroad.com/l/CODQn) by [Maximilien Dagois](https://mdagois.gumroad.com/).

## Samples

All the code samples referenced in the book are available in the ***samples*** folder.

### Building the samples

#### RGBDS

To build the samples, you need the [RGBDS](https://rgbds.gbdev.io/) toolchain.
RGBDS executables (rgbasm, rgblink and rgbfix) are expected to be in your path for the provided build scripts to work.

The samples have been tested with RGBDS v0.5.1.
The samples will be updated if there are any compilation issues introduced with newer versions of the RGBDS toolchain.
Older versions of the toolchain are not supported.

#### Scripts

There are build scripts in each sample folder.
For Windows, use the batch file named ***build.bat***.
For other operating systems, use the shell script named ***build.sh***.

There are also scripts to build all the samples at once.
They are located at the root of the repository: ***build_samples.bat*** for Windows and ***build_samples.sh*** (a bash script) for the other operating systems.
The built ROMs are gathered into the ***generated/roms*** folder at the root of the repository.

## Assets

All the sample assets are available in the ***assets*** folder.
Graphics assets (tilesets and tilemaps) are PNG files.

### Converting assets

Assets are already converted and ready to use in the samples.
However, it is possible to convert them again using the ***gfxconv*** tool.

### The gfxconv tool

The ***gfxconv*** converts PNG files into tilesets and tilemaps compatible with the DMG VRAM.

#### Usage

The tool takes one tileset PNG and any number of tilemap PNG files as parameters.
Here are a few example of using ***gfxconv***.

```
# Converting a tileset
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
Tilemap PNGs are expected to use the same color as the tileset.
They are converted to TLM files.
TLM files contain tile indices referring tiles inside the tileset.
They can be copied as is in VRAM.

#### Binaries

Windows binaries for ***gfxconv*** are available in the ***bin*** folder.
For other operating systems, it is necessary to build the tool.

#### Building gfxconv

A cmake file is available in the ***tools/conv*** folder.
As the code fits into a single source file, it should be easy to use any other build system.
The only dependency is [libpng](http://www.libpng.org/pub/png/libpng.html).

#### Script

A Windows batch file, ***build_assets.bat***, is available to rebuild all the assets.
It puts the tilesets and tilemaps in the **generated/chr** and **generated/tlm** folders respectively.
The batch uses ***gfxconv*** from the **bin** folder, so there is no need to rebuild it.

For other operating systems, it is necessary to rebuild ***gfxconv*** and write an equivalent script.

## Support

Please report any repository issues [here](https://github.com/mdagois/gca/issues).
For other topics, please contact the support at support@codingadventures.xyz.

