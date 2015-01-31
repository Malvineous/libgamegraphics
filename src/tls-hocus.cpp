/**
 * @file  tls-hocus.cpp
 * @brief Hocus Pocus tileset
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Hocus_Pocus
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

#include "tls-hocus.hpp"
#include "tls-img.hpp"
#include "img-pcx.hpp"

namespace camoto {
namespace gamegraphics {

#define HP_TILE_WIDTH  16 ///< Width of each tile
#define HP_TILE_HEIGHT 16 ///< Height of each tile

#define HP_TILES_X (320 / HP_TILE_WIDTH)  ///< Number of tiles in a row
#define HP_TILES_Y (200 / HP_TILE_HEIGHT) ///< Number of tile rows

TilesetType_Hocus::TilesetType_Hocus()
{
}

TilesetType_Hocus::~TilesetType_Hocus()
{
}

std::string TilesetType_Hocus::getCode() const
{
	return "tls-hocus";
}

std::string TilesetType_Hocus::getFriendlyName() const
{
	return "Hocus Pocus Tileset";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> TilesetType_Hocus::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("pcx");
	return vcExtensions;
}

std::vector<std::string> TilesetType_Hocus::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Hocus Pocus");
	return vcGames;
}

TilesetType_Hocus::Certainty TilesetType_Hocus::isInstance(stream::input_sptr psGraphics) const
{
	ImageType_PCX_LinearVGA pcx;
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

TilesetPtr TilesetType_Hocus::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	// TODO
	throw stream::error("Not yet implemented");
}

TilesetPtr TilesetType_Hocus::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	ImagePtr img(new Image_PCX(psGraphics, 8, 1));
	return TilesetPtr(new Image_TilesetFrom(img,
		HP_TILE_WIDTH, HP_TILE_HEIGHT, HP_TILES_X, HP_TILES_Y));
}

SuppFilenames TilesetType_Hocus::getRequiredSupps(const std::string& filenameGraphics) const
{
	// No supplemental types/empty list
	return SuppFilenames();
}

} // namespace gamegraphics
} // namespace camoto
