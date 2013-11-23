/**
 * @file   img-vga-planar.cpp
 * @brief  Image implementation adding support for VGA mode X planar format.
 *
 * Copyright (C) 2010-2013 Adam Nielsen <malvineous@shikadi.net>
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

#include "img-vga-planar.hpp"

namespace camoto {
namespace gamegraphics {

VGAPlanarImage::VGAPlanarImage(stream::inout_sptr data,
	stream::pos off)
	:	data(data),
		off(off)
{
}

VGAPlanarImage::~VGAPlanarImage()
{
}

int VGAPlanarImage::getCaps()
{
	return Image::ColourDepthVGA;
}

StdImageDataPtr VGAPlanarImage::toStandard()
{
	unsigned int width, height;
	this->getDimensions(&width, &height);
	assert((width != 0) && (height != 0));
	unsigned long dataSize = width * height;

	uint8_t *rawData = new uint8_t[dataSize];
	StdImageDataPtr raw(rawData);
	uint8_t *imgData = new uint8_t[dataSize];
	StdImageDataPtr ret(imgData);
	this->data->seekg(this->off, stream::start);
	this->data->read(rawData, dataSize);

	// Convert the planar data to linear
	unsigned int planeWidth = width / 4;
	unsigned int planeSize = planeWidth * height;
	for (unsigned int i = 0; i < dataSize; i++) {
		imgData[i % planeSize * 4 + i / planeSize] = rawData[i];
	}

	return ret;
}

StdImageDataPtr VGAPlanarImage::toStandardMask()
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

void VGAPlanarImage::fromStandard(StdImageDataPtr newContent,
	StdImageDataPtr newMask
)
{
	unsigned int width, height;
	this->getDimensions(&width, &height);
	assert((width != 0) && (height != 0));
	unsigned long dataSize = width * height;

	stream::pos len = this->data->size();

	// Cut off any leftover data or resize so there's enough space
	if (dataSize + this->off != len) {
		this->data->truncate(dataSize + this->off);
	} // else size didn't need to change, e.g. fixed-size VGA image

	uint8_t *rawData = new uint8_t[dataSize];
	StdImageDataPtr raw(rawData);

	// Convert the linear data to planar
	unsigned int planeWidth = width / 4;
	unsigned int planeSize = planeWidth * height;
	for (unsigned int i = 0; i < dataSize; i++) {
		rawData[i] = newContent[i % planeSize * 4 + i / planeSize];
	}

	this->data->seekp(this->off, stream::start);
	this->data->write(rawData, dataSize);

	return;
}

} // namespace gamegraphics
} // namespace camoto
