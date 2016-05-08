/**
 * @file  img-mono.cpp
 * @brief Image implementation adding support for monochrome graphics.
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
#include <camoto/util.hpp> // make_unique
#include "img-mono.hpp"
#include "img-ega-planar.hpp"

namespace camoto {
namespace gamegraphics {

ImageType_Mono::ImageType_Mono()
{
}

ImageType_Mono::~ImageType_Mono()
{
}

std::string ImageType_Mono::code() const
{
	return "img-mono-raw-fullscreen";
}

std::string ImageType_Mono::friendlyName() const
{
	return "Raw monochrome fullscreen image";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> ImageType_Mono::fileExtensions() const
{
	return {};
}

std::vector<std::string> ImageType_Mono::games() const
{
	return {};
}

ImageType::Certainty ImageType_Mono::isInstance(stream::input& content) const
{
	stream::pos len = content.size();

	// TESTED BY: TODO
	if (len == 8000) return Certainty::PossiblyYes;

	// TESTED BY: TODO
	return Certainty::DefinitelyNo;
}

std::unique_ptr<Image> ImageType_Mono::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->truncate(8000);
	content->seekp(0, stream::start);
	char buf[64];
	memset(buf, 0, 64);
	for (int i = 0; i < 125; i++) content->write(buf, 64);

	return this->open(std::move(content), suppData);
}

std::unique_ptr<Image> ImageType_Mono::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	return std::make_unique<Image_EGA_Planar>(
		std::move(content), 0,
		Point{320, 200},
		EGAPlaneLayout{
			EGAPlanePurpose::Intensity1,
			EGAPlanePurpose::Unused,
			EGAPlanePurpose::Unused,
			EGAPlanePurpose::Unused,
			EGAPlanePurpose::Unused,
			EGAPlanePurpose::Unused
		},
		nullptr
	);
}

SuppFilenames ImageType_Mono::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	return {};
}

} // namespace gamegraphics
} // namespace camoto
