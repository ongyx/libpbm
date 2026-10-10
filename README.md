# libpbm

<img src="assets/showcase.png" width="256">

A small library to work with the PBM/PGM/PPM format, written by myself to learn
C.

All three formats in both ASCII and binary format are currently supported.
For more information on the format itself, please see the [Wikipedia article] and the [original specification].

## Usage

Run `make` to build the shared library, then link using `-lm -lpbm` with your compiler of choice.
This library's only dependency is libm.

All documentation for public functions can be found in [`pbm.h`](./include/pbm.h).

## License

MIT.


## Credits

[Wikipedia article]: https://en.wikipedia.org/wiki/Netpbm#Description
[original specification]: https://netpbm.sourceforge.net/doc/pbm.html
