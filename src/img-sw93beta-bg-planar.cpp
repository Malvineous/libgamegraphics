/**
 * @file  img-sw93beta-bg-planar.hpp
 * @brief Shadow Warrior 1993 Beta planar background image.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Shadow_Warrior_(1993_Beta)_Background_Image_Format
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
#include "img-sw93beta-bg-planar.hpp"
#include "pal-vga-raw.hpp"

/// Depth of the palette file
#define SWBP_PALETTE_DEPTH 6

#define SWBGP_WIDTH 428
#define SWBGP_HEIGHT 75

namespace camoto {
namespace gamegraphics {

/// Planar VGA Image implementation for Shadow Warrior Beta.
class Image_SW93Beta_BG_Planar: virtual public Image
{
	public:
		/// Constructor
		/**
		 * @param content
		 *   Image data for planes 1-4
		 *
		 * @param pal
		 *   Image palette
		 */
		Image_SW93Beta_BG_Planar(std::unique_ptr<stream::inout> content[4],
			std::shared_ptr<const Palette> pal);
		virtual ~Image_SW93Beta_BG_Planar();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual Point dimensions() const;
		virtual Pixels convert() const;
		virtual Pixels convert_mask() const;
		virtual void convert(const Pixels& newContent, const Pixels& newMask);

	protected:
		std::unique_ptr<stream::inout> content[4]; ///< Image content
};


std::string ImageType_SW93Beta_BG_Planar::code() const
{
	return "img-sw93beta-bg-planar";
}

std::string ImageType_SW93Beta_BG_Planar::friendlyName() const
{
	return "Shadow Warrior 1993 Beta Background Planar image";
}

std::vector<std::string> ImageType_SW93Beta_BG_Planar::fileExtensions() const
{
	return {"130"}; // .DAT format code, exposed as extension by libgamearchive
}

std::vector<std::string> ImageType_SW93Beta_BG_Planar::games() const
{
	return {
		"Shadow Warrior 1993 Beta",
	};
}

ImageType::Certainty ImageType_SW93Beta_BG_Planar::isInstance(
	stream::input& content) const
{
	stream::len len = content.size();

	if (len != 32100) return Certainty::DefinitelyNo;

	return Certainty::Unsure;
}

std::unique_ptr<Image> ImageType_SW93Beta_BG_Planar::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	if (suppData.find(SuppItem::Extra1) == suppData.end()) {
		throw error("Missing supplementary file for plane #2 data.");
	}
	if (suppData.find(SuppItem::Extra2) == suppData.end()) {
		throw error("Missing supplementary file for plane #3 data.");
	}
	if (suppData.find(SuppItem::Extra3) == suppData.end()) {
		throw error("Missing supplementary file for plane #4 data.");
	}
	content->truncate(32100);
	suppData[SuppItem::Extra1]->truncate(32100);
	suppData[SuppItem::Extra2]->truncate(32100);
	suppData[SuppItem::Extra3]->truncate(32100);
	return this->open(std::move(content), suppData);
}

std::unique_ptr<Image> ImageType_SW93Beta_BG_Planar::open(
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
	if (suppData.find(SuppItem::Extra1) == suppData.end()) {
		throw error("Missing supplementary file for plane #2 data.");
	}
	if (suppData.find(SuppItem::Extra2) == suppData.end()) {
		throw error("Missing supplementary file for plane #3 data.");
	}
	if (suppData.find(SuppItem::Extra3) == suppData.end()) {
		throw error("Missing supplementary file for plane #4 data.");
	}
	std::unique_ptr<stream::inout> c[4];
	c[0] = std::move(content);
	c[1] = std::move(suppData[SuppItem::Extra1]);
	c[2] = std::move(suppData[SuppItem::Extra2]);
	c[3] = std::move(suppData[SuppItem::Extra3]);
	return std::make_unique<Image_SW93Beta_BG_Planar>(c, pal);
}

SuppFilenames ImageType_SW93Beta_BG_Planar::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	SuppFilenames supps;
	std::string baseName = filename.substr(0, filename.length() - 3);
	supps[SuppItem::Palette] = "Palette1.134";
	supps[SuppItem::Extra1] = baseName + "131";
	supps[SuppItem::Extra2] = baseName + "132";
	supps[SuppItem::Extra3] = baseName + "133";
	return supps;
}


Image_SW93Beta_BG_Planar::Image_SW93Beta_BG_Planar(
	std::unique_ptr<stream::inout> content[4],
	std::shared_ptr<const Palette> pal)
{
	this->content[0] = std::move(content[0]);
	this->content[1] = std::move(content[1]);
	this->content[2] = std::move(content[2]);
	this->content[3] = std::move(content[3]);
	this->pal = pal;
}

Image_SW93Beta_BG_Planar::~Image_SW93Beta_BG_Planar()
{
}

Image::Caps Image_SW93Beta_BG_Planar::caps() const
{
	return Caps::HasPalette;
}

ColourDepth Image_SW93Beta_BG_Planar::colourDepth() const
{
	return ColourDepth::VGA;
}

Point Image_SW93Beta_BG_Planar::dimensions() const
{
	return Point{SWBGP_WIDTH * 4, SWBGP_HEIGHT};
}

Pixels Image_SW93Beta_BG_Planar::convert() const
{
	Pixels src, dst;
	dst.resize(SWBGP_WIDTH * SWBGP_HEIGHT * 4, 0);

	for (unsigned int p = 0; p < 4; p++) {
		this->content[p]->seekg(0, stream::start);

		for (unsigned int y = 0; y < SWBGP_WIDTH * 4 * SWBGP_HEIGHT; y += SWBGP_WIDTH * 4) {
			for (unsigned long x = p; x < SWBGP_WIDTH * 4; x += 4) {
				*this->content[p] >> u8(dst[y + x]);
			}
		}
	}

	return dst;
}

Pixels Image_SW93Beta_BG_Planar::convert_mask() const
{
	int dataSize = SWBGP_WIDTH * SWBGP_HEIGHT * 4;

	// Return an entirely opaque mask
	Pixels pix;
	pix.resize(dataSize, 0);
	return pix;
}

void Image_SW93Beta_BG_Planar::convert(const Pixels& newContent,
	const Pixels& newMask)
{
	for (unsigned int p = 0; p < 4; p++) {
		this->content[p]->truncate(32100);
		this->content[p]->seekp(0, stream::start);

		for (unsigned int y = 0; y < SWBGP_WIDTH * 4 * SWBGP_HEIGHT; y += SWBGP_WIDTH * 4) {
			for (unsigned long x = p; x < SWBGP_WIDTH * 4; x += 4) {
				*this->content[p] << u8(newContent[y + x]);
			}
		}
	}
	return;
}

} // namespace gamegraphics
} // namespace camoto
