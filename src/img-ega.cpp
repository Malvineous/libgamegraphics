/**
 * @file  img-ega.cpp
 * @brief Partial Image implementation common to all EGA formats.
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
#include "img-ega.hpp"

namespace camoto {
namespace gamegraphics {

Image_EGA::Image_EGA(std::unique_ptr<stream::inout> content, stream::pos offset,
	Point dimensions, EGAPlaneLayout planes, std::shared_ptr<const Palette> pal)
	:	content(std::move(content)),
		offset(offset),
		dims(dimensions),
		planes(planes)
{
	this->pal = pal;
}

Image_EGA::~Image_EGA()
{
}

Image::Caps Image_EGA::caps() const
{
	return (this->pal ? Caps::HasPalette : Caps::Default);
}

ColourDepth Image_EGA::colourDepth() const
{
	return ColourDepth::EGA;
}

Point Image_EGA::dimensions() const
{
	return this->dims;
}

void Image_EGA::dimensions(const Point& newDimensions)
{
	assert(this->caps() & Caps::SetDimensions);

	int numPlanes = 0;
	for (auto& p : this->planes) {
		// Count the plane if its order is nonzero, otherwise ignore it
		if (p != EGAPlanePurpose::Unused) numPlanes++;
	}

	// TODO: Confirm this is correct
	this->content->truncate(this->offset +
		(newDimensions.x * numPlanes + 7) / 8 * newDimensions.y);
	this->dims = newDimensions;
	return;
}

Pixels Image_EGA::convert() const
{
	if (this->pixels.size() == 0) {
		// Populate cache
		auto noconst_this = const_cast<Image_EGA*>(this);
		noconst_this->doConversion();
	}
	return this->pixels;
}

Pixels Image_EGA::convert_mask() const
{
	if (this->mask.size() == 0) {
		bool hasMask = false;
		for (auto& p : this->planes) {
			if (
				(p == EGAPlanePurpose::Opaque0) ||
				(p == EGAPlanePurpose::Opaque1) ||
				(p == EGAPlanePurpose::Hit0) ||
				(p == EGAPlanePurpose::Hit1)
			) {
				hasMask = true;
				break;
			}
		}
		// Populate cache
		auto noconst_this = const_cast<Image_EGA*>(this);
		if (!hasMask) {
			// Mask is unused, skip the conversion and return an opaque mask
			auto dims = this->dimensions();
			assert((dims.x != 0) && (dims.y != 0));
			int dataSize = dims.x * dims.y;

			// Return an entirely opaque mask
			noconst_this->mask = Pixels(dataSize, 0x00);
		} else {
			noconst_this->doConversion();
		}
	}

	return this->mask;
}

} // namespace gamegraphics
} // namespace camoto
