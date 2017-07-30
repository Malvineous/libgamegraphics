Camoto: Classic-game Modding Tools
==================================
Copyright 2010-2016 Adam Nielsen <<malvineous@shikadi.net>>  
http://www.shikadi.net/camoto/  
Linux/OSX: [![Build Status](https://travis-ci.org/Malvineous/libgamegraphics.svg?branch=master)](https://travis-ci.org/Malvineous/libgamegraphics)

Camoto is a collection of utilities for editing (modding) "classic" PC
games - those running under MS-DOS from the 1980s and 1990s.

This is **libgamegraphics**, one component of the Camoto suite.  libgamegraphics
provides a standard method of accessing pixel and palette data in the many
different (often custom) image formats used by many games.  It allows
full-screen images, sprites and animations to be extracted and/or modified.

Pixel data is provided as an array of bytes, with one byte per pixel.  This
allows it to work with images of up to 256 colours.  Images of less colours
(such as EGA 16-colour) will still be expanded to one byte per pixel, only the
values used will be limited (e.g. 0-15.)  This means any application using
the library will only need to handle a single image format - 8bpp with a
32-bit RGBA palette.

The palette will be made available if it exists, however as many image formats
store the palette externally it is left up to the application to locate and
supply any external palette data.

File formats from the following games have been implemented:

  * Catacomb/Catacomb II (tiles)
  * Captain Comic (tiles, sprites and fullscreen images)
  * Captain Comic II (tiles, fullscreen, no sprites yet)
  * Cosmo's Cosmic Adventures (tiles, fullscreen, sprites [read-only])
  * Crystal Caves (tiles, sprites and fullscreen images)
  * Dangerous Dave (tiles)
  * Duke Nukem II (tiles and fullscreen images)
  * God of Thunder (tiles)
  * Halloween Harry (tiles and fullscreen images)
  * Jill of the Jungle (tiles, read-only)
  * Major Stryker
  * Monster Bash (tiles)
  * Raptor (some fullscreen)
  * Secret Agent (tiles, sprites and fullscreen images)
  * Shadow Warrior 1993 Beta (tiles, images)
  * Vinyl Goddess From Mars (tiles, some fullscreen)
  * Wacky Wheels (tiles and fullscreen images)
  * Word Rescue (tiles and fullscreen images)
  * Xargon (tiles, read-only)
  * Zone 66 (tiles)

Many more formats are planned.

This distribution includes two example programs (`gameimg` and `gametls`) which
provide access to single-image files and tilesets respectively.  These are
intended as both command-line interfaces to the library as well as an example
of how to use the library.  The programs are installed as part of the
`make install` process.  See their manpages for full details.

The library is compiled and installed in the usual way:

    ./autogen.sh          # Only if compiling from git
    ./configure && make
    make check            # Optional, compile and run tests
    sudo make install
    sudo ldconfig

You will need the following prerequisites already installed:

  * [libgamecommon](https://github.com/Malvineous/libgamecommon) >= 2.0
  * [libgamearchive](https://github.com/Malvineous/libgamearchive) >= 2.0
  * Boost >= 1.59 (Boost >= 1.46 will work if not using `make check`)
  * libpng
  * [png++](http://www.nongnu.org/pngpp/) >= 0.2.7
  * xmlto (optional for tarball releases, required for git version and if
    manpages are to be changed)

All supported file formats are fully documented on the ModdingWiki
([images](http://www.shikadi.net/moddingwiki/Category:Image_formats) and
[tilesets](http://www.shikadi.net/moddingwiki/Category:Tileset_formats)).

This library is released under the GPLv3 license.
