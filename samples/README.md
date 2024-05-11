# Samples

This folder contains all the samples from the book.
Prebuilt ROMs are available [here](../prebuilt/roms).
This documentation explains how to build the samples.

## Requirements

To build the samples, you need the [RGBDS](https://rgbds.gbdev.io/) toolchain.
RGBDS executables (rgbasm, rgblink and rgbfix) are expected to be in your path for any build method to work.

The samples have been tested with [RGBDS v0.7.0](https://github.com/gbdev/rgbds/releases/tag/v0.7.0).
The code source will be updated if there are any compilation issues introduced with newer versions of the RGBDS toolchain.
Older versions of the toolchain might work, but they are not actively supported.

## Using scripts

There are build scripts in each sample folder.
For Windows, use the batch file named `build.bat`.
For other operating systems, use the shell script named `build.sh`.

There are also scripts to build all the samples at once.
They are located at the root of the folder: [build_samples.bat](build_samples.bat) for Windows, and [build_samples.sh](build_samples.sh) (a bash script) for the other operating systems.
The built ROMs are copied into the [prebuilt/roms](../prebuilt/roms) folder at the root of the repository.

## Using GBBS

It is possible to build the samples with [GBBS](https://github.com/mdagois/gbtools/tree/main/gbbs).
Just type `make -j` at the root of the repository to build all samples.
The ROMs will be output in the `build\release` folder at the root of the repository.
It is also possible to build each sample separately.
For example, to build the `palette_fade` sample, just type `make palette_fade`.

