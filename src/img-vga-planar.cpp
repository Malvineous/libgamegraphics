/**
 * @file  img-vga-planar.cpp
 * @brief Image implementation adding support for VGA mode X planar format.
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

#include <cassert>
#include "img-vga-planar.hpp"

namespace camoto {
namespace gamegraphics {

Image_VGA_Planar::Image_VGA_Planar(std::unique_ptr<stream::inout> content,
	stream::pos off)
	:	content(std::move(content)),
		off(off)
{
}

Image_VGA_Planar::~Image_VGA_Planar()
{
}

Image::Caps Image_VGA_Planar::caps() const
{
	return Caps::Default;
}

ColourDepth Image_VGA_Planar::colourDepth() const
{
	return ColourDepth::VGA;
}

Pixels Image_VGA_Planar::convert() const
{
	auto dims = this->dimensions();
	unsigned long dataSize = dims.x * dims.y;

	Pixels src, dst;
	src.resize(dataSize, 0);
	dst.resize(dataSize, 0);

	this->content->seekg(this->off, stream::start);
	this->content->read(src.data(), dataSize);

	// Convert the planar data to linear
	unsigned int planeWidth = dims.x / 4;
	unsigned int planeSize = planeWidth * dims.y;
	for (unsigned int i = 0; i < dataSize; i++) {
		dst[i % planeSize * 4 + i / planeSize] = src[i];
	}

	return dst;
}

Pixels Image_VGA_Planar::convert_mask() const
{
	auto dims = this->dimensions();
	int dataSize = dims.x * dims.y;

	// Return an entirely opaque mask
	Pixels pix;
	pix.resize(dataSize, 0);
	return pix;
}

void Image_VGA_Planar::convert(const Pixels& newContent,
	const Pixels& newMask)
{
	auto dims = this->dimensions();
	unsigned long dataSize = dims.x * dims.y;

	stream::pos len = this->content->size();

	// Cut off any leftover data or resize so there's enough space
	if (dataSize + this->off != len) {
		this->content->truncate(dataSize + this->off);
	} // else size didn't need to change, e.g. fixed-size VGA image

	Pixels pix;
	pix.resize(dataSize, 0);

	// Convert the linear data to planar
	unsigned int planeWidth = dims.x / 4;
	unsigned int planeSize = planeWidth * dims.y;
	for (unsigned int i = 0; i < dataSize; i++) {
		pix[i] = newContent[i % planeSize * 4 + i / planeSize];
	}

	this->content->seekp(this->off, stream::start);
	this->content->write(pix.data(), dataSize);

	return;
}

} // namespace gamegraphics
} // namespace camoto
