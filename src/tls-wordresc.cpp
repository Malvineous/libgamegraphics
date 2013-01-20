/**
 * @file   tls-wordresc.cpp
 * @brief  Word Rescue tileset
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Word_Rescue
 *
 * Copyright (C) 2010-2013 Adam Nielsen <malvineous@shikadi.net>
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
#include "tls-img.hpp"
#include "img-pcx.hpp"

namespace camoto {
namespace gamegraphics {

#define WR_TILE_WIDTH  16 ///< Width of each tile
#define WR_TILE_HEIGHT 16 ///< Height of each tile

#define WR_TILES_X (320 / WR_TILE_WIDTH)  ///< Number of tiles in a row
#define WR_TILES_Y (200 / WR_TILE_HEIGHT) ///< Number of tile rows

WordrescTilesetType::WordrescTilesetType()
{
}

WordrescTilesetType::~WordrescTilesetType()
{
}

std::string WordrescTilesetType::getCode() const
{
	return "tls-wordresc";
}

std::string WordrescTilesetType::getFriendlyName() const
{
	return "Word Rescue Tileset";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> WordrescTilesetType::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("wr");
	return vcExtensions;
}

std::vector<std::string> WordrescTilesetType::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Word Rescue");
	return vcGames;
}

WordrescTilesetType::Certainty WordrescTilesetType::isInstance(stream::input_sptr psGraphics) const
{
	PCX_PlanarEGA_ImageType pcx;
	if (!pcx.isInstance(psGraphics)) return DefinitelyNo;

/// @todo Implement this when TilesetType::open() can take a read-only stream
/*
	SuppData sd;
	ImagePtr img = pcx.open(psGraphics, sd);

	unsigned int width, height;
	img->getDimensions(&width, &height);
	if ((width != 320) || (height != 200)) return DefinitelyNo;
*/
	return PossiblyYes; // best we can hope for
}

TilesetPtr WordrescTilesetType::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	// TODO
	throw stream::error("Not yet implemented");
}

TilesetPtr WordrescTilesetType::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	PCX_PlanarEGA_ImageType pcx;
	ImagePtr img = pcx.open(psGraphics, suppData);//(new PCXImage(psGraphics));
	return TilesetPtr(new TilesetFromImage(img,
		WR_TILE_WIDTH, WR_TILE_HEIGHT, WR_TILES_X, WR_TILES_Y));
}

SuppFilenames WordrescTilesetType::getRequiredSupps(const std::string& filenameGraphics) const
{
	// No supplemental types/empty list
	return SuppFilenames();
}

} // namespace gamegraphics
} // namespace camoto
