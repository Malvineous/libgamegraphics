/**
 * @file   img-vga.cpp
 * @brief  Image implementation adding support for VGA mode 13 format.
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

#include "img-vga.hpp"

namespace camoto {
namespace gamegraphics {

VGAImage::VGAImage(iostream_sptr data,
	FN_TRUNCATE fnTruncate, io::stream_offset off)
	throw () :
		data(data),
		fnTruncate(fnTruncate),
		off(off)
{
}

VGAImage::~VGAImage()
	throw ()
{
}

int VGAImage::getCaps()
	throw ()
{
	return ColourDepthVGA;
}

StdImageDataPtr VGAImage::toStandard()
	throw ()
{
	unsigned int width, height;
	this->getDimensions(&width, &height);
	assert((width != 0) && (height != 0));
	int dataSize = width * height;

	uint8_t *imgData = new uint8_t[dataSize];
	StdImageDataPtr ret(imgData);
	this->data->seekg(this->off, std::ios::beg);
	this->data->rdbuf()->sgetn((char *)imgData, dataSize);

	return ret;
}

StdImageDataPtr VGAImage::toStandardMask()
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

void VGAImage::fromStandard(StdImageDataPtr newContent,
	StdImageDataPtr newMask
)
	throw ()
{
	unsigned int width, height;
	this->getDimensions(&width, &height);
	assert((width != 0) && (height != 0));
	int dataSize = width * height;

	this->data->seekp(0, std::ios::end);
	io::stream_offset len = this->data->tellp();

	// Cut off any leftover data or resize so there's enough space
	if (dataSize + this->off != len) {
		assert(this->fnTruncate);
		this->fnTruncate(dataSize + this->off);
	} // else size didn't need to change, e.g. fixed-size VGA image

	// No conversion needed, write out as-is
	uint8_t *imgData = (uint8_t *)newContent.get();
	this->data->seekp(this->off, std::ios::beg);
	this->data->rdbuf()->sputn((char *)imgData, dataSize);

	return;
}

} // namespace gamegraphics
} // namespace camoto
