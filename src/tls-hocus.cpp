/**
 * @file   tls-hocus.cpp
 * @brief  Hocus Pocus tileset
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Hocus_Pocus
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
#include "tls-hocus.hpp"
#include "tls-img.hpp"
#include "img-pcx.hpp"

namespace camoto {
namespace gamegraphics {

#define HP_TILE_WIDTH  16 ///< Width of each tile
#define HP_TILE_HEIGHT 16 ///< Height of each tile

#define HP_TILES_X (320 / HP_TILE_WIDTH)  ///< Number of tiles in a row
#define HP_TILES_Y (200 / HP_TILE_HEIGHT) ///< Number of tile rows

HocusTilesetType::HocusTilesetType()
	throw ()
{
}

HocusTilesetType::~HocusTilesetType()
	throw ()
{
}

std::string HocusTilesetType::getCode() const
	throw ()
{
	return "tls-hocus";
}

std::string HocusTilesetType::getFriendlyName() const
	throw ()
{
	return "Hocus Pocus Tileset";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> HocusTilesetType::getFileExtensions() const
	throw ()
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("pcx");
	return vcExtensions;
}

std::vector<std::string> HocusTilesetType::getGameList() const
	throw ()
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Hocus Pocus");
	return vcGames;
}

HocusTilesetType::Certainty HocusTilesetType::isInstance(stream::input_sptr psGraphics) const
	throw (stream::error)
{
	psGraphics->seekg(0, stream::end);
	unsigned long lenData = psGraphics->tellg();
	psGraphics->seekg(0, stream::start);

/// @todo Implement this when TilesetType::open() can take a read-only stream
/*

*/
	return PossiblyYes; // best we can hope for
}

TilesetPtr HocusTilesetType::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
	throw (stream::error)
{
	// TODO
	throw stream::error("Not yet implemented");
}

TilesetPtr HocusTilesetType::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
	throw (stream::error)
{
	ImagePtr img(new PCXImage(psGraphics));
	return TilesetPtr(new TilesetFromImage(img,
		HP_TILE_WIDTH, HP_TILE_HEIGHT, HP_TILES_X, HP_TILES_Y));
}

SuppFilenames HocusTilesetType::getRequiredSupps(const std::string& filenameGraphics) const
	throw ()
{
	// No supplemental types/empty list
	return SuppFilenames();
}

} // namespace gamegraphics
} // namespace camoto
