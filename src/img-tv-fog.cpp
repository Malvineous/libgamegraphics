/**
 * @file   img-tv-fog.cpp
 * @brief  Terminal Velocity fog mapping table.
 *
 * Copyright (C) 2010-2011 Adam Nielsen <malvineous@shikadi.net>
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

#include <boost/iostreams/stream.hpp>
#include "img-tv-fog.hpp"
#include "pal-vga-raw.hpp"
#include "img-vga-raw.hpp"

/// Width of the resulting image
#define TV_FOG_WIDTH 256

/// Height of the resulting image
#define TV_FOG_HEIGHT 16

namespace camoto {
namespace gamegraphics {

namespace io = boost::iostreams;

TVFogImageType::TVFogImageType()
	throw ()
{
}

TVFogImageType::~TVFogImageType()
	throw ()
{
}

std::string TVFogImageType::getCode() const
	throw ()
{
	return "img-tv-fog";
}

std::string TVFogImageType::getFriendlyName() const
	throw ()
{
	return "Terminal Velocity fog map";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> TVFogImageType::getFileExtensions() const
	throw ()
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("fog");
	return vcExtensions;
}

std::vector<std::string> TVFogImageType::getGameList() const
	throw ()
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Terminal Velocity");
	return vcGames;
}

ImageType::Certainty TVFogImageType::isInstance(stream::input_sptr psImage) const
	throw (stream::error)
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

ImagePtr TVFogImageType::create(stream::inout_sptr psImage,
	SuppData& suppData) const
	throw (stream::error)
{
	psImage->truncate(TV_FOG_WIDTH * TV_FOG_HEIGHT);
	uint8_t buf;
	for (int i = 0; i < TV_FOG_WIDTH * TV_FOG_HEIGHT; i++) {
		buf = i % 256;
		psImage->write((char *)&buf, 1);
	}

	ImagePtr palFile(new VGAPalette(suppData[SuppItem::Palette]));
	PaletteTablePtr pal = palFile->getPalette();
	return ImagePtr(new VGARawImage(psImage, TV_FOG_WIDTH, TV_FOG_HEIGHT, pal));
}

ImagePtr TVFogImageType::open(stream::inout_sptr psImage,
	SuppData& suppData) const
	throw (stream::error)
{
	ImagePtr palFile(new VGAPalette(suppData[SuppItem::Palette]));
	PaletteTablePtr pal = palFile->getPalette();
	return ImagePtr(new VGARawImage(psImage, TV_FOG_WIDTH, TV_FOG_HEIGHT, pal));
}

SuppFilenames TVFogImageType::getRequiredSupps(const std::string& filenameImage) const
	throw ()
{
	SuppFilenames supps;
	std::string filenameBase =
		filenameImage.substr(0, filenameImage.find_last_of('.'));
	supps[SuppItem::Palette] = filenameBase + ".act";
	return supps;
}

} // namespace gamegraphics
} // namespace camoto
