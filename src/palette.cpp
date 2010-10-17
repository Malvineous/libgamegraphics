/**
 * @file   palette.cpp
 * @brief  Declaration of Palette class, for accessing indexed colour maps used
 *         by game images.
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

#include <camoto/gamegraphics/palette.hpp>

namespace camoto {
namespace gamegraphics {

PaletteEntry::PaletteEntry()
{
}

PaletteEntry::PaletteEntry(uint8_t red, uint8_t green, uint8_t blue) :
	red(red), green(green), blue(blue)
{
}

Palette::Palette()
	throw ()
{
}

Palette::~Palette()
	throw ()
{
}

int Palette::getMaxEntries()
	throw ()
{
	return 0;
}

const PaletteEntries& Palette::getEntries()
	throw ()
{
	return this->entries;
}

} // namespace gamegraphics
} // namespace camoto
