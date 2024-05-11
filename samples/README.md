# Samples

This folder contains all the samples from the book.
This documentation explains how to build the samples.
Each sample has copies of the converted assets it uses, so there is no need to convert those assets before building any sample.

## Requirements

The [RGBDS](https://rgbds.gbdev.io/) toolchain in required to build the samples.
RGBDS executables (`rgbasm`, `rgblink` and `rgbfix`) are expected to be in the path for any build method to work.

The samples have been tested with [RGBDS v0.7.0](https://github.com/gbdev/rgbds/releases/tag/v0.7.0).
Best effort will be done to support newer versions of the toolchain as they get released.
Older versions of the toolchain might work, but they are not actively supported.

## Using scripts

There are build scripts in each sample folder.
For Windows, use the batch file named `build.bat`.
For other operating systems, use the shell script named `build.sh`.
These scripts output the ROM inside their folder.

There are also scripts to build all the samples at once.
They are located at the root of the folder: [build_samples.bat](build_samples.bat) for Windows, and [build_samples.sh](build_samples.sh) (a bash script) for the other operating systems.
These scripts output ROMs into the [prebuilt/roms](../prebuilt/roms) folder at the root of the repository.

## Using GBBS

It is possible to build the samples with [GBBS](https://github.com/mdagois/gbtools/tree/main/gbbs).
Just type `make -j` from the [samples](.) folder to build all the samples.
The ROMs are output in the `build\release` folder at the root of the repository.
It is also possible to build each sample separately.
For example, to build the `palette_fade` sample, just type `make palette_fade`.

