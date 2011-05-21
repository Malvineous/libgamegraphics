/**
 * @file   img-ddave.cpp
 * @brief  Image specialisation for Dangerous Dave CGA/EGA/VGA images.
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

#include <camoto/iostream_helpers.hpp>
#include "img-ddave.hpp"

namespace camoto {
namespace gamegraphics {

DDaveCGAImage::DDaveCGAImage(iostream_sptr data, FN_TRUNCATE fnTruncate,
	bool fixedSize)
	throw () :
		CGAImage(data, fnTruncate, fixedSize ? 0 : 4, CGAImage::CyanMagentaBright),
		stream_data(data),
		fixedSize(fixedSize)
{
	if (fixedSize) this->width = this->height = 16;
	else {
		data >> u16le(this->width) >> u16le(this->height);
	}
}

DDaveCGAImage::~DDaveCGAImage()
	throw ()
{
}

int DDaveCGAImage::getCaps()
	throw ()
{
	return this->CGAImage::getCaps()
		| (this->fixedSize ? 0 : Image::CanSetDimensions);
}

void DDaveCGAImage::getDimensions(unsigned int *width, unsigned int *height)
	throw ()
{
	*width = this->width;
	*height = this->height;
	return;
}

void DDaveCGAImage::setDimensions(unsigned int width, unsigned int height)
	throw (std::ios::failure)
{
	assert(this->getCaps() & Image::CanSetDimensions);
	this->width = width;
	this->height = height;
	return;
}

void DDaveCGAImage::fromStandard(StdImageDataPtr newContent,
	StdImageDataPtr newMask
)
	throw ()
{
	this->CGAImage::fromStandard(newContent, newMask);

	if (!this->fixedSize) {
		// Update offset
		this->stream_data->seekp(0, std::ios::beg);
		this->stream_data << u16le(this->width) << u16le(this->height);
	}
	return;
}


DDaveEGAImage::DDaveEGAImage(iostream_sptr data, FN_TRUNCATE fnTruncate,
	bool fixedSize)
	throw () :
		fixedSize(fixedSize)
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
	this->setParams(data, fnTruncate, fixedSize ? 0 : 4,
		this->width, this->height, planes);
}

DDaveEGAImage::~DDaveEGAImage()
	throw ()
{
}

int DDaveEGAImage::getCaps()
	throw ()
{
	return this->EGARowPlanarImage::getCaps()
		| (this->fixedSize ? 0 : Image::CanSetDimensions);
}

void DDaveEGAImage::fromStandard(StdImageDataPtr newContent,
	StdImageDataPtr newMask
)
	throw ()
{
	this->EGARowPlanarImage::fromStandard(newContent, newMask);

	if (!this->fixedSize) {
		// Update offset
		this->data->seekp(0, std::ios::beg);
		this->data << u16le(this->width) << u16le(this->height);
	}
	return;
}



DDaveVGAImage::DDaveVGAImage(iostream_sptr data, FN_TRUNCATE fnTruncate,
	bool fixedSize, PaletteTablePtr pal)
	throw () :
		VGAImage(data, fnTruncate, fixedSize ? 0 : 4),
		fixedSize(fixedSize),
		pal(pal)
{
	assert(data->tellg() == 0);

	if (fixedSize) this->width = this->height = 16;
	else {
		data >> u16le(this->width) >> u16le(this->height);
	}
}

DDaveVGAImage::~DDaveVGAImage()
	throw ()
{
}

int DDaveVGAImage::getCaps()
	throw ()
{
	return this->VGAImage::getCaps()
		| Image::HasPalette
		| (this->fixedSize ? 0 : Image::CanSetDimensions);
}

void DDaveVGAImage::getDimensions(unsigned int *width, unsigned int *height)
	throw ()
{
	*width = this->width;
	*height = this->height;
	return;
}

void DDaveVGAImage::setDimensions(unsigned int width, unsigned int height)
	throw (std::ios::failure)
{
	assert(this->getCaps() & Image::CanSetDimensions);
	this->width = width;
	this->height = height;
	return;
}

void DDaveVGAImage::fromStandard(StdImageDataPtr newContent,
	StdImageDataPtr newMask
)
	throw ()
{
	this->VGAImage::fromStandard(newContent, newMask);

	if (!this->fixedSize) {
		// Update offset
		this->data->seekp(0, std::ios::beg);
		this->data << u16le(this->width) << u16le(this->height);
	}
	return;
}

PaletteTablePtr DDaveVGAImage::getPalette()
	throw (std::ios::failure)
{
	return this->pal;
}

} // namespace gamegraphics
} // namespace camoto
