/**
 * @file   gamegraphics.hpp
 * @brief  Main header for libgamegraphics (includes everything.)
 *
 * Copyright (C) 2010-2013 Adam Nielsen <malvineous@shikadi.net>
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

libgamegraphics provides a standard interface to access different game data
"graphicss" regardless of their file format.  Just like a .zip file, many games
squash all their data files together into one big file, and this library
provides access to those single big files.  Files can be listed and extracted,
as well as added, removed, renamed and rearranged.

\section structure Structure

The main interface to the library is the getManager() function, which returns
an instance of the Manager class.  The Manager is used to query supported
graphics formats, and for each supported file format it returns an instance of
the GraphicsType class.

The GraphicsType class can be used to examine files and check what file format
they are in, and if they are in the correct format, to open them.  Successfully
opening an graphics file produces an instance of the Graphics class.  The
GraphicsType class can also be used to create new graphicss from scratch, which
will again return an Graphics instance.

The Graphics class is used to directly manipulate the graphics file, such as by
adding and removing files.

\section example Examples

The libgamegraphics distribution comes with example code in the form of the
<a href="http://github.com/Malvineous/libgamegraphics/blob/master/examples/gamearch.cpp">gamearch
utility</a>, which provides a simple command-line interface to the
full functionality of the library.

For a small "hello world" example, try this:

@include hello.cpp

When run, this program produces output similar to the following:

@verbatim
Found 456 files.
LOGO.ANM
CINEOV2.ANM
CINEOV3.ANM
RADLOGO.ANM
DUKETEAM.ANM
VOL41A.ANM
VOL42A.ANM
VOL43A.ANM
VOL4E1.ANM
...
Done.
@endverbatim

\section info More information

Additional information including a mailing list is available from the Camoto
homepage <http://www.shikadi.net/camoto>.

**/
}
}

// These are all in the camoto::gamegraphics namespace
#include <camoto/gamegraphics/palettetable.hpp>
#include <camoto/gamegraphics/image.hpp>
#include <camoto/gamegraphics/imagetype.hpp>
#include <camoto/gamegraphics/tileset.hpp>
#include <camoto/gamegraphics/tilesettype.hpp>
#include <camoto/gamegraphics/manager.hpp>

#endif // _CAMOTO_GAMEGRAPHICS_HPP_
