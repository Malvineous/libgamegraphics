/**
 * @file	img-ega-linear.cpp
 * @brief Image implementation adding support for linear EGA graphics.
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

#include "img-ega-linear.hpp"

namespace camoto {
namespace gamegraphics {

Image_EGA_Linear::Image_EGA_Linear(std::unique_ptr<stream::inout> content,
	stream::pos offset, Point dimensions, EGAPlaneLayout planes,
	bitstream::endian endian, std::shared_ptr<const Palette> pal)
	:	Image_EGA(std::move(content), offset, dimensions, planes, pal),
		bits(this->content, endian)
{
}

Image_EGA_Linear::~Image_EGA_Linear()
{
}

void Image_EGA_Linear::convert(const Pixels& newContent,
	const Pixels& newMask)
{
	this->bits.seek(this->offset * 8, stream::start);

	auto dims = this->dimensions();

	auto imgData = &newContent[0];
	auto maskData = &newMask[0];
	for (unsigned int y = 0; y < dims.y; y++) {
		// Run through each lot of eight pixels (a "cell"), including a partial
		// cell at the end if the width isn't a multiple of 8.
		for (unsigned int x = 0; x < dims.x; x++) {
			for (auto p : this->planes) {
				// Don't waste time processing a plane we're ignoring
				if (p == EGAPlanePurpose::Unused) break;

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

				auto rowData = doMask ? maskData : imgData;

				bool bit = *rowData & value;
				if (swap) bit ^= 1;

				this->bits.write(1, bit ? 1 : 0);
			}
			imgData++;
			maskData++;
		}
		// Always start each row on a byte boundary
		this->bits.flushByte();
	}
	this->bits.flush();
	return;
}

void Image_EGA_Linear::doConversion()
{
	this->bits.seek(this->offset * 8, stream::start);

	auto dims = this->dimensions();
	this->pixels = Pixels(dims.x * dims.y, '\x00');
	this->mask = Pixels(dims.x * dims.y, '\x00');

	auto imgData = &this->pixels[0];
	auto maskData = &this->mask[0];
	for (unsigned int y = 0; y < dims.y; y++) {
		// Run through each lot of eight pixels (a "cell"), including a partial
		// cell at the end if the width isn't a multiple of 8.
		for (unsigned int x = 0; x < dims.x; x++) {
			for (auto p : this->planes) {
				// Don't waste time processing a plane we're ignoring
				if (p == EGAPlanePurpose::Unused) break;

				unsigned int bit;
				this->bits.read(1, &bit);
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

				auto rowData = doMask ? maskData : imgData;

				if (swap) bit ^= 1;

				if (bit) *rowData |= value;
			}
			imgData++;
			maskData++;
		}
		// Always start each row on a byte boundary
		this->bits.flushByte();
	}
	return;
}


} // namespace gamegraphics
} // namespace camoto
