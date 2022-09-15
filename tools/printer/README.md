# Game Boy Printer Simulator

The Game Boy Printer Simulator is a Game Boy Color ROM that acts as a Game Boy Printer.
It can be linked to a game with printer support and stand in for the printer.
The simulator is not meant to be a very accurate emulation of the real printer hardware.
Instead, its main goal is to help developers quickly test and iterate over the printer code of their ROMs.

## Highlights

* Works on emulators (tested with BGB)
* Works on real hardware (tested on a Game Boy Color)
* Has BGB-style logs to help debugging (debug version)
* Supports data compression
* Tested with _Game Boy Camera_, _1942_ and _Pokémon Card GB_ (should work with other games too)
* Simulates printer errors

## Build instructions

Use `make rom` to build a release version.
Use `make romd` to build a debug version.
Check the makefile for additional information.

The debug version emits logs in BGB for easier debugging.
However, it is slower and more sensible to miss print data bytes if a ROM transfers them too fast.

## Typical usage

1. Launch the simulator ROM in BGB in _listen_ mode (`--listen <port>`)
2. Launch a ROM with printer support in BGB and connect it (`--connect <address:port>`) to the BGB instance running the simulator ROM
3. Launch a print job from the ROM
4. Check the printed data appear on the LCD of the simulator
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

