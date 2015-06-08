/**
 * @file  img-pic-raptor.cpp
 * @brief Raptor PIC image format.
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

#include <camoto/iostream_helpers.hpp>
#include <camoto/util.hpp> // make_unique
#include "img-pic-raptor.hpp"
#include "pal-vga-raw.hpp"

/// Offset where standard VGA data begins
#define PIC_DATA_OFFSET 20

/// Depth of the Raptor palette file
#define PIC_PAL_DEPTH 6

namespace camoto {
namespace gamegraphics {

/// Raptor PIC Image implementation.
class Image_RaptorPIC: virtual public Image_VGA
{
	public:
		/// Constructor
		/**
		 * No truncate function is required as the image dimensions are fixed, so
		 * the file size will always remain constant.
		 *
		 * @param data
		 *   VGA data
		 *
		 * @param pal
		 *   Image palette
		 */
		Image_RaptorPIC(std::unique_ptr<stream::inout> content,
			std::shared_ptr<const Palette> pal);
		virtual ~Image_RaptorPIC();

		virtual Caps caps() const;
		virtual Point dimensions() const;
		virtual void dimensions(const Point& newDimensions);
};


ImageType_RaptorPIC::ImageType_RaptorPIC()
{
}

ImageType_RaptorPIC::~ImageType_RaptorPIC()
{
}

std::string ImageType_RaptorPIC::code() const
{
	return "img-pic-raptor";
}

std::string ImageType_RaptorPIC::friendlyName() const
{
	return "Raptor PIC image";
}

std::vector<std::string> ImageType_RaptorPIC::fileExtensions() const
{
	return {"pic"};
}

std::vector<std::string> ImageType_RaptorPIC::games() const
{
	return {"Raptor"};
}

ImageType::Certainty ImageType_RaptorPIC::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();
	// File too short
	// TESTED BY: img_pic_raptor_isinstance_c01
	if (len < PIC_DATA_OFFSET) return DefinitelyNo;
	uint32_t unknown1, unknown2, unknown3, width, height;
	content.seekg(0, stream::start);
	content
		>> u32le(unknown1)
		>> u32le(unknown2)
		>> u32le(unknown3)
		>> u32le(width)
		>> u32le(height)
	;

	// Image dimensions larger than available data
	// TESTED BY: img_pic_raptor_isinstance_c02
	if (width * height + PIC_DATA_OFFSET != len) return DefinitelyNo;

	// TESTED BY: img_pic_raptor_isinstance_c00
	return DefinitelyYes;
}

std::unique_ptr<Image> ImageType_RaptorPIC::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->truncate(PIC_DATA_OFFSET);
	content->seekp(0, stream::start);
	*content
		<< u32le(1)
		<< u32le(1)
		<< u32le(0)
		<< u32le(0)
		<< u32le(0)
	;

	return this->open(std::move(content), suppData);
}

std::unique_ptr<Image> ImageType_RaptorPIC::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	std::shared_ptr<const Palette> pal;
	// Only load the palette if one was given
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		auto palFile = std::make_unique<Palette_VGA>(
			std::move(suppData[SuppItem::Palette]), PIC_PAL_DEPTH
		);
		pal = palFile->palette();
	}
	return std::make_unique<Image_RaptorPIC>(std::move(content), pal);
}

SuppFilenames ImageType_RaptorPIC::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	SuppFilenames supps;
	supps[SuppItem::Palette] = "palette_dat";
	return supps;
}


Image_RaptorPIC::Image_RaptorPIC(std::unique_ptr<stream::inout> content,
	std::shared_ptr<const Palette> pal)
	:	Image_VGA(std::move(content), PIC_DATA_OFFSET)
{
	this->pal = pal;
}

Image_RaptorPIC::~Image_RaptorPIC()
{
}

Image::Caps Image_RaptorPIC::caps() const
{
	return this->Image_VGA::caps()
		| Caps::SetDimensions
		| (this->pal ? Caps::HasPalette : Caps::Default);
}

Point Image_RaptorPIC::dimensions() const
{
	Point dims;
	this->content->seekg(12, stream::start);
	*this->content
		>> u32le(dims.x)
		>> u32le(dims.y)
	;
	return dims;
}

void Image_RaptorPIC::dimensions(const Point& newDimensions)
{
	this->content->truncate(PIC_DATA_OFFSET + newDimensions.x * newDimensions.y);
	this->content->seekp(12, stream::start);
	*this->content
		<< u32le(newDimensions.x)
		<< u32le(newDimensions.y)
	;
	return;
}

} // namespace gamegraphics
} // namespace camoto
