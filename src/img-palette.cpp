/**
 * @file  img-palette.cpp
 * @brief Image implementation of a Palette file.  This will be inherited by
 *        classes implementing a specific palette file formats.
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

#include "img-palette.hpp"

namespace camoto {
namespace gamegraphics {

Palette::Palette()
{
}

Palette::~Palette()
{
}

int Palette::getCaps()
{
	return HasPalette;
}

void Palette::getDimensions(unsigned int *width, unsigned int *height)
{
	*width = 0;
	*height = 0;
}

StdImageDataPtr Palette::toStandard()
{
	return StdImageDataPtr();
}

StdImageDataPtr Palette::toStandardMask()
{
	return StdImageDataPtr();
}

void Palette::fromStandard(StdImageDataPtr newContent,
	StdImageDataPtr newMask)
{
	return;
}

} // namespace gamegraphics
} // namespace camoto
