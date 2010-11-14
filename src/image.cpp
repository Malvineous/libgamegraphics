/**
 * @file   image.cpp
 * @brief  Standard Image functions.
 *
 * Copyright (C) 2010 Adam Nielsen <malvineous@shikadi.net>
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
	throw ()
{
}

Image::~Image()
	throw ()
{
}

void Image::setDimensions(unsigned int width, unsigned int height)
	throw (std::ios::failure)
{
	// Caller didn't check getCaps()
	assert(false);
	throw std::ios::failure("this image format can't have its dimensions changed"
		" (this is a bug - the caller should have used getCaps() to detect this)");
}

PaletteTablePtr Image::getPalette()
	throw (std::ios::failure)
{
	return PaletteTablePtr();
}

void Image::setPalette(PaletteTablePtr newPalette)
	throw (std::ios::failure)
{
	// Caller didn't check getCaps()
	assert(false);
	throw std::ios::failure("this format doesn't support palettes"
		" (this is a bug - the caller should have used getCaps() to detect this)");
}

} // namespace gamegraphics
} // namespace camoto
