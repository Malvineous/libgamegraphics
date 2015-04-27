/**
 * @file  img-ccomic.cpp
 * @brief Image specialisation for Captain Comic full-screen images.
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
#include <camoto/stream_filtered.hpp>
#include <camoto/util.hpp> // make_unique
#include "filter-ccomic.hpp"
#include "img-ccomic.hpp"
#include "img-ega-planar.hpp"

/// Width of image, in pixels
#define CCIMG_WIDTH 320

// Height of image, in pixels
#define CCIMG_HEIGHT 200

namespace camoto {
namespace gamegraphics {

ImageType_CComic::ImageType_CComic()
{
}

ImageType_CComic::~ImageType_CComic()
{
}

std::string ImageType_CComic::code() const
{
	return "img-ccomic";
}

std::string ImageType_CComic::friendlyName() const
{
	return "Captain Comic full-screen image";
}

std::vector<std::string> ImageType_CComic::fileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("ega");
	return vcExtensions;
}

std::vector<std::string> ImageType_CComic::games() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Captain Comic");
	return vcGames;
}

ImageType::Certainty ImageType_CComic::isInstance(
	stream::input& content) const
{
	auto lenContent = content.size();

	// Too short
	// TESTED BY: img_ccomic_isinstance_c01
	if (lenContent < 2) return DefinitelyNo;

	content.seekg(0, stream::start);

	uint16_t lenPlane;
	content
		>> u16le(lenPlane)
	;

	// Plane must be correct size
	// TESTED BY: img_ccomic_isinstance_c02
	if (lenPlane != 8000) return DefinitelyNo;

	lenPlane *= 4;  // count all planes
	try {
		while (lenPlane) {
			uint8_t code;
			try {
				content >> u8(code);
			} catch (...) {
				return DefinitelyNo;
			}
			if (code & 0x80) {
				unsigned int repeat = code & 0x7F;
				if (repeat > lenPlane) return DefinitelyNo;
				content.seekg(1, stream::cur);
				lenPlane -= repeat;
			} else {
				if (code > lenPlane) return DefinitelyNo;
				lenPlane -= code;
				content.seekg(code, stream::cur);
			}
		}
	} catch (const stream::error&) {
		// TESTED BY: img_ccomic_isinstance_c03
		return DefinitelyNo;
	}

	// Should be no trailing data
	// TESTED BY: img_ccomic_isinstance_c04
	if (content.tellg() != lenContent) return DefinitelyNo;

	// TESTED BY: img_ccomic_isinstance_c00
	return DefinitelyYes;
}

std::unique_ptr<Image> ImageType_CComic::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	return this->open(std::move(content), suppData);
}

std::unique_ptr<Image> ImageType_CComic::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	auto content_filtered = std::make_unique<stream::filtered>(
		std::move(content),
		std::make_shared<filter_ccomic_unrle>(),
		std::make_shared<filter_ccomic_rle>(),
		nullptr
	);

	EGAPlaneLayout planes = {
		EGAPlanePurpose::Blue1,
		EGAPlanePurpose::Green1,
		EGAPlanePurpose::Red1,
		EGAPlanePurpose::Intensity1,
		EGAPlanePurpose::Unused,
		EGAPlanePurpose::Unused,
	};

	return std::make_unique<Image_EGA_Planar>(
		std::move(content_filtered), 0, Point{CCIMG_WIDTH, CCIMG_HEIGHT}, planes,
		nullptr
	);
}

SuppFilenames ImageType_CComic::getRequiredSupps(
	const std::string& filenameImage) const
{
	return SuppFilenames();
}

} // namespace gamegraphics
} // namespace camoto
