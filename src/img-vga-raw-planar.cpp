/**
 * @file  img-vga-raw-planar.cpp
 * @brief Image_VGA specialisation for planar fixed-size headerless images.
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

#include <camoto/util.hpp> // make_unique
#include "pal-vga-raw.hpp"
#include "img-vga-raw-planar.hpp"

namespace camoto {
namespace gamegraphics {

std::vector<std::string> ImageType_VGA_Planar_RawBase::fileExtensions() const
{
	return {};
}

std::vector<std::string> ImageType_VGA_Planar_RawBase::games() const
{
	return {};
}

ImageType::Certainty ImageType_VGA_Planar_RawBase::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();

	// TESTED BY: TODO
	if (len == 64000) return Certainty::PossiblyYes;

	// TESTED BY: TODO
	return Certainty::DefinitelyNo;
}

std::unique_ptr<Image> ImageType_VGA_Planar_RawBase::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->truncate(64000);
	char buf[64];
	memset(buf, 0, 64);
	for (int i = 0; i < 1000; i++) content->write(buf, 64);

	return this->open(std::move(content), suppData);
}

std::unique_ptr<Image> ImageType_VGA_Planar_RawBase::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	std::shared_ptr<const Palette> pal;
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		auto palFile = std::make_shared<Palette_VGA>(
			std::move(suppData[SuppItem::Palette]), this->depth
		);
		pal = palFile->palette();
	}
	return std::make_unique<Image_VGA_Planar_Raw>(
		std::move(content), Point{320, 200}, pal
	);
}

SuppFilenames ImageType_VGA_Planar_RawBase::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	SuppFilenames supps;
	std::string filenameBase =
		filename.substr(0, filename.find_last_of('.'));
	supps[SuppItem::Palette] = filenameBase + ".pal";
	return supps;
}


ImageType_VGA_Planar_Raw6::ImageType_VGA_Planar_Raw6()
{
	this->depth = 6;
}

std::string ImageType_VGA_Planar_Raw6::code() const
{
	return "img-vga-planar-fullscreen";
}

std::string ImageType_VGA_Planar_Raw6::friendlyName() const
{
	return "Planar VGA fullscreen image (6-bit palette)";
}


ImageType_VGA_Planar_Raw8::ImageType_VGA_Planar_Raw8()
{
	this->depth = 8;
}

std::string ImageType_VGA_Planar_Raw8::code() const
{
	return "img-vga-planar8-fullscreen";
}

std::string ImageType_VGA_Planar_Raw8::friendlyName() const
{
	return "Planar VGA fullscreen image (24-bit palette)";
}


Image_VGA_Planar_Raw::Image_VGA_Planar_Raw(std::unique_ptr<stream::inout> content,
	Point dims, std::shared_ptr<const Palette> pal)
	:	Image_VGA_Planar(std::move(content), 0),
		dims(dims)
{
	this->pal = pal;
}

Image_VGA_Planar_Raw::~Image_VGA_Planar_Raw()
{
}

Image::Caps Image_VGA_Planar_Raw::caps() const
{
	return Caps::HasPalette;
}

ColourDepth Image_VGA_Planar_Raw::colourDepth() const
{
	return ColourDepth::VGA;
}

Point Image_VGA_Planar_Raw::dimensions() const
{
	return this->dims;
}

void Image_VGA_Planar_Raw::dimensions(const Point& newDimensions)
{
	throw stream::error("this image is a fixed size");
}

} // namespace gamegraphics
} // namespace camoto
