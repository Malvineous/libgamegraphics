/**
 * @file   img-palette.cpp
 * @brief  Image implementation of a Palette file.  This will be inherited by
 *         classes implementing a specific palette file formats.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Crystal_Caves_Tileset_Format
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

#include "img-palette.hpp"

namespace camoto {
namespace gamegraphics {

Palette::Palette()
	throw ()
{
}

Palette::~Palette()
	throw ()
{
}

int Palette::getCaps()
	throw ()
{
	return HasPalette;
}

void Palette::getDimensions(unsigned int *width, unsigned int *height)
	throw (std::ios::failure)
{
	*width = 0;
	*height = 0;
}

StdImageDataPtr Palette::toStandard()
	throw (std::ios::failure)
{
	return StdImageDataPtr();
}

StdImageDataPtr Palette::toStandardMask()
	throw (std::ios::failure)
{
	return StdImageDataPtr();
}

void Palette::fromStandard(StdImageDataPtr newContent,
	StdImageDataPtr newMask)
	throw (std::ios::failure)
{
	return;
}

} // namespace gamegraphics
} // namespace camoto
