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

#include "img-tv-fog.hpp"
#include "pal-vga-raw.hpp"
#include "img-vga-raw.hpp"

/// Width of the resulting image
#define TV_FOG_WIDTH 256

/// Height of the resulting image
#define TV_FOG_HEIGHT 16

namespace camoto {
namespace gamegraphics {

ImageType_TVFog::ImageType_TVFog()
{
}

ImageType_TVFog::~ImageType_TVFog()
{
}

std::string ImageType_TVFog::getCode() const
{
	return "img-tv-fog";
}

std::string ImageType_TVFog::getFriendlyName() const
{
	return "Terminal Velocity fog map";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> ImageType_TVFog::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("fog");
	return vcExtensions;
}

std::vector<std::string> ImageType_TVFog::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Terminal Velocity");
	return vcGames;
}

ImageType::Certainty ImageType_TVFog::isInstance(stream::input_sptr psImage) const
{
	stream::pos len = psImage->size();

	// TESTED BY: TODO
	if (len != 4096) return DefinitelyNo;

	uint8_t start[256];
	psImage->seekg(0, stream::start);
	psImage->read((char *)start, 256);
	for (int i = 0; i < 256; i++) {
		// TESTED BY: TODO
		if (start[i] != i) return DefinitelyNo;
	}

	// TESTED BY: TODO
	return DefinitelyYes;
}

ImagePtr ImageType_TVFog::create(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	psImage->truncate(TV_FOG_WIDTH * TV_FOG_HEIGHT);
	uint8_t buf;
	for (int i = 0; i < TV_FOG_WIDTH * TV_FOG_HEIGHT; i++) {
		buf = i % 256;
		psImage->write((char *)&buf, 1);
	}

	ImagePtr palFile(new Palette_VGA(suppData[SuppItem::Palette], 6));
	PaletteTablePtr pal = palFile->getPalette();
	return ImagePtr(new Image_VGARaw(psImage, TV_FOG_WIDTH, TV_FOG_HEIGHT, pal));
}

ImagePtr ImageType_TVFog::open(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	ImagePtr palFile(new Palette_VGA(suppData[SuppItem::Palette], 6));
	PaletteTablePtr pal = palFile->getPalette();
	return ImagePtr(new Image_VGARaw(psImage, TV_FOG_WIDTH, TV_FOG_HEIGHT, pal));
}

SuppFilenames ImageType_TVFog::getRequiredSupps(const std::string& filenameImage) const
{
	SuppFilenames supps;
	std::string filenameBase =
		filenameImage.substr(0, filenameImage.find_last_of('.'));
	supps[SuppItem::Palette] = filenameBase + ".act";
	return supps;
}

} // namespace gamegraphics
} // namespace camoto
