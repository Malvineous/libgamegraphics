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

Image_Palette::Image_Palette()
{
}

Image_Palette::~Image_Palette()
{
}

Image::Caps Image_Palette::caps() const
{
	return Caps::HasPalette;
}

Point Image_Palette::dimensions() const
{
	return {0, 0};
}

Pixels Image_Palette::convert() const
{
	return {};
}

Pixels Image_Palette::convert_mask() const
{
	return {};
}

void Image_Palette::convert(const Pixels& newContent,
	const Pixels& newMask)
{
	return;
}

} // namespace gamegraphics
} // namespace camoto
