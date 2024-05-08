# Game Boy Coding Adventure

This is the companion repository for the book [Game Boy Coding Adventure](https://mdagois.gumroad.com/l/CODQn) by [Maximilien Dagois](https://mdagois.gumroad.com/).

The repository contains all the samples and conversion tools presented in the book, as well as some fully commented games.
Prebuilt ROMs are provided for the samples and games, so it is possible to try and use them right away.
Instructions to build the ROMs and tools from source are available in the repository.

## License

Most of the content in the repository is licensed under the CC0 1.0 Universal license, available [here](LICENSE.txt).
This includes the [samples](samples), the [games](games), and the [tools](tools).

However, there are three exceptions.

First, the copy of GNU make, available [here](bin/make.exe), is licensed under the GPLv3+.

Second, the [stb_image.h](tools/conv/third_party/stb_image.h) file, from the [stb](https://github.com/nothings/stb) repository, belongs to the public domain.

Finally, the vast majority of the graphics [assets](assets) are from [Oceans Dream](https://oceansdream.itch.io)'s [Nostalgia Base Pack](https://oceansdream.itch.io/nostalgia-pack).
Some of the assets from the pack were modified to match the purpose of the samples.
You cannot use any of these assets in your own project without buying the pack first.
There are assets from other sources as well.
They all belong to the public domain and were authored by [Armando Montero](https://opengameart.org/users/armm1998), [GX310](https://gx310.itch.io), [Patrick](https://opengameart.org/users/patvanmackelberg), and [Sebastian Riedel](https://opengameart.org/users/ba%C5%9Dto).

## Samples

The source code of all samples is available in the [samples](samples) folder.
Prebuilt ROMs are available in the [prebuilt/roms](prebuilt/roms) folder.
The instructions to build the ROMs are in [samples/README.md](samples/README.md).

## Assets

The graphics assets used in the samples are available in the [assets](assets) folder.
Converted assets, such as tilesets and tilemaps, are available in the [prebuilt/assets](prebuilt/assets) folder.
Each sample has copies of the converted assets it uses in its folder.
The instructions to convert the assets are in [assets/README.md](assets/README.md).

## Tools

The source code for all the custom tools created for the purpose of this book is available in the [tools](tools) folder.
This includes all the data converters, an HTML color converter, and the printer simulator.
For Windows, prebuilt data conversion tools, as well as a copy of GNU make, are available in the [bin](bin) directory, for convenience.
For other operating systems, it is necessary to rebuild the tools.
The instructions to build and use the tools are in [tools/README.md](tools/README.md).

## Games

The source code and prebuilt ROMs for the games are available in the [games](games) folder.
More information is available in [games/README.md](games/README.md).

## Contributing

If you wish to contribute fixes, tools, or games to the repository, please do a pull request.
Contact the support at support@codingadventures.xyz for any question related to contributions.

## Support

Please report any repository issues [here](https://github.com/mdagois/gca/issues).
For other topics, please contact the support at support@codingadventures.xyz.

