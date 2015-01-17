/**
 * @file   img-scr-vinyl.cpp
 * @brief  Vinyl Goddess From Mars SCR image format.
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
#include "img-scr-vinyl.hpp"
#include "img-vga-raw-planar.hpp"
#include "pal-vga-raw.hpp"

/// Offset where planar VGA data begins
#define SCR_DATA_OFFSET 0

/// Depth of the Vinyl palette file
#define SCR_PALETTE_DEPTH 8

namespace camoto {
namespace gamegraphics {

std::string VinylSCRImageType::getCode() const
{
	return "img-scr-vinyl";
}

std::string VinylSCRImageType::getFriendlyName() const
{
	return "Vinyl Goddess From Mars SCR image";
}

std::vector<std::string> VinylSCRImageType::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("gra");
	return vcExtensions;
}

std::vector<std::string> VinylSCRImageType::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Vinyl Goddess From Mars");
	return vcGames;
}

ImageType::Certainty VinylSCRImageType::isInstance(stream::input_sptr psImage) const
{
	stream::len len = psImage->size();
	if (len != 320*200) return DefinitelyNo;
	return Unsure;
}

ImagePtr VinylSCRImageType::create(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	psImage->truncate(SCR_DATA_OFFSET);
	psImage->seekp(0, stream::start);
	psImage
		<< u16le(0)
		<< u16le(0)
	;

	PaletteTablePtr pal;
	// Only load the palette if one was given
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new VGAPalette(suppData[SuppItem::Palette], SCR_PALETTE_DEPTH));
		pal = palFile->getPalette();
	}
	return ImagePtr(new VGARawPlanarImage(psImage, 320, 200, pal));
}

ImagePtr VinylSCRImageType::open(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	PaletteTablePtr pal;
	// Only load the palette if one was given
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new VGAPalette(suppData[SuppItem::Palette], SCR_PALETTE_DEPTH));
		pal = palFile->getPalette();
	}
	return ImagePtr(new VGARawPlanarImage(psImage, 320, 200, pal));
}

SuppFilenames VinylSCRImageType::getRequiredSupps(const std::string& filenameImage) const
{
	SuppFilenames supps;
	std::string filenameBase =
		filenameImage.substr(0, filenameImage.find_last_of('.'));
	supps[SuppItem::Palette] = filenameBase + ".pal";
	return supps;
}


} // namespace gamegraphics
} // namespace camoto
