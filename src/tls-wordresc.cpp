/**
 * @file   tls-wordresc.cpp
 * @brief  Word Rescue tileset
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Word_Rescue
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

#include "tls-wordresc.hpp"
#include "tls-pcx.hpp"

namespace camoto {
namespace gamegraphics {

#define WR_TILE_WIDTH  16 ///< Width of each tile
#define WR_TILE_HEIGHT 16 ///< Height of each tile

#define WR_TILES_X (320 / WR_TILE_WIDTH)  ///< Number of tiles in a row
#define WR_TILES_Y (200 / WR_TILE_HEIGHT) ///< Number of tile rows

WordrescTilesetType::WordrescTilesetType()
	throw ()
{
}

WordrescTilesetType::~WordrescTilesetType()
	throw ()
{
}

std::string WordrescTilesetType::getCode() const
	throw ()
{
	return "tls-wordresc";
}

std::string WordrescTilesetType::getFriendlyName() const
	throw ()
{
	return "Word Rescue Tileset";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> WordrescTilesetType::getFileExtensions() const
	throw ()
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("wr");
	return vcExtensions;
}

std::vector<std::string> WordrescTilesetType::getGameList() const
	throw ()
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Word Rescue");
	return vcGames;
}

WordrescTilesetType::Certainty WordrescTilesetType::isInstance(iostream_sptr psGraphics) const
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
			(img.colorMapSize() == 16)
		) return PossiblyYes;
	} catch (Magick::Exception& e) {
		// We will end up here if ImageMagick doesn't recognise the format at all
		return DefinitelyNo;
	}
	return DefinitelyNo;
}

TilesetPtr WordrescTilesetType::create(iostream_sptr psGraphics,
	FN_TRUNCATE fnTruncate, SuppData& suppData) const
	throw (std::ios::failure)
{
	// TODO
	throw std::ios::failure("Not yet implemented");
}

TilesetPtr WordrescTilesetType::open(iostream_sptr psGraphics,
	FN_TRUNCATE fnTruncate, SuppData& suppData) const
	throw (std::ios::failure)
{
	return TilesetPtr(new PCXTileset(psGraphics, fnTruncate,
		WR_TILE_WIDTH, WR_TILE_HEIGHT, WR_TILES_X, WR_TILES_Y));
}

SuppFilenames WordrescTilesetType::getRequiredSupps(const std::string& filenameGraphics) const
	throw ()
{
	// No supplemental types/empty list
	return SuppFilenames();
}

} // namespace gamegraphics
} // namespace camoto
