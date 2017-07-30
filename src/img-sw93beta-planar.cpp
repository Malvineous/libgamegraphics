/**
 * @file  img-sw93beta-planar.hpp
 * @brief Shadow Warrior 1993 Beta planar image.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Shadow_Warrior_(1993_Beta)_Image_Format
 *
 * Copyright (C) 2010-2017 Adam Nielsen <malvineous@shikadi.net>
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
#include <camoto/gamegraphics/image.hpp>
#include "img-sw93beta-planar.hpp"
#include "pal-vga-raw.hpp"

/// Depth of the palette file
#define SWBP_PALETTE_DEPTH 6

namespace camoto {
namespace gamegraphics {

/// Planar VGA Image implementation for Shadow Warrior Beta.
class Image_SW93Beta_Planar: virtual public Image
{
	public:
		/// Constructor
		/**
		 * @param content
		 *   Image data
		 *
		 * @param pal
		 *   Image palette
		 */
		Image_SW93Beta_Planar(std::unique_ptr<stream::inout> content,
			std::shared_ptr<const Palette> pal);
		virtual ~Image_SW93Beta_Planar();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual Point dimensions() const;
		virtual void dimensions(const Point& newDimensions);
		virtual Pixels convert() const;
		virtual Pixels convert_mask() const;
		virtual void convert(const Pixels& newContent, const Pixels& newMask);

	protected:
		std::unique_ptr<stream::inout> content; ///< Image content
		Point dims; ///< Image dimensions
};


std::string ImageType_SW93Beta_Planar::code() const
{
	return "img-sw93beta-planar";
}

std::string ImageType_SW93Beta_Planar::friendlyName() const
{
	return "Shadow Warrior 1993 Beta Planar image";
}

std::vector<std::string> ImageType_SW93Beta_Planar::fileExtensions() const
{
	return {"138"}; // .DAT format code, exposed as extension by libgamearchive
}

std::vector<std::string> ImageType_SW93Beta_Planar::games() const
{
	return {
		"Shadow Warrior 1993 Beta",
	};
}

ImageType::Certainty ImageType_SW93Beta_Planar::isInstance(
	stream::input& content) const
{
	stream::len len = content.size();

	// Too short
	// TESTED BY: img_sw93beta_planar_isinstance_c01
	// TESTED BY: img_sw93beta_planar_isinstance_c01a
	if (len < 7) return Certainty::DefinitelyNo;

	content.seekg(0, stream::start);

	uint8_t height;
	uint16_t width;
	content
		>> u8(height)
		>> u16le(width)
	;
	stream::pos offset = 3;
	for (int p = 0; p < 4; p++) {
		// Missing a plane
		// TESTED BY: img_sw93beta_planar_isinstance_c02
		if (offset >= len) return Certainty::DefinitelyNo;

		uint8_t planeWidth;
		content >> u8(planeWidth);
		offset++;
		long planeSize = planeWidth * height;

		offset += planeSize;
		// Incomplete plane
		// TESTED BY: img_sw93beta_planar_isinstance_c03
		if (offset > len) return Certainty::DefinitelyNo;

		content.seekg(planeSize, stream::cur);
	}

	// Extra trailing data
	// TESTED BY: img_sw93beta_planar_isinstance_c04
	if (offset != len) return Certainty::DefinitelyNo;

	return Certainty::DefinitelyYes;
}

std::unique_ptr<Image> ImageType_SW93Beta_Planar::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->truncate(7);
	content->seekp(0, stream::start);
	*content
		<< u8(0)
		<< u16le(0)
		<< u8(0)
		<< u8(0)
		<< u8(0)
		<< u8(0)
	;
	return this->open(std::move(content), suppData);
}

std::unique_ptr<Image> ImageType_SW93Beta_Planar::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	std::shared_ptr<const Palette> pal;
	// Only load the palette if one was given
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		auto palFile = std::make_unique<Palette_VGA>(
			std::move(suppData[SuppItem::Palette]), SWBP_PALETTE_DEPTH
		);
		pal = palFile->palette();
	}
	return std::make_unique<Image_SW93Beta_Planar>(
		std::move(content), pal
	);
}

SuppFilenames ImageType_SW93Beta_Planar::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	SuppFilenames supps;
	supps[SuppItem::Palette] = "Palette1.134";
	return supps;
}


Image_SW93Beta_Planar::Image_SW93Beta_Planar(std::unique_ptr<stream::inout> content,
			std::shared_ptr<const Palette> pal)
	:	content(std::move(content))
{
	this->pal = pal;

	uint8_t height;
	uint16_t width;
	this->content->seekg(0, stream::start);
	*this->content
		>> u8(height)
		>> u16le(width)
	;
	this->dims = Point{width, height};
}

Image_SW93Beta_Planar::~Image_SW93Beta_Planar()
{
}

Image::Caps Image_SW93Beta_Planar::caps() const
{
	return Caps::HasPalette | Caps::SetDimensions;
}

ColourDepth Image_SW93Beta_Planar::colourDepth() const
{
	return ColourDepth::VGA;
}

Point Image_SW93Beta_Planar::dimensions() const
{
	return this->dims;
}

void Image_SW93Beta_Planar::dimensions(const Point& newDimensions)
{
	if (newDimensions.y > 255) {
		throw error("img-sw93beta-planar images must be less than 256 pixels high.");
	}
	this->dims = newDimensions;
	return;
}

Pixels Image_SW93Beta_Planar::convert() const
{
	Pixels src, dst;
	dst.resize(this->dims.x * this->dims.y, 0);

	// Return to start of first plane
	this->content->seekg(3, stream::start);

	for (int p = 0; p < 4; p++) {
		uint8_t planeWidth;
		*this->content >> u8(planeWidth);
		unsigned long planeSize = planeWidth * this->dims.y;
		src.resize(planeSize, 0);

		this->content->read(src.data(), planeSize);

		// Convert the planar data to linear
		for (unsigned int y = 0; y < this->dims.y; y++) {
			for (unsigned int x = 0; x < planeWidth; x++) {
				long d = y * this->dims.x + x * 4 + p;
				if (d > this->dims.x * this->dims.y) break; // corrupt data
				dst[d] = src[y * planeWidth + x];
			}
		}
	}

	return dst;
}

Pixels Image_SW93Beta_Planar::convert_mask() const
{
	int dataSize = this->dims.x * this->dims.y;

	// Return an entirely opaque mask
	Pixels pix;
	pix.resize(dataSize, 0);
	return pix;
}

void Image_SW93Beta_Planar::convert(const Pixels& newContent,
	const Pixels& newMask)
{
	unsigned int plane1Width = (this->dims.x + 3) / 4;
	unsigned int plane2Width = (this->dims.x + 2) / 4;
	unsigned int plane3Width = (this->dims.x + 1) / 4;
	unsigned int plane4Width = this->dims.x / 4;

	stream::len lenContent = 3 + 4
		+ plane1Width * this->dims.y
		+ plane2Width * this->dims.y
		+ plane3Width * this->dims.y
		+ plane4Width * this->dims.y;

	this->content->truncate(lenContent);
	this->content->seekp(0, stream::start);

	*this->content
		<< u8(this->dims.y)
		<< u16le(this->dims.x)
	;
	for (int p = 0; p < 4; p++) {
		int planeWidth = (this->dims.x + (3 - p)) / 4;
		*this->content << u8(planeWidth);
		for (int y = 0; y < dims.y; y++) {
			for (int x = p; x < dims.x; x += 4) {
				*this->content << u8(newContent[y * this->dims.x + x]);
			}
		}
	}
	return;
}

} // namespace gamegraphics
} // namespace camoto
