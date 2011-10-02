/**
 * @file   tls-wacky.cpp
 * @brief  Wacky Wheels tileset
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Wacky_Wheels
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

#include <Magick++.h>
#include "tls-wacky.hpp"
#include "tls-img.hpp"
#include "img-pcx.hpp"

namespace camoto {
namespace gamegraphics {

#define WW_TILE_WIDTH  32 ///< Width of each tile
#define WW_TILE_HEIGHT 32 ///< Height of each tile

#define WW_TILES_X (320 / WW_TILE_WIDTH)  ///< Number of tiles in a row
#define WW_TILES_Y (200 / WW_TILE_HEIGHT) ///< Number of tile rows

WackyTilesetType::WackyTilesetType()
	throw ()
{
}

WackyTilesetType::~WackyTilesetType()
	throw ()
{
}

std::string WackyTilesetType::getCode() const
	throw ()
{
	return "tls-wacky";
}

std::string WackyTilesetType::getFriendlyName() const
	throw ()
{
	return "Wacky Wheels Tileset";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> WackyTilesetType::getFileExtensions() const
	throw ()
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("pcx");
	return vcExtensions;
}

std::vector<std::string> WackyTilesetType::getGameList() const
	throw ()
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Wacky Wheels");
	return vcGames;
}

WackyTilesetType::Certainty WackyTilesetType::isInstance(iostream_sptr psGraphics) const
	throw (std::ios::failure)
{
	psGraphics->seekg(0, std::ios::end);
	unsigned long lenData = psGraphics->tellg();
	psGraphics->seekg(0, std::ios::beg);

	boost::shared_ptr<uint8_t> imgData(new uint8_t[lenData]);
	psGraphics->read((char *)imgData.get(), lenData);

	try {
		Magick::Blob blob;
		blob.update(imgData.get(), lenData);
		Magick::Image img(blob);
		if (
			(img.magick().compare("PCX") == 0) &&
			(img.columns() == 320) &&
			(img.rows() == 200) &&
			(img.colorMapSize() == 256)
		) return PossiblyYes;
	} catch (Magick::Exception& e) {
		// We will end up here if ImageMagick doesn't recognise the format at all
		return DefinitelyNo;
	}
	return DefinitelyNo;
}

TilesetPtr WackyTilesetType::create(iostream_sptr psGraphics,
	FN_TRUNCATE fnTruncate, SuppData& suppData) const
	throw (std::ios::failure)
{
	// TODO
	throw std::ios::failure("Not yet implemented");
}

TilesetPtr WackyTilesetType::open(iostream_sptr psGraphics,
	FN_TRUNCATE fnTruncate, SuppData& suppData) const
	throw (std::ios::failure)
{
	ImagePtr img(new PCXImage(psGraphics, fnTruncate));
	return TilesetPtr(new TilesetFromImage(img,
		WW_TILE_WIDTH, WW_TILE_HEIGHT, WW_TILES_X, WW_TILES_Y));
}

SuppFilenames WackyTilesetType::getRequiredSupps(const std::string& filenameGraphics) const
	throw ()
{
	// No supplemental types/empty list
	return SuppFilenames();
}

} // namespace gamegraphics
} // namespace camoto
