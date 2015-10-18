/**
 * @file  img-zone66_tile.cpp
 * @brief Image implementation for Zone66 tiles.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Zone_66_Tileset_Format
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
#include <camoto/util.hpp> // make_unique
#include "pal-vga-raw.hpp"
#include "img-zone66_tile.hpp"

/// Offset where the VGA image data begins
#define Z66_IMG_OFFSET 4

/// Palette index to make transparent
#define Z66_TRANSPARENT_COLOUR 0

/// Width or height larger than this is considered invalid file
#define Z66_MAX_DIMS 2048

namespace camoto {
namespace gamegraphics {

ImageType_Zone66Tile::ImageType_Zone66Tile()
{
}

ImageType_Zone66Tile::~ImageType_Zone66Tile()
{
}

std::string ImageType_Zone66Tile::code() const
{
	return "img-zone66_tile";
}

std::string ImageType_Zone66Tile::friendlyName() const
{
	return "Zone 66 tile";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> ImageType_Zone66Tile::fileExtensions() const
{
	return {};
}

std::vector<std::string> ImageType_Zone66Tile::games() const
{
	return {
		"Zone 66",
	};
}

ImageType::Certainty ImageType_Zone66Tile::isInstance(stream::input& content)
	const
{
	stream::pos len = content.size();

	// Too short
	// TESTED BY: img_zone66_tile_isinstance_c01
	if (len < 4) return DefinitelyNo;

	content.seekg(0, stream::start);
	Point dims;
	content >> u16le(dims.x) >> u16le(dims.y);
	len -= 4;

	// Image too large
	// TESTED BY: img_zone66_tile_isinstance_c02
	if ((dims.x > Z66_MAX_DIMS) || (dims.y > Z66_MAX_DIMS)) return DefinitelyNo;

	int imgSize = dims.x * dims.y;
	int imgPos = 0;
	while (len && (imgPos <= imgSize)) {
		uint8_t code;
		content >> u8(code);
		len--;
		switch (code) {
			case 0x00:
				// Invalid code
				// TESTED BY: img_zone66_tile_isinstance_c04
				return DefinitelyNo;
			case 0xFF: // end of image
				// EOF marker at EOF
				// TESTED BY: img_zone66_tile_isinstance_c00
				if (len == 0) return DefinitelyYes;

				// EOF marker with trailing data
				// TESTED BY: img_zone66_tile_isinstance_c05
				return DefinitelyNo;
			case 0xFE: // skip to EOL
				imgPos += (dims.x - (imgPos % dims.x)) % dims.x;
				break;
			case 0xFD:
				// Cut off in middle of 0xFD code
				// TESTED BY: img_zone66_tile_isinstance_c03
				if (!len) return DefinitelyNo;
				content >> u8(code);
				len--;
				imgPos += code;
				// Subtract code+1 from len, but make len=0 instead of going negative and wrapping
				//len -= std::min<stream::pos>(code+1, len);
				break;
			default:
				imgPos += code;
				// Subtract code from len, but make len=0 instead of going negative and wrapping
				len -= std::min<stream::pos>(code, len);
				if (len) content.seekg(code, stream::cur);
				break;
		}
	}

	// Read all data but didn't have 0xFF at EOF
	// TESTED BY: img_zone66_tile_isinstance_c06
	if ((len == 0) && (imgPos == imgSize)) return DefinitelyYes;

	// Ended mid-data
	// TESTED BY: img_zone66_tile_isinstance_c07
	return DefinitelyNo;
}

std::unique_ptr<Image> ImageType_Zone66Tile::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	*content
		<< u16le(0)  // width
		<< u16le(0)  // height
		<< u8(0xFF)  // end-of-image code
	;
	return this->open(std::move(content), suppData);
}

std::unique_ptr<Image> ImageType_Zone66Tile::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	std::shared_ptr<const Palette> pal;
	// Only load the palette if one was given
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		auto palFile = std::make_shared<Palette_VGA>(std::move(suppData[SuppItem::Palette]), 6);
		//auto newPal = std::make_shared<Palette>();
		//*newPal = *(palFile->palette());
		auto copyPal = std::make_shared<Palette>(*(palFile->palette()));
		copyPal->at(Z66_TRANSPARENT_COLOUR).alpha = 0;
		pal = copyPal;
	}
	return std::make_unique<Image_Zone66Tile>(std::move(content), pal);
}

SuppFilenames ImageType_Zone66Tile::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	SuppFilenames supps;
	supps[SuppItem::Palette] = "tpal.z66";
	return supps;
}


Image_Zone66Tile::Image_Zone66Tile(std::unique_ptr<stream::inout> content,
	std::shared_ptr<const Palette> pal)
	:	content(std::move(content))
{
	this->pal = pal;

	stream::len lenData = this->content->size();
	if (lenData == 64000) {
		// Headerless fullscreen image
		this->dims = {320, 200};
	} else {
		this->content->seekg(0, stream::start);
		*this->content
			>> u16le(this->dims.x)
			>> u16le(this->dims.y)
		;
	}
}


Image_Zone66Tile::~Image_Zone66Tile()
{
}

Image::Caps Image_Zone66Tile::caps() const
{
	return Caps::SetDimensions | Caps::HasPalette;
}

ColourDepth Image_Zone66Tile::colourDepth() const
{
	return ColourDepth::VGA;
}

Point Image_Zone66Tile::dimensions() const
{
	return this->dims;
}

void Image_Zone66Tile::dimensions(const Point& newDimensions)
{
	this->content->seekg(0, stream::end);
	if ((newDimensions.x == 320) && (newDimensions.y == 200)) {
		this->content->truncate(64000);
	} else {
		if (this->content->tellg() < 4) {
			// Need to enlarge stream to write image size
			this->content->truncate(4);
		}

		this->content->seekp(0, stream::start);
		*this->content
			<< u16le(newDimensions.x)
			<< u16le(newDimensions.y)
		;
	}
	this->dims = newDimensions;
	return;
}

Pixels Image_Zone66Tile::convert() const
{
	auto dims = this->dimensions();
	Pixels imgData(dims.x * dims.y, 0x00);

	// Full screen images are in a different format, so should never end up here.
	// The tls-zone66 handler should create an Image_VGARaw for those tiles
	// instead.
	assert((dims.x != 320) && (dims.y != 200));

	this->content->seekg(Z66_IMG_OFFSET, stream::start);
	unsigned int y = 0;
	for (unsigned int i = 0; i < imgData.size(); ) {
		uint8_t code;
		*this->content >> u8(code);
		switch (code) {
			case 0xFD: // Skip the given number of pixels
				*this->content >> u8(code);
				i += code;
				// Note: i may now be >= dataSize
				break;

			case 0xFE: // Go to the next line
				i = ++y * dims.x;
				break;

			case 0xFF: // End of image
				i = imgData.size();
				break;

			case 0x00: // shouldn't happen
				throw stream::error("corrupted data");

			default:
				if (i + code <= imgData.size()) {
					this->content->read(&imgData[i], code);
					i += code;
				} else {
					throw stream::error("bad data, tried to write past end of image");
				}
				break;
		}
	}
	return imgData;
}

Pixels Image_Zone66Tile::convert_mask() const
{
	// Return an entirely opaque mask
	auto dims = this->dimensions();
	return Pixels(dims.x * dims.y, 0x00);
}

void Image_Zone66Tile::convert(const Pixels& newContent, const Pixels& newMask)
{
//	assert((this->width != 0) && (this->height != 0));
	this->content->seekp(0, stream::start);

	auto dims = this->dimensions();

	// Full screen images are in a different format, so should never end up here.
	// The tls-zone66 handler should create an Image_VGARaw for those tiles
	// instead.
	assert((dims.x != 320) && (dims.y != 200));

	// Start off with enough space for the worst-case size
	this->content->truncate(4 + (dims.x + 2) * dims.y + 1);
	int finalSize = 4; // for header

	this->content->seekp(Z66_IMG_OFFSET, stream::start);
	auto imgData = newContent.data();

	// Find the last non-black pixel in the image
	auto imgEnd = imgData + dims.x * dims.y - 1;
	while ((*imgEnd == 0) && (imgEnd > imgData)) imgEnd--;
	imgEnd++; // still want current (non-black) pixel

	for (unsigned int y = 0; y < dims.y; y++) {
		int dw = dims.x;
		while (dw > 0) {
			// Count how many black pixels are in a row starting at the current pos
			int amt = 0;
			while ((*imgData == 0) && (amt < 254) && (dw > 0)) {
				imgData++;
				amt++;
				dw--;
			}
			// If there were black pixels, figure out the best way to write them
			if (amt) {
				if (dw == 0) {
					// Got blanks until EOL
					// TESTED BY: img_zone66_tile_from_standard_8x4
					break; // go to next line (will write 0xFE or 0xFF)
				} else {
					// Encountered at least one blank pixel
					if (amt > 1) {
						// More efficient to write as RLE
						// TESTED BY: img_zone66_tile_from_standard_8x4
						*this->content << u8(0xFD) << u8(amt);
						finalSize += 2;
						// If there were enough blanks, keep looking for more.
						// TESTED BY: TODO
						if (amt == 255) continue;
					} else {
						// Less/same efficiency to write code, so don't
						// TESTED BY: img_zone66_tile_from_standard_8x4
						imgData -= amt; // reprocess data in next step
						dw += amt;
					}
				}
			}
			if (dw == 0) break;
			// This next pixel is not blank (or it is but there's only 1-2 of them)
			// so write out as straight pixel data.  If this assertion fails then
			// the code above is broken because three blanks in a row should've been
			// handled up there.
			assert(
				(imgData[0] != 0) ||
				(imgData[1] != 0) ||
				(imgData[2] != 0)
			);
			amt = (dw > 255) ? 255 : dw;
			for (int l = 1; l < amt; l++) { // assume [0] is not blank - if it is [1] or [2] won't be
				if (imgData[l] == 0) {
					// found first empty
					int lenBlank = 1;
					for (int ll = l + 1; ll < amt; ll++) {
						if (imgData[ll] == 0) lenBlank++;
						else break; // TESTED BY: img_zone66_tile_from_standard_8x4
					}
					if (lenBlank > 2) {
						// More than two blanks start at pos l, so only write up to pos l
						// as normal pixels (then the loop will run again and the blanks
						// will get picked up by the condition above.)
						// TESTED BY: img_zone66_tile_from_standard_8x4
						amt = l;
						break;
					}
				}
			}
			*this->content << u8(amt);
			this->content->write(imgData, amt);
			imgData += amt;
			dw -= amt;
			finalSize += amt + 1;
		}

		// TESTED BY: img_zone66_tile_from_standard_8x5
		// TESTED BY: img_zone66_tile_from_standard_8x7
		if (imgData >= imgEnd) break; // just write EOF

		assert(dw == 0); // make sure we read everything
		*this->content << u8(0xFE); // end of line
		finalSize++;
	}
	*this->content << u8(0xFF); // end of file
	finalSize++;

	this->content->flush();

	// Then shrink back to actual size
	this->content->truncate(finalSize);

	return;
}

} // namespace gamegraphics
} // namespace camoto
