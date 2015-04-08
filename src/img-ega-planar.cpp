/**
 * @file  img-ega-planar.cpp
 * @brief Image implementation adding support for the EGA planar format.
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
#include <camoto/util.hpp> // make_unique
#include <camoto/iostream_helpers.hpp>
#include "img-ega-planar.hpp"

namespace camoto {
namespace gamegraphics {

Image_EGA_Planar::Image_EGA_Planar(std::unique_ptr<stream::inout> content,
	stream::pos offset, Point dimensions, EGAPlaneLayout planes,
	std::shared_ptr<const Palette> pal)
	:	Image_EGA(std::move(content), dimensions, planes, pal),
		offset(offset)
{
}

Image_EGA_Planar::~Image_EGA_Planar()
{
}

void Image_EGA_Planar::convert(const Pixels& newContent,
	const Pixels& newMask)
{
	this->content->seekp(0, stream::start);

	for (auto p : this->planes) {

		// NOTE: This will keep going after the unused plane when technically
		// we should be inserting some blank data or something.  Maybe will
		// implement it if it ever becomes necessary.
		if (p == PlanePurpose::Unused) continue;

		bool doMask, swap;
		uint8_t value;
		switch (p) {
			case PlanePurpose::Unused: continue;
			case PlanePurpose::Blue0:      doMask = false; value = 0x01; swap = true;  break;
			case PlanePurpose::Blue1:      doMask = false; value = 0x01; swap = false; break;
			case PlanePurpose::Green0:     doMask = false; value = 0x02; swap = true;  break;
			case PlanePurpose::Green1:     doMask = false; value = 0x02; swap = false; break;
			case PlanePurpose::Red0:       doMask = false; value = 0x04; swap = true;  break;
			case PlanePurpose::Red1:       doMask = false; value = 0x04; swap = false; break;
			case PlanePurpose::Intensity0: doMask = false; value = 0x08; swap = true;  break;
			case PlanePurpose::Intensity1: doMask = false; value = 0x08; swap = false; break;
			case PlanePurpose::Hit0:       doMask = true;  value = (uint8_t)Mask::Touch;       swap = true;  break;
			case PlanePurpose::Hit1:       doMask = true;  value = (uint8_t)Mask::Touch;       swap = false; break;
			case PlanePurpose::Opaque0:    doMask = true;  value = (uint8_t)Mask::Transparent; swap = false;  break;
			case PlanePurpose::Opaque1:    doMask = true;  value = (uint8_t)Mask::Transparent; swap = true; break;
		}

		// For each pixel, set the appropriate bits in the cell
		const uint8_t *imgData = &newContent[0];
		const uint8_t *maskData = &newMask[0];

		for (unsigned int y = 0; y < this->dims.y; y++) {

			// Run through each lot of eight pixels (a "cell")
			for (unsigned int x = 0; x < (this->dims.x + 7) / 8; x++) {

				uint8_t c = 0;

				// Work out if this plane will read from the input image or mask data.
				const uint8_t *rowData;
				if (doMask) rowData = maskData;
				else rowData = imgData;

				// See how many bits we should run through.  This is only used
				// when the image is not an even multiple of 8.
				unsigned int bits = 8;
				if (x * 8 + 8 > this->dims.x) bits = this->dims.x % 8;

				// Run through each pixel in the group
				for (unsigned int b = 0; b < bits; b++) {
					bool on = rowData[x * 8 + b] & value;
					if ((on && !swap) || (!on && swap)) {//(rowData[x * 8 + b] & value) {
						// The pixel is on in this plane
						c |= 0x80 >> b;
					} // else the pixel is off in this plane
				}

				*this->content << u8(c);
			}
			imgData += this->dims.x;
			maskData += this->dims.x;
		}
	}
	this->content->truncate_here();
	this->content->flush();
	return;
}

void Image_EGA_Planar::doConversion()
{
	this->content->seekg(0, stream::start);

	auto dims = this->dimensions();
	this->pixels = Pixels(dims.x * dims.y, '\x00');
	this->mask = Pixels(dims.x * dims.y, '\x00');

	unsigned int planeSizeBytes = dims.y * ((dims.x + 7) / 8);
	stream::len lenSkip = 0;
	for (auto p : this->planes) {
		if (p == PlanePurpose::Unused) {
			// Don't waste time processing a plane we're ignoring
			lenSkip += planeSizeBytes;
			// Don't do the skip here in case the unused planes are all at the end
			// and we will end up seeking past EOF
			continue;
		}
		if (lenSkip) {
			// We're processing this plane and it's following one or more ignored
			// planes, so now do the seek.
			this->content->seekg(lenSkip, stream::cur);
		}

		bool doMask, swap;
		uint8_t value;
		switch (p) {
			case PlanePurpose::Unused: continue;
			case PlanePurpose::Blue0:      doMask = false; value = 0x01; swap = true;  break;
			case PlanePurpose::Blue1:      doMask = false; value = 0x01; swap = false; break;
			case PlanePurpose::Green0:     doMask = false; value = 0x02; swap = true;  break;
			case PlanePurpose::Green1:     doMask = false; value = 0x02; swap = false; break;
			case PlanePurpose::Red0:       doMask = false; value = 0x04; swap = true;  break;
			case PlanePurpose::Red1:       doMask = false; value = 0x04; swap = false; break;
			case PlanePurpose::Intensity0: doMask = false; value = 0x08; swap = true;  break;
			case PlanePurpose::Intensity1: doMask = false; value = 0x08; swap = false; break;
			case PlanePurpose::Hit0:       doMask = true;  value = (uint8_t)Mask::Touch;       swap = true;  break;
			case PlanePurpose::Hit1:       doMask = true;  value = (uint8_t)Mask::Touch;       swap = false; break;
			case PlanePurpose::Opaque0:    doMask = true;  value = (uint8_t)Mask::Transparent; swap = false;  break;
			case PlanePurpose::Opaque1:    doMask = true;  value = (uint8_t)Mask::Transparent; swap = true; break;
		}

		auto rowData = doMask ? &this->mask[0] : &this->pixels[0];
		for (unsigned int y = 0; y < dims.y; y++) {
			// Run through each lot of eight pixels (a "cell"), including a partial
			// cell at the end if the width isn't a multiple of 8.
			for (unsigned int x = 0; x < dims.x; x += 8) {

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
				for (int b = 7; b >= bits; b--) {
					*rowData++ |= (((nextByte >> b) & 1) ^ swap) ? value : 0x00;
				}
			}
		}
	}
	return;
}


//
// ImageType_EGA_RawPlanarBGRI
//

ImageType_EGA_RawPlanarBGRI::ImageType_EGA_RawPlanarBGRI()
{
}

ImageType_EGA_RawPlanarBGRI::~ImageType_EGA_RawPlanarBGRI()
{
}

std::string ImageType_EGA_RawPlanarBGRI::code() const
{
	return "img-ega-raw-planar-bgri-fullscreen";
}

std::string ImageType_EGA_RawPlanarBGRI::friendlyName() const
{
	return "Raw Planar EGA fullscreen image (BGRI)";
}

std::vector<std::string> ImageType_EGA_RawPlanarBGRI::fileExtensions() const
{
	std::vector<std::string> vcExtensions;
	return vcExtensions;
}

std::vector<std::string> ImageType_EGA_RawPlanarBGRI::games() const
{
	std::vector<std::string> vcGames;
	return vcGames;
}

ImageType::Certainty ImageType_EGA_RawPlanarBGRI::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();

	// TESTED BY: TODO
	if (len == 32000) return PossiblyYes;

	// TESTED BY: TODO
	return DefinitelyNo;
}

std::unique_ptr<Image> ImageType_EGA_RawPlanarBGRI::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->truncate(32000);
	content->seekp(0, stream::start);
	char buf[64];
	memset(buf, 0, 64);
	for (int i = 0; i < 500; i++) content->write(buf, 64);

	SuppData dummy;
	return this->open(std::move(content), dummy);
}

std::unique_ptr<Image> ImageType_EGA_RawPlanarBGRI::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	return std::make_unique<Image_EGA_Planar>(
		std::move(content), 0,
		Point{320, 200},
		EGAPlaneLayout{
			PlanePurpose::Blue1,
			PlanePurpose::Green1,
			PlanePurpose::Red1,
			PlanePurpose::Intensity1,
			PlanePurpose::Unused,
			PlanePurpose::Unused
		},
		nullptr
	);
}

SuppFilenames ImageType_EGA_RawPlanarBGRI::getRequiredSupps(
	const std::string& filenameImage) const
{
	return SuppFilenames();
}


} // namespace gamegraphics
} // namespace camoto
