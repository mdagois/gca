# Game Boy Coding Adventure

Companion repository for the book [**Game Boy Coding Adventure**](https://mdagois.gumroad.com/l/CODQn) by [Maximilien Dagois](https://mdagois.gumroad.com/).

All the code samples referenced in the book are available in the **samples** folder.

## Building the samples

### RGBDS

The samples have been tested with [RGBDS](https://rgbds.gbdev.io/) v0.5.1.
The samples will be updated if there are any compilation issues introduced with newer versions of the RGBDS toolchain,
Older versions of the toolchain are not supported.

RGBDS executables (rgbasm, rgblink and rgbfix) are expected to be in your path.

### Scripts

There are build scripts in each sample's folder.
For Windows, use the batch file named **build.bat**.
For other operating systems, use the shell script named **build.sh**.

There are also build scripts to build all samples at once at the root of the repository.
Use **build_samples.bat** for Windows, and **build_samples.sh** for the other operating systems.

## Issue reporting

Please report any issues [here](https://github.com/mdagois/gca/issues).

