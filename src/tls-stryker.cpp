/**
 * @file   tls-stryker.cpp
 * @brief  Major Stryker tileset
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Stryker_Tileset_Format
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

#include <camoto/iostream_helpers.hpp>
#include "img-ega-byteplanar.hpp"
#include "tls-stryker.hpp"
#include "tls-ega-apogee.hpp"

namespace camoto {
namespace gamegraphics {

/// Offset of first tileset in a tileset collection.
#define MS_FIRST_TILE_OFFSET 0

/// Width of each tile, in pixels
#define MS_TILE_WIDTH 16

/// Height of each tile, in pixels
#define MS_TILE_HEIGHT 16

/// Size of a tile with the given number of planes, in bytes
#define MS_TILE_SIZE(p) (MS_TILE_WIDTH / 8 * MS_TILE_HEIGHT * (p))

/// Ideal width of the standard/default tileset, in number of tiles
#define MS_STD_IDEAL_WIDTH 20

/// Size in bytes of standard Stryker tileset (20 tiles by 12 tiles)
#define MS_SIZE_STANDARD ((20 * 12) * MS_TILE_SIZE(EGA_NUMPLANES_SOLID))

/// Size in bytes of Stryker backdrop (16 tiles by 10 tiles)
#define MS_SIZE_BACKDROP ((16 * 10) * MS_TILE_SIZE(EGA_NUMPLANES_SOLID))

/// Size in bytes of masked Stryker tileset (20 tiles by 12 tiles)
#define MS_SIZE_STDMASK ((20 * 12) * MS_TILE_SIZE(EGA_NUMPLANES_MASKED))

//
// StrykerTilesetType
//

StrykerTilesetType::StrykerTilesetType()
{
}

StrykerTilesetType::~StrykerTilesetType()
{
}

std::string StrykerTilesetType::getCode() const
{
	return "tls-stryker";
}

std::string StrykerTilesetType::getFriendlyName() const
{
	return "Major Stryker Tileset";
}

std::vector<std::string> StrykerTilesetType::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("dr1");
	return vcExtensions;
}

std::vector<std::string> StrykerTilesetType::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Major Stryker");
	return vcGames;
}

StrykerTilesetType::Certainty StrykerTilesetType::isInstance(stream::input_sptr psGraphics) const
{
	stream::pos len = psGraphics->size();

	if (len == MS_SIZE_STANDARD) return PossiblyYes;
	if (len == MS_SIZE_BACKDROP) return PossiblyYes;

	return DefinitelyNo;
}

TilesetPtr StrykerTilesetType::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	psGraphics->seekp(0, stream::start);
	// Zero tiles, 0x0, ideal width of standard tileset (20)
	return TilesetPtr(
		new EGAApogeeTileset(psGraphics, MS_TILE_WIDTH, MS_TILE_HEIGHT,
			EGA_NUMPLANES_SOLID, MS_STD_IDEAL_WIDTH, PaletteTablePtr())
	);
}

TilesetPtr StrykerTilesetType::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	stream::pos len = psGraphics->size();

	unsigned int idealWidth = MS_STD_IDEAL_WIDTH;
	if (len == MS_SIZE_BACKDROP) idealWidth = 16;

	return TilesetPtr(
		new EGAApogeeTileset(psGraphics, MS_TILE_WIDTH, MS_TILE_HEIGHT,
			EGA_NUMPLANES_SOLID, idealWidth, PaletteTablePtr())
	);
}

SuppFilenames StrykerTilesetType::getRequiredSupps(const std::string& filenameGraphics) const
{
	// No supplemental types/empty list
	return SuppFilenames();
}


//
// StrykerMaskedTilesetType
//

std::string StrykerMaskedTilesetType::getCode() const
{
	return "tls-stryker-masked";
}

std::string StrykerMaskedTilesetType::getFriendlyName() const
{
	return "Major Stryker Masked Tileset";
}

StrykerMaskedTilesetType::Certainty StrykerMaskedTilesetType::isInstance(stream::input_sptr psGraphics) const
{
	stream::pos len = psGraphics->size();
	if (len == MS_SIZE_STDMASK) return PossiblyYes;
	return DefinitelyNo;
}

TilesetPtr StrykerMaskedTilesetType::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	psGraphics->seekp(0, stream::start);
	// Zero tiles, 0x0
	return TilesetPtr(
		new EGAApogeeTileset(psGraphics, MS_TILE_WIDTH, MS_TILE_HEIGHT,
			EGA_NUMPLANES_MASKED, MS_STD_IDEAL_WIDTH, PaletteTablePtr())
	);
}

TilesetPtr StrykerMaskedTilesetType::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	return TilesetPtr(
		new EGAApogeeTileset(psGraphics, MS_TILE_WIDTH, MS_TILE_HEIGHT,
			EGA_NUMPLANES_MASKED, MS_STD_IDEAL_WIDTH, PaletteTablePtr())
	);
}

} // namespace gamegraphics
} // namespace camoto
