/**
 * @file   gamegraphics.i
 * @brief  SWIG configuration file for libgamegraphics.
 *
 * Copyright (C) 2010-2011 Adam Nielsen <malvineous@shikadi.net>
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
%module gamegraphics

%include <std_string.i>
%include <std_vector.i>
%include <exception.i>
%include <boost_shared_ptr.i>

%rename(None2) camoto::gamegraphics::Tileset::None;


%shared_ptr(camoto::gamegraphics::PaletteTable)
%shared_ptr(camoto::gamegraphics::Image)
%shared_ptr(camoto::gamegraphics::ImageType)
%shared_ptr(camoto::gamegraphics::Tileset)
%shared_ptr(camoto::gamegraphics::TilesetType)
%shared_ptr(camoto::gamegraphics::Manager)

%apply          char      {   int8_t }
%apply unsigned char      {  uint8_t }
%apply          short     {  int16_t }
%apply unsigned short     { uint16_t }
%apply          int       {  int32_t }
%apply unsigned int       { uint32_t }
%apply          long long {  int64_t }
%apply unsigned long long { uint64_t }

%exception {
	try {
		$action
	} catch (const std::ios::failure& e) {
		SWIG_exception(SWIG_IOError, e.what());
	} catch (const std::exception& e) {
		SWIG_exception(SWIG_RuntimeError, e.what());
	}
}

// Tileset::Entry doesn't need to be accessible from within Python (yet)
//%warnfilter(SWIGWARN_PARSE_NAMED_NESTED_CLASS) Entry;

%{
	#include <fstream>
	#include <boost/pointer_cast.hpp>
	#include <camoto/gamegraphics.hpp>

	using namespace camoto;
	using namespace camoto::gamegraphics;

	istream_sptr openFile(const char *filename)
	{
		std::ifstream *f = new std::ifstream(filename);
		return istream_sptr(f);
	}
%}

class std::istream;
%include <camoto/types.hpp>
%include <camoto/metadata.hpp>
%include <camoto/gamegraphics/palettetable.hpp>
%include <camoto/gamegraphics/image.hpp>
%include <camoto/gamegraphics/imagetype.hpp>
%include <camoto/gamegraphics/tileset.hpp>
%include <camoto/gamegraphics/tilesettype.hpp>
%include <camoto/gamegraphics/manager.hpp>
%include <camoto/gamegraphics.hpp>

camoto::istream_sptr openFile(const char *filename);
