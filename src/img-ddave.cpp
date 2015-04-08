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

#include <cassert>
#include <camoto/iostream_helpers.hpp>
#include "img-ddave.hpp"

namespace camoto {
namespace gamegraphics {

Image_DDaveCGA::Image_DDaveCGA(std::unique_ptr<stream::inout> content,
	bool fixedSize)
	:	Image_EGA_Linear(
			std::move(content),
			fixedSize ? 0 : 4,
			Point{16, 16},
			EGAPlaneLayout{
				EGAPlanePurpose::Green1,
				EGAPlanePurpose::Blue1,
			},
			bitstream::endian::bigEndian,
			createPalette_CGA(CGAPaletteType::CyanMagentaBright)
		),
		fixedSize(fixedSize)
{
	if (!fixedSize) {
		this->content->seekg(0, stream::start);
		*this->content
			>> u16le(this->dims.x)
			>> u16le(this->dims.y)
		;
	}
}

Image_DDaveCGA::~Image_DDaveCGA()
{
}

Image::Caps Image_DDaveCGA::caps() const
{
	return this->Image_EGA_Linear::caps() // handles palette caps
		| (this->fixedSize ? Image::Caps::Default : Image::Caps::SetDimensions);
}

ColourDepth Image_DDaveCGA::colourDepth() const
{
	return ColourDepth::CGA;
}

void Image_DDaveCGA::convert(const Pixels& newContent, const Pixels& newMask)
{
	if (!this->fixedSize) {
		auto dims = this->dimensions();
		// Update offset
		this->content->seekp(0, stream::start);
		*this->content
			<< u16le(dims.x)
			<< u16le(dims.y)
		;
	}
	this->Image_EGA_Linear::convert(newContent, newMask);
	return;
}


Image_DDaveEGA::Image_DDaveEGA(std::unique_ptr<stream::inout> content,
	bool fixedSize)
	:	Image_EGA_RowPlanar(
			std::move(content),
			fixedSize ? 0 : 4,
			Point{16, 16},
			EGAPlaneLayout{
				EGAPlanePurpose::Intensity1,
				EGAPlanePurpose::Red1,
				EGAPlanePurpose::Green1,
				EGAPlanePurpose::Blue1,
			},
			nullptr // default EGA palette
		),
		fixedSize(fixedSize)
{
	if (!fixedSize) {
		this->content->seekg(0, stream::start);
		*this->content
			>> u16le(this->dims.x)
			>> u16le(this->dims.y)
		;
	}
}

Image_DDaveEGA::~Image_DDaveEGA()
{
}

Image::Caps Image_DDaveEGA::caps() const
{
	return this->Image_EGA_RowPlanar::caps()
		| (this->fixedSize ? Image::Caps::Default : Image::Caps::SetDimensions);
}

void Image_DDaveEGA::convert(const Pixels& newContent,
	const Pixels& newMask)
{
	if (!this->fixedSize) {
		auto dims = this->dimensions();
		// Update offset
		this->content->seekp(0, stream::start);
		*this->content
			<< u16le(dims.x)
			<< u16le(dims.y)
		;
	}
	this->Image_EGA_RowPlanar::convert(newContent, newMask);
	return;
}



Image_DDaveVGA::Image_DDaveVGA(std::unique_ptr<stream::inout> content,
	bool fixedSize, std::shared_ptr<Palette> pal)
	:	Image_VGA(std::move(content), fixedSize ? 0 : 4),
		fixedSize(fixedSize)
{
	this->pal = pal;
	if (fixedSize) this->dims.x = this->dims.y = 16;
	else {
		if (this->content->size() == 0) {
			// New tile
			this->dims.x = this->dims.y = 0;
		} else {
			this->content->seekg(0, stream::start);
			*this->content
				>> u16le(this->dims.x)
				>> u16le(this->dims.y)
			;
		}
	}
}

Image_DDaveVGA::~Image_DDaveVGA()
{
}

Image::Caps Image_DDaveVGA::caps() const
{
	return this->Image_VGA::caps()
		| Image::Caps::HasPalette
		| (this->fixedSize ? Image::Caps::Default : Image::Caps::SetDimensions);
}

Point Image_DDaveVGA::dimensions() const
{
	return this->dims;
}

void Image_DDaveVGA::dimensions(const Point& newDimensions)
{
	assert(this->caps() & Image::Caps::SetDimensions);
	this->dims = newDimensions;
	return;
}

void Image_DDaveVGA::convert(const Pixels& newContent,
	const Pixels& newMask)
{
	if (!this->fixedSize) {
		auto dims = this->dimensions();
		// Update offset
		this->content->seekp(0, stream::start);
		*this->content
			<< u16le(dims.x)
			<< u16le(dims.y)
		;
	}
	this->Image_VGA::convert(newContent, newMask);
	return;
}

} // namespace gamegraphics
} // namespace camoto
