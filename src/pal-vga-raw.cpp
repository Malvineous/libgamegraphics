/**
 * @file   pal-vga-raw.cpp
 * @brief  Palette interface to 768-byte raw 6-bit VGA palette files.
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

#include "pal-vga-raw.hpp"

namespace camoto {
namespace gamegraphics {

VGAPalette::VGAPalette(iostream_sptr data)
	throw (std::ios::failure) :
		data(data)
{
	uint8_t buf[768];
	memset(buf, 0, 768);
	data->read((char *)buf, 768);
	// If the palette data is cut off (short read) the rest of the entries will
	// be black.
	int i = 0;
	while (i < 768) {
		PaletteEntry p;
		p.red = (buf[i] > 0x3F) ? 255 : buf[i] * 255 / 63;     i++;
		p.green = (buf[i] > 0x3F) ? 255 : buf[i] * 255 / 63;   i++;
		p.blue = (buf[i] > 0x3F) ? 255 : buf[i] * 255 / 63;    i++;
		this->entries.push_back(p);
	}
}

VGAPalette::~VGAPalette()
	throw ()
{
}

int VGAPalette::getMaxEntries()
	throw ()
{
	return 256;
}

void VGAPalette::setEntries(const PaletteEntries& source)
	throw (std::ios::failure)
{
	uint8_t buf[768];
	memset(buf, 0, 768);
	int i = 0;
	for (PaletteEntries::const_iterator p = source.begin(); p < source.end(); p++) {
		buf[i++] = p->red * 63 / 255;
		buf[i++] = p->green * 63 / 255;
		buf[i++] = p->blue * 63 / 255;
	}
	if (this->data->rdbuf()->sputn((char *)buf, 768) != 768) {
		throw std::ios::failure("Unable to write palette to stream (disk full?)");
	}
	this->entries = source;
	return;
}

} // namespace gamegraphics
} // namespace camoto
