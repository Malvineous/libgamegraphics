/**
 * @file  baseimage.cpp
 * @brief Base class for all Image subclasses.
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

#include <cassert>
#include "baseimage.hpp"

namespace camoto {
namespace gamegraphics {

Image_Base::Image_Base()
{
}

Image_Base::~Image_Base()
{
}

void Image_Base::setDimensions(unsigned int width, unsigned int height)
{
	// Caller didn't check getCaps()
	assert(false);
	throw stream::error("this image format can't have its dimensions changed"
		" (this is a bug - the caller should have used getCaps() to detect this)");
}

PaletteTablePtr Image_Base::getPalette()
{
	return PaletteTablePtr();
}

void Image_Base::getHotspot(signed int *x, signed int *y)
{
	// Caller didn't check getCaps()
	assert(false);
	throw stream::error("this image format can't have a hotspot set"
		" (this is a bug - the caller should have used getCaps() to detect this)");
}

void Image_Base::setHotspot(signed int x, signed int y)
{
	// Caller didn't check getCaps()
	assert(false);
	throw stream::error("this image format can't have a hotspot set"
		" (this is a bug - the caller should have used getCaps() to detect this)");
}

void Image_Base::getHitRect(signed int *x, signed int *y)
{
	// Caller didn't check getCaps()
	assert(false);
	throw stream::error("this image format can't have a hitmap rectangle"
		" (this is a bug - the caller should have used getCaps() to detect this)");
}

void Image_Base::setHitRect(signed int x, signed int y)
{
	// Caller didn't check getCaps()
	assert(false);
	throw stream::error("this image format can't have a hitmap rectangle"
		" (this is a bug - the caller should have used getCaps() to detect this)");
}

void Image_Base::setPalette(PaletteTablePtr newPalette)
{
	// Caller didn't check getCaps()
	assert(false);
	throw stream::error("this format doesn't support palettes"
		" (this is a bug - the caller should have used getCaps() to detect this)");
}

} // namespace gamegraphics
} // namespace camoto
