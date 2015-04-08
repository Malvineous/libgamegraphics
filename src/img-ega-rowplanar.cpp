/**
 * @file  img-ega-rowplanar.cpp
 * @brief Image implementation adding support for the EGA row-planar format.
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

#include <cstring>  // memset
#include <cassert>
#include <camoto/iostream_helpers.hpp>
#include "img-ega-rowplanar.hpp"

namespace camoto {
namespace gamegraphics {

Image_EGA_RowPlanar::Image_EGA_RowPlanar(std::unique_ptr<stream::inout> content,
	stream::pos offset, Point dimensions, EGAPlaneLayout planes,
	std::shared_ptr<const Palette> pal)
	:	Image_EGA(std::move(content), dimensions, planes, pal),
		offset(offset)
{
}

Image_EGA_RowPlanar::~Image_EGA_RowPlanar()
{
}

void Image_EGA_RowPlanar::convert(const Pixels& newContent,
	const Pixels& newMask)
{
	this->content->seekp(this->offset, stream::start);
	auto dims = this->dimensions();

	// For each pixel, set the appropriate bits in the cell
	for (unsigned int y = 0; y < dims.y; y++) {

		for (auto p : this->planes) {
			if (p == EGAPlanePurpose::Unused) break;;

			auto imgData = &newContent[y * dims.x];
			auto maskData = &newMask[y * dims.x];

			// Run through each lot of eight pixels (a "cell")
			for (unsigned int x = 0; x < dims.x; x += 8) {

				bool doMask, swap;
				uint8_t value;
				switch (p) {
					case EGAPlanePurpose::Unused: continue;
					case EGAPlanePurpose::Blank:      doMask = false; value = 0x00; swap = false; break;
					case EGAPlanePurpose::Blue0:      doMask = false; value = 0x01; swap = true;  break;
					case EGAPlanePurpose::Blue1:      doMask = false; value = 0x01; swap = false; break;
					case EGAPlanePurpose::Green0:     doMask = false; value = 0x02; swap = true;  break;
					case EGAPlanePurpose::Green1:     doMask = false; value = 0x02; swap = false; break;
					case EGAPlanePurpose::Red0:       doMask = false; value = 0x04; swap = true;  break;
					case EGAPlanePurpose::Red1:       doMask = false; value = 0x04; swap = false; break;
					case EGAPlanePurpose::Intensity0: doMask = false; value = 0x08; swap = true;  break;
					case EGAPlanePurpose::Intensity1: doMask = false; value = 0x08; swap = false; break;
					case EGAPlanePurpose::Hit0:       doMask = true;  value = (uint8_t)Mask::Touch;       swap = true;  break;
					case EGAPlanePurpose::Hit1:       doMask = true;  value = (uint8_t)Mask::Touch;       swap = false; break;
					case EGAPlanePurpose::Opaque0:    doMask = true;  value = (uint8_t)Mask::Transparent; swap = false;  break;
					case EGAPlanePurpose::Opaque1:    doMask = true;  value = (uint8_t)Mask::Transparent; swap = true; break;
				}

				// Work out if this plane will read from the input image or mask data.
				auto rowData = doMask ? maskData : imgData;

				// See how many bits we should run through.  This is only used
				// when the image is not an even multiple of 8.
				unsigned int bits = 8;
				if (x + 8 > dims.x) bits = dims.x % 8;

				// Run through each pixel in the group
				uint8_t c = 0;
				for (unsigned int b = 0; b < bits; b++) {
					bool on = rowData[b] & value;
					if ((on && !swap) || (!on && swap)) {
						// The pixel is on in this plane
						c |= 0x80 >> b;
					} // else the pixel is off in this plane
				}

				*this->content << u8(c);

				imgData += 8;
				maskData += 8;
			}
		}
	}
	this->content->truncate_here();
	this->content->flush();
	return;
}

void Image_EGA_RowPlanar::doConversion()
{
	this->content->seekg(this->offset, stream::start);

	auto dims = this->dimensions();
	this->pixels = Pixels(dims.x * dims.y, '\x00');
	this->mask = Pixels(dims.x * dims.y, '\x00');

	for (unsigned int y = 0; y < dims.y; y++) {

		for (auto p : this->planes) {
			if (p == EGAPlanePurpose::Unused) break;

			auto imgData = &this->pixels[y * dims.x];
			auto maskData = &this->mask[y * dims.x];

			// Run through each lot of eight pixels (a "cell"), including a partial
			// cell at the end if the width isn't a multiple of 8.
			for (unsigned int x = 0; x < dims.x; x += 8) {
				bool doMask, swap;
				uint8_t value;
				switch (p) {
					case EGAPlanePurpose::Unused: continue;
					case EGAPlanePurpose::Blank:      doMask = false; value = 0x00; swap = false; break;
					case EGAPlanePurpose::Blue0:      doMask = false; value = 0x01; swap = true;  break;
					case EGAPlanePurpose::Blue1:      doMask = false; value = 0x01; swap = false; break;
					case EGAPlanePurpose::Green0:     doMask = false; value = 0x02; swap = true;  break;
					case EGAPlanePurpose::Green1:     doMask = false; value = 0x02; swap = false; break;
					case EGAPlanePurpose::Red0:       doMask = false; value = 0x04; swap = true;  break;
					case EGAPlanePurpose::Red1:       doMask = false; value = 0x04; swap = false; break;
					case EGAPlanePurpose::Intensity0: doMask = false; value = 0x08; swap = true;  break;
					case EGAPlanePurpose::Intensity1: doMask = false; value = 0x08; swap = false; break;
					case EGAPlanePurpose::Hit0:       doMask = true;  value = (uint8_t)Mask::Touch;       swap = true;  break;
					case EGAPlanePurpose::Hit1:       doMask = true;  value = (uint8_t)Mask::Touch;       swap = false; break;
					case EGAPlanePurpose::Opaque0:    doMask = true;  value = (uint8_t)Mask::Transparent; swap = false;  break;
					case EGAPlanePurpose::Opaque1:    doMask = true;  value = (uint8_t)Mask::Transparent; swap = true; break;
				}

				uint8_t nextByte;
				try {
					*this->content >> u8(nextByte);
				} catch (const stream::incomplete_read&) {
					std::cerr << "ERROR: Incomplete read converting image to standard "
						"format.  Returning partial conversion." << std::endl;
					return;
				}

				// See which bit we should read down to (starting at 7.)  This is only
				// used when the image is not an even multiple of 8.
				int bits = 0;
				if (x + 8 > dims.x) bits = 8 - (dims.x % 8);

				// Run through all the (valid) bits in this byte
				auto rowData = doMask ? maskData : imgData;
				for (int b = 7; b >= bits; b--) {
					*rowData++ |= (((nextByte >> b) & 1) ^ swap) ? value : 0x00;
				}
				imgData += 8;
				maskData += 8;
			}
		}
	}
	return;
}

} // namespace gamegraphics
} // namespace camoto
