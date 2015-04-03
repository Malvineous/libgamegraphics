/**
 * @file  image.cpp
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
#include <camoto/gamegraphics/image.hpp>

namespace camoto {
namespace gamegraphics {

Image::Image()
{
}

Image::~Image()
{
}

void Image::dimensions(const Point& newDimensions)
{
	// Fail if this function is called when the caps say not to
	assert(this->caps() & Caps::SetDimensions);

	// If we get here the file format said the dimensions can be changed but
	// forgot to override this function, so fail.
	assert(false);
	throw stream::error("This image format can't have its dimensions changed"
		" (this is a bug - the caller should have used caps() to detect this).");
}

Point Image::hotspot() const
{
	// Caller didn't check caps()
	assert(false);
	throw stream::error("This image format can't have a hotspot set"
		" (this is a bug - the caller should have used caps() to detect this).");
}

void Image::hotspot(const Point& newHotspot)
{
	// Caller didn't check caps()
	assert(false);
	throw stream::error("This image format can't have a hotspot set"
		" (this is a bug - the caller should have used caps() to detect this).");
}

Point Image::hitrect() const
{
	// Caller didn't check caps()
	assert(false);
	throw stream::error("This image format can't have a hitmap rectangle"
		" (this is a bug - the caller should have used caps() to detect this).");
}

void Image::hitrect(const Point& newHitrect)
{
	// Caller didn't check caps()
	assert(false);
	throw stream::error("This image format can't have a hitmap rectangle"
		" (this is a bug - the caller should have used caps() to detect this).");
}

std::shared_ptr<const Palette> Image::palette() const
{
	return this->pal;
}

void Image::palette(std::shared_ptr<const Palette> newPalette)
{
	assert(this->caps() & Caps::SetPalette);
	this->pal = newPalette;
	return;
}

} // namespace gamegraphics
} // namespace camoto
