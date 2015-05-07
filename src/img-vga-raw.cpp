/**
 * @file  img-vga-raw.cpp
 * @brief Image_VGA specialisation for fixed-size headerless images.
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
#include "img-vga-raw.hpp"

namespace camoto {
namespace gamegraphics {

std::vector<std::string> ImageType_VGARawBase::fileExtensions() const
{
	return {"pal"};
}

std::vector<std::string> ImageType_VGARawBase::games() const
{
	std::vector<std::string> vcGames;
	return vcGames;
}

ImageType::Certainty ImageType_VGARawBase::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();

	// TESTED BY: TODO
	if (len == 64000) return PossiblyYes;

	// TESTED BY: TODO
	return DefinitelyNo;
}

std::unique_ptr<Image> ImageType_VGARawBase::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->truncate(64000);
	char buf[64];
	memset(buf, 0, 64);
	for (int i = 0; i < 1000; i++) content->write(buf, 64);

	return this->open(std::move(content), suppData);
}

std::unique_ptr<Image> ImageType_VGARawBase::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	std::shared_ptr<const Palette> pal;
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		auto palFile = std::make_unique<Palette_VGA>(
			std::move(suppData[SuppItem::Palette]), this->depth);
		pal = palFile->palette();
	}
	return std::make_unique<Image_VGARaw>(
		std::move(content), Point{320, 200},
		std::dynamic_pointer_cast<const Palette>(pal));
}

SuppFilenames ImageType_VGARawBase::getRequiredSupps(
	const std::string& filenameImage) const
{
	SuppFilenames supps;
	std::string filenameBase =
		filenameImage.substr(0, filenameImage.find_last_of('.'));
	supps[SuppItem::Palette] = filenameBase + ".pal";
	return supps;
}


ImageType_VGA6Raw::ImageType_VGA6Raw()
{
	this->depth = 6;
}

std::string ImageType_VGA6Raw::code() const
{
	return "img-vga-raw-fullscreen";
}

std::string ImageType_VGA6Raw::friendlyName() const
{
	return "Raw VGA fullscreen image (6-bit palette)";
}


ImageType_VGA8Raw::ImageType_VGA8Raw()
{
	this->depth = 8;
}

std::string ImageType_VGA8Raw::code() const
{
	return "img-vga-raw8-fullscreen";
}

std::string ImageType_VGA8Raw::friendlyName() const
{
	return "Raw VGA fullscreen image (24-bit palette)";
}


Image_VGARaw::Image_VGARaw(std::unique_ptr<stream::inout> content, Point dims,
	std::shared_ptr<const Palette> pal)
	:	Image_VGA(std::move(content), 0),
		dims(dims)
{
	this->pal = pal;
}

Image_VGARaw::~Image_VGARaw()
{
}

Image::Caps Image_VGARaw::caps() const
{
	return Caps::HasPalette;
}

Point Image_VGARaw::dimensions() const
{
	return this->dims;
}

void Image_VGARaw::dimensions(const Point& newDimensions)
{
	throw stream::error("this image is a fixed size");
}

} // namespace gamegraphics
} // namespace camoto
