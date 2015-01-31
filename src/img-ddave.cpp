/**
 * @file  img-ddave.cpp
 * @brief Image specialisation for Dangerous Dave CGA/EGA/VGA images.
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

#include <camoto/iostream_helpers.hpp>
#include "img-ddave.hpp"

namespace camoto {
namespace gamegraphics {

Image_DDaveCGA::Image_DDaveCGA(stream::inout_sptr data,
	bool fixedSize)
	:	Image_CGA(data, fixedSize ? 0 : 4, 16, 16, CGAPal_CyanMagentaBright),
		stream_data(data),
		fixedSize(fixedSize)
{
	if (!fixedSize) {
		data >> u16le(this->width) >> u16le(this->height);
	}
}

Image_DDaveCGA::~Image_DDaveCGA()
{
}

int Image_DDaveCGA::getCaps()
{
	return this->Image_CGA::getCaps()
		| (this->fixedSize ? 0 : Image::CanSetDimensions);
}

void Image_DDaveCGA::fromStandard(StdImageDataPtr newContent,
	StdImageDataPtr newMask
)
{
	this->Image_CGA::fromStandard(newContent, newMask);

	if (!this->fixedSize) {
		// Update offset
		this->stream_data->seekp(0, stream::start);
		this->stream_data << u16le(this->width) << u16le(this->height);
	}
	return;
}


Image_DDaveEGA::Image_DDaveEGA(stream::inout_sptr data,
	bool fixedSize)
	:	fixedSize(fixedSize)
{
	if (fixedSize) this->width = this->height = 16;
	else {
		data >> u16le(this->width) >> u16le(this->height);
	}

	PLANE_LAYOUT planes;
	planes[PLANE_BLUE] = 4;
	planes[PLANE_GREEN] = 3;
	planes[PLANE_RED] = 2;
	planes[PLANE_INTENSITY] = 1;
	planes[PLANE_HITMAP] = 0;
	planes[PLANE_OPACITY] = 0;
	this->setParams(data, fixedSize ? 0 : 4,
		this->width, this->height, planes);
}

Image_DDaveEGA::~Image_DDaveEGA()
{
}

int Image_DDaveEGA::getCaps()
{
	return this->Image_EGARowPlanar::getCaps()
		| (this->fixedSize ? 0 : Image::CanSetDimensions);
}

void Image_DDaveEGA::fromStandard(StdImageDataPtr newContent,
	StdImageDataPtr newMask
)
{
	this->Image_EGARowPlanar::fromStandard(newContent, newMask);

	if (!this->fixedSize) {
		// Update offset
		this->data->seekp(0, stream::start);
		this->data << u16le(this->width) << u16le(this->height);
	}
	return;
}



Image_DDaveVGA::Image_DDaveVGA(stream::inout_sptr data,
	bool fixedSize, PaletteTablePtr pal)
	:	Image_VGA(data, fixedSize ? 0 : 4),
		fixedSize(fixedSize),
		pal(pal)
{
	assert(data->tellg() == 0);

	if (fixedSize) this->width = this->height = 16;
	else {
		if (data->size() == 0) {
			// New tile
			this->width = this->height = 0;
		} else {
			data >> u16le(this->width) >> u16le(this->height);
		}
	}
}

Image_DDaveVGA::~Image_DDaveVGA()
{
}

int Image_DDaveVGA::getCaps()
{
	return this->Image_VGA::getCaps()
		| Image::HasPalette
		| (this->fixedSize ? 0 : Image::CanSetDimensions);
}

void Image_DDaveVGA::getDimensions(unsigned int *width, unsigned int *height)
{
	*width = this->width;
	*height = this->height;
	return;
}

void Image_DDaveVGA::setDimensions(unsigned int width, unsigned int height)
{
	assert(this->getCaps() & Image::CanSetDimensions);
	this->width = width;
	this->height = height;
	return;
}

void Image_DDaveVGA::fromStandard(StdImageDataPtr newContent,
	StdImageDataPtr newMask)
{
	this->Image_VGA::fromStandard(newContent, newMask);

	if (!this->fixedSize) {
		// Update dimensions
		this->data->seekp(0, stream::start);
		this->data << u16le(this->width) << u16le(this->height);
	}
	return;
}

PaletteTablePtr Image_DDaveVGA::getPalette()
{
	return this->pal;
}

} // namespace gamegraphics
} // namespace camoto
