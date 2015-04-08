/**
 * @file  img-vga.cpp
 * @brief Image implementation adding support for VGA mode 13 format.
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
#include "img-vga.hpp"

namespace camoto {
namespace gamegraphics {

Image_VGA::Image_VGA(std::unique_ptr<stream::inout> content,
	stream::pos off)
	:	content(std::move(content)),
		off(off)
{
}

Image_VGA::~Image_VGA()
{
}

Image::Caps Image_VGA::caps() const
{
	return Caps::Default;
}

ColourDepth Image_VGA::colourDepth() const
{
	return ColourDepth::VGA;
}

Pixels Image_VGA::convert() const
{
	auto dims = this->dimensions();
	unsigned long dataSize = dims.x * dims.y;

	Pixels pix;
	pix.resize(dataSize);
	this->content->seekg(this->off, stream::start);
	this->content->read(pix.data(), dataSize);
	return pix;
}

Pixels Image_VGA::convert_mask() const
{
	auto dims = this->dimensions();
	int dataSize = dims.x * dims.y;

	// Return an entirely opaque mask
	Pixels pix;
	pix.resize(dataSize, 0);
	return pix;
}

void Image_VGA::convert(const Pixels& newContent,
	const Pixels& newMask)
{
	auto dims = this->dimensions();
	unsigned long dataSize = dims.x * dims.y;

	stream::pos len = this->content->size();

	// Cut off any leftover data or resize so there's enough space
	if (dataSize + this->off != len) {
		this->content->truncate(dataSize + this->off);
	} // else size didn't need to change, e.g. fixed-size VGA image

	// No conversion needed, write out as-is
	this->content->seekp(this->off, stream::start);
	this->content->write(newContent.data(), dataSize);
	this->content->flush();
	return;
}

} // namespace gamegraphics
} // namespace camoto
