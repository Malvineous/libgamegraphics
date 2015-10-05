/**
 * @file  img-tv-fog.cpp
 * @brief Terminal Velocity fog mapping table.
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
#include "img-tv-fog.hpp"
#include "pal-vga-raw.hpp"
#include "img-vga-raw.hpp"

/// Width of the resulting image
#define TV_FOG_WIDTH 256

/// Height of the resulting image
#define TV_FOG_HEIGHT 16

/// Depth of the TV palette file
#define FOG_PAL_DEPTH 6

namespace camoto {
namespace gamegraphics {

ImageType_TVFog::ImageType_TVFog()
{
}

ImageType_TVFog::~ImageType_TVFog()
{
}

std::string ImageType_TVFog::code() const
{
	return "img-tv-fog";
}

std::string ImageType_TVFog::friendlyName() const
{
	return "Terminal Velocity fog map";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> ImageType_TVFog::fileExtensions() const
{
	return {"fog"};
}

std::vector<std::string> ImageType_TVFog::games() const
{
	return {
		"Terminal Velocity",
	};
}

ImageType::Certainty ImageType_TVFog::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();

	// Image wrong length
	// TESTED BY: img_tv_fog_isinstance_c01
	if (len != 4096) return DefinitelyNo;

	uint8_t start[256];
	content.seekg(0, stream::start);
	content.read((char *)start, 256);
	for (int i = 0; i < 256; i++) {
		// TESTED BY: img_tv_fog_isinstance_c02
		if (start[i] != i) return DefinitelyNo;
	}

	// TESTED BY: img_tv_fog_isinstance_c00
	return DefinitelyYes;
}

std::unique_ptr<Image> ImageType_TVFog::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->truncate(TV_FOG_WIDTH * TV_FOG_HEIGHT);
	content->seekp(0, stream::start);
	uint8_t buf;
	for (int i = 0; i < TV_FOG_WIDTH * TV_FOG_HEIGHT; i++) {
		buf = i % 256;
		content->write((char *)&buf, 1);
	}
	return this->open(std::move(content), suppData);
}

std::unique_ptr<Image> ImageType_TVFog::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	std::shared_ptr<const Palette> pal;
	// Only load the palette if one was given
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		auto palFile = std::make_unique<Palette_VGA>(
			std::move(suppData[SuppItem::Palette]), FOG_PAL_DEPTH
		);
		pal = palFile->palette();
	}
	return std::make_unique<Image_VGARaw>(
		std::move(content), Point{TV_FOG_WIDTH, TV_FOG_HEIGHT}, pal
	);
}

SuppFilenames ImageType_TVFog::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	SuppFilenames supps;
	std::string filenameBase =
		filename.substr(0, filename.find_last_of('.'));
	supps[SuppItem::Palette] = filenameBase + ".act";
	return supps;
}

} // namespace gamegraphics
} // namespace camoto
