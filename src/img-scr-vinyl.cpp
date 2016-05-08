/**
 * @file  img-scr-vinyl.cpp
 * @brief Vinyl Goddess From Mars SCR image format.
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
#include "img-scr-vinyl.hpp"
#include "img-vga-raw-planar.hpp"
#include "pal-vga-raw.hpp"

/// Offset where planar VGA data begins
#define SCR_DATA_OFFSET 0

/// Depth of the Vinyl palette file
#define SCR_PALETTE_DEPTH 8

namespace camoto {
namespace gamegraphics {

std::string ImageType_VinylSCR::code() const
{
	return "img-scr-vinyl";
}

std::string ImageType_VinylSCR::friendlyName() const
{
	return "Vinyl Goddess From Mars SCR image";
}

std::vector<std::string> ImageType_VinylSCR::fileExtensions() const
{
	return {"scr"};
}

std::vector<std::string> ImageType_VinylSCR::games() const
{
	return {
		"Vinyl Goddess From Mars",
	};
}

ImageType::Certainty ImageType_VinylSCR::isInstance(
	stream::input& content) const
{
	stream::len len = content.size();
	if (len != 320*200) return Certainty::DefinitelyNo;
	return Certainty::Unsure;
}

std::unique_ptr<Image> ImageType_VinylSCR::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->truncate(SCR_DATA_OFFSET);
	content->seekp(0, stream::start);
	*content
		<< u16le(0)
		<< u16le(0)
	;

	return this->open(std::move(content), suppData);
}

std::unique_ptr<Image> ImageType_VinylSCR::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	std::shared_ptr<const Palette> pal;
	// Only load the palette if one was given
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		auto palFile = std::make_unique<Palette_VGA>(
			std::move(suppData[SuppItem::Palette]), SCR_PALETTE_DEPTH
		);
		pal = palFile->palette();
	}
	return std::make_unique<Image_VGA_Planar_Raw>(
		std::move(content), Point{320, 200}, pal
	);
}

SuppFilenames ImageType_VinylSCR::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	SuppFilenames supps;
	std::string filenameBase =
		filename.substr(0, filename.find_last_of('.'));
	supps[SuppItem::Palette] = filenameBase + ".pal";
	return supps;
}


} // namespace gamegraphics
} // namespace camoto
