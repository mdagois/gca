# Game Boy Printer Simulator

The Game Boy Printer Simulator is a Game Boy Color ROM that acts as a Game Boy Printer.
It can be linked to a game with printer support and stand in for a printer.
The main goal of the simulator is to help developers test the printer support in their ROMs.

## Highlights

* Works on emulators (tested on BGB)
* Works on real hardware (tested on Game Boy Color)
* The debug version has BGB-style logs to help debugging
* Supports data compression
* Tested with _Game Boy Camera_, _1942_ and _Pokémon Card GB_ (should work with other games too)
* Simulates errors

## Build instructions

Use `build rom` to build a release version.
Use `build romd` to build a debug version.
The debug version will emit logs in BGB.
However, it is slower and more sensible to miss print data bytes if a ROM emits them too fast.
Check the makefile for additional information.

## Typical usage

1. Launch the simulator ROM in BGB in _listen_ mode (`--listen <port>`)
2. Launch a ROM with printer support in BGB and connect it (`--connect <address:port>`) to the BGB instance running the simulator ROM
3. Launch a print job from the ROM
4. Check the printed data in the simulator
5. Alternatively, hold d-pad keys during data transfer to generate errors
	* UP for low battery (error #01)
	* RIGHT for a packet error (error #02)
	* LEFT for paper jam (error #03)
	* DOWN for any other error (error #04)

## Unsupported features

The following parameters are not supported, because they are rarely the cause of bugs.
They are are simply ignored by the simulator.

* sheet count
* margin
* burning levels

## Support

Please report any issues [here](https://github.com/mdagois/gca/issues).
Alternatively, please contact the support at support@codingadventures.xyz.

