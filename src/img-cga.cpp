/**
 * @file   img-cga.cpp
 * @brief  Image implementation adding support for CGA graphics.
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

#include "img-cga.hpp"

namespace camoto {
namespace gamegraphics {

CGAImage::CGAImage(stream::inout_sptr data,
	stream::pos off, CGAPaletteType cgaPal)
	throw () :
		parent(data),
		data(new bitstream(data, bitstream::bigEndian)),
		off(off),
		cgaPal(cgaPal)
{
}

CGAImage::~CGAImage()
	throw ()
{
}

int CGAImage::getCaps()
	throw ()
{
	return ColourDepthCGA | HasPalette;
}

StdImageDataPtr CGAImage::toStandard()
	throw ()
{
	unsigned int width, height;
	this->getDimensions(&width, &height);
	assert((width != 0) && (height != 0));
	int dataSize = width * height;

	uint8_t *imgData = new uint8_t[dataSize];
	StdImageDataPtr ret(imgData);
	this->data->seek(this->off << 3, stream::start);

	// Read the data as 2bpp and write it to the buffer as 8bpp
	while (dataSize--) {
		int val;
		this->data->read(2, &val);
		*imgData++ = (uint8_t)val;
	}

	return ret;
}

StdImageDataPtr CGAImage::toStandardMask()
	throw ()
{
	unsigned int width, height;
	this->getDimensions(&width, &height);
	assert((width != 0) && (height != 0));
	int dataSize = width * height;

	// Return an entirely opaque mask
	uint8_t *imgData = new uint8_t[dataSize];
	StdImageDataPtr ret(imgData);
	memset(imgData, 0, dataSize);

	return ret;
}

void CGAImage::fromStandard(StdImageDataPtr newContent,
	StdImageDataPtr newMask
)
	throw ()
{
	unsigned int width, height;
	this->getDimensions(&width, &height);
	assert((width != 0) && (height != 0));
	int dataSize = width * height;

	// Cut off any leftover data or resize so there's enough space
	this->parent->truncate((dataSize >> 2) + this->off);

	// No conversion needed, write out as-is
	uint8_t *imgData = (uint8_t *)newContent.get();
	this->data->seek(this->off << 3, stream::start);

	// Write the data as 2bpp
	while (dataSize--) {
		this->data->write(2, *imgData++);
	}

	return;
}

PaletteTablePtr CGAImage::getPalette()
	throw (stream::error)
{
	return CGAImage::generatePalette(this->cgaPal);
}

PaletteTablePtr CGAImage::generatePalette(CGAPaletteType cgaPal)
	throw ()
{
	PaletteTablePtr pal(new PaletteTable());
	pal->reserve(4);
	switch (cgaPal & 0x0F) {
		case 0: pal->push_back(PaletteEntry(0x00, 0x00, 0x00)); break;
		case 1: pal->push_back(PaletteEntry(0x00, 0x00, 0xAA)); break;
		case 2: pal->push_back(PaletteEntry(0x00, 0xAA, 0x00)); break;
		case 3: pal->push_back(PaletteEntry(0x00, 0xAA, 0xAA)); break;
		case 4: pal->push_back(PaletteEntry(0xAA, 0x00, 0x00)); break;
		case 5: pal->push_back(PaletteEntry(0xAA, 0x00, 0xAA)); break;
		case 6: pal->push_back(PaletteEntry(0xAA, 0x55, 0x00)); break;
		case 7: pal->push_back(PaletteEntry(0xAA, 0xAA, 0xAA)); break;
		case 8: pal->push_back(PaletteEntry(0x55, 0x55, 0x55)); break;
		case 9: pal->push_back(PaletteEntry(0x55, 0x55, 0xFF)); break;
		case 10: pal->push_back(PaletteEntry(0x55, 0xFF, 0x55)); break;
		case 11: pal->push_back(PaletteEntry(0x55, 0xFF, 0xFF)); break;
		case 12: pal->push_back(PaletteEntry(0xFF, 0x55, 0x55)); break;
		case 13: pal->push_back(PaletteEntry(0xFF, 0x55, 0xFF)); break;
		case 14: pal->push_back(PaletteEntry(0xFF, 0xFF, 0x55)); break;
		case 15: pal->push_back(PaletteEntry(0xFF, 0xFF, 0xFF)); break;
	}

	switch (cgaPal & 0xF0) {
		case GreenRed:
			pal->push_back(PaletteEntry(0x00, 0xAA, 0x00));
			pal->push_back(PaletteEntry(0xAA, 0x00, 0x00));
			pal->push_back(PaletteEntry(0xAA, 0xAA, 0x00));
			break;
		case GreenRedBright:
			pal->push_back(PaletteEntry(0x00, 0xFF, 0x00));
			pal->push_back(PaletteEntry(0xFF, 0x00, 0x00));
			pal->push_back(PaletteEntry(0xFF, 0xFF, 0x00));
			break;
		case CyanMagenta:
			pal->push_back(PaletteEntry(0x00, 0xAA, 0xAA));
			pal->push_back(PaletteEntry(0xAA, 0x00, 0xAA));
			pal->push_back(PaletteEntry(0xAA, 0xAA, 0xAA));
			break;
		case CyanMagentaBright:
			pal->push_back(PaletteEntry(0x00, 0xFF, 0xFF));
			pal->push_back(PaletteEntry(0xFF, 0x00, 0xFF));
			pal->push_back(PaletteEntry(0xFF, 0xFF, 0xFF));
			break;
		case CyanRed:
			pal->push_back(PaletteEntry(0x00, 0xAA, 0xAA));
			pal->push_back(PaletteEntry(0xAA, 0x00, 0x00));
			pal->push_back(PaletteEntry(0xAA, 0xAA, 0xAA));
			break;
		case CyanRedBright:
			pal->push_back(PaletteEntry(0x00, 0xFF, 0xFF));
			pal->push_back(PaletteEntry(0xFF, 0x00, 0x00));
			pal->push_back(PaletteEntry(0xFF, 0xFF, 0xFF));
			break;
	}
	return pal;
}

} // namespace gamegraphics
} // namespace camoto
