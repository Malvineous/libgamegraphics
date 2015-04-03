/**
 * @file  camoto/gamegraphics.hpp
 * @brief Main header for libgamegraphics (includes everything.)
 *
 * Copyright (C) 2010-2015 Adam Nielsen <malvineous@shikadi.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _CAMOTO_GAMEGRAPHICS_HPP_
#define _CAMOTO_GAMEGRAPHICS_HPP_

/// Main namespace
namespace camoto {
/// Namespace for this library
namespace gamegraphics {

/**

\mainpage libgamegraphics

libgamegraphics provides a standard interface to access images in different file
formats, many of which are unique to a single game only.

Graphics are split into two categories - images and tilesets.  An image is a
single picture, while a tileset is a collection of many images.  While images
have width and height, a tileset often (but not always) requires all its images
to be the same size.

When reading images and tilesets, there are \a capability flags that need to be
checked to discover whether an image can be resized, whether all the images in
a tileset can be resized together, or whether a tileset can have a new image
inserted, and so on.

\section libgamegraphics_structure Structure

The main interface to the library is the getManager() function, which returns
an instance of the Manager class.  The Manager is used to query supported
graphics formats, and for each supported file format it returns an instance of
the ImageType or TilesetType class.

The Type class can be used to examine files and check what file format
they are in, and if they are in the correct format, to open them.  Successfully
opening an image file produces an instance of the Image class, and opening a
tileset file produces an instance of the Tileset class.  The Type classes can
also be used to create new files from scratch, which will again return an Image
or Tileset instance.

The Image and Tileset classes are used to directly manipulate the graphics
files, such as by importing/exporting image data or inserting and removing
tiles.

\section libgamegraphics_example Examples

The libgamegraphics distribution comes with example code in the form of the
<a href="http://github.com/Malvineous/libgamegraphics/blob/master/examples/gamearch.cpp">gamearch
utility</a>, which provides a simple command-line interface to the
full functionality of the library.

For a small "hello world" example, try this:

@include hello.cpp

When run, this program produces output similar to the following:

@verbatim
This image is 320x200 pixels in size.
@endverbatim

\section libgamegraphics_info More information

Additional information is available from the Camoto homepage
<http://www.shikadi.net/camoto>.

**/
}
}

// These are all in the camoto::gamegraphics namespace
#include <camoto/gamegraphics/palette.hpp>
#include <camoto/gamegraphics/image.hpp>
#include <camoto/gamegraphics/imagetype.hpp>
#include <camoto/gamegraphics/tileset.hpp>
#include <camoto/gamegraphics/tilesettype.hpp>
#include <camoto/gamegraphics/manager.hpp>

#endif // _CAMOTO_GAMEGRAPHICS_HPP_
