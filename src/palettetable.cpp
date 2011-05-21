/**
 * @file   palettetable.cpp
 * @brief  PaletteEntry, for accessing indexed colour maps used by game images.
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

#include <camoto/gamegraphics/palettetable.hpp>

namespace camoto {
namespace gamegraphics {

PaletteEntry::PaletteEntry()
{
}

PaletteEntry::PaletteEntry(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) :
	red(red), green(green), blue(blue), alpha(alpha)
{
}

PaletteTablePtr createDefaultCGAPalette()
	throw ()
{
	PaletteTablePtr pal(new PaletteTable());
	pal->reserve(16);

	for (int i = 0; i < 16; i++) {
		PaletteEntry p;
		p.red   = (i & 4) ? ((i & 8) ? 0xFF : 0xAA) : ((i & 8) ? 0x55 : 0x00);
		p.green = (i & 2) ? ((i & 8) ? 0xFF : 0xAA) : ((i & 8) ? 0x55 : 0x00);
		p.blue  = (i & 1) ? ((i & 8) ? 0xFF : 0xAA) : ((i & 8) ? 0x55 : 0x00);
		p.alpha = 255;
		if (i == 6) {
			p.green = 0x55;
		}
		pal->push_back(p);
	}

	return pal;
}

PaletteTablePtr createDefaultEGAPalette()
	throw ()
{
	PaletteTablePtr pal(new PaletteTable());
	pal->reserve(64);

	uint8_t low = 0x55;

	for (int i = 0; i < 64; i++) {
		PaletteEntry p;
		p.red   = ((i & 4) ? ~low : 0) | ((i & 32) ? low : 0);
		p.green = ((i & 2) ? ~low : 0) | ((i & 16) ? low : 0);
		p.blue  = ((i & 1) ? ~low : 0) | ((i & 8) ? low : 0);
		p.alpha = 255;
		pal->push_back(p);
	}

	return pal;
}

} // namespace gamegraphics
} // namespace camoto
