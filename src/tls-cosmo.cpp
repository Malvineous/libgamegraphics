/**
 * @file  tls-cosmo.cpp
 * @brief Cosmo's Cosmic Adventures tileset
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Cosmo_Tileset_Format
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
#include "img-ega-byteplanar.hpp"
#include "tls-cosmo.hpp"
#include "tls-ega-apogee.hpp"

namespace camoto {
namespace gamegraphics {

/// Offset of first tileset in a tileset collection.
#define CCA_FIRST_TILE_OFFSET 0

/// Width of each tile, in pixels
#define CCA_TILE_WIDTH 8

/// Height of each tile, in pixels
#define CCA_TILE_HEIGHT 8

/// Size of a tile with the given number of planes, in bytes
#define CCA_TILE_SIZE(p) (CCA_TILE_WIDTH / 8 * CCA_TILE_HEIGHT * (p))

/// Size in bytes of standard Cosmo tileset (40 tiles by 100 tiles)
#define CCA_SIZE_STANDARD ((40 * 50) * CCA_TILE_SIZE(EGA_NUMPLANES_SOLID))

/// Size in bytes of Cosmo backdrop (40 tiles by 18 tiles)
#define CCA_SIZE_BACKDROP ((40 * 18) * CCA_TILE_SIZE(EGA_NUMPLANES_SOLID))

/// Size in bytes of Duke Nukem II backdrop (40 tiles by 25 tiles)
#define DN2_SIZE_BACKDROP ((40 * 25) * CCA_TILE_SIZE(EGA_NUMPLANES_SOLID))

/// Size in bytes of masked Cosmo tileset (40 tiles by 100 tiles)
#define CCA_SIZE_STDMASK ((40 * 25) * CCA_TILE_SIZE(EGA_NUMPLANES_MASKED))

/// Ideal width of tileset, in number of tiles
#define CCA_IDEAL_WIDTH 40

//
// TilesetType_Cosmo
//

TilesetType_Cosmo::TilesetType_Cosmo()
{
}

TilesetType_Cosmo::~TilesetType_Cosmo()
{
}

std::string TilesetType_Cosmo::getCode() const
{
	return "tls-cosmo";
}

std::string TilesetType_Cosmo::getFriendlyName() const
{
	return "Cosmo's Cosmic Adventures Tileset";
}

std::vector<std::string> TilesetType_Cosmo::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("mni");
	return vcExtensions;
}

std::vector<std::string> TilesetType_Cosmo::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Cosmo's Cosmic Adventures");
	vcGames.push_back("Duke Nukem II");
	return vcGames;
}

TilesetType_Cosmo::Certainty TilesetType_Cosmo::isInstance(stream::input_sptr psGraphics) const
{
	stream::pos len = psGraphics->size();

	// Standard tileset
	if (len == CCA_SIZE_STANDARD) return PossiblyYes;

	// Map backdrop (Cosmo)
	if (len == CCA_SIZE_BACKDROP) return PossiblyYes;

	// Map backdrop (Duke II)
	if (len == DN2_SIZE_BACKDROP) return PossiblyYes;

	return DefinitelyNo;
}

TilesetPtr TilesetType_Cosmo::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	psGraphics->seekp(0, stream::start);
	// Zero tiles, 0x0
	return TilesetPtr(
		new Tileset_EGAApogee(psGraphics, CCA_TILE_WIDTH, CCA_TILE_HEIGHT,
			EGA_NUMPLANES_SOLID, CCA_IDEAL_WIDTH, PaletteTablePtr())
	);
}

TilesetPtr TilesetType_Cosmo::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	return TilesetPtr(
		new Tileset_EGAApogee(psGraphics, CCA_TILE_WIDTH, CCA_TILE_HEIGHT,
			EGA_NUMPLANES_SOLID, CCA_IDEAL_WIDTH, PaletteTablePtr())
	);
}

SuppFilenames TilesetType_Cosmo::getRequiredSupps(const std::string& filenameGraphics) const
{
	// No supplemental types/empty list
	return SuppFilenames();
}


//
// TilesetType_CosmoMasked
//

std::string TilesetType_CosmoMasked::getCode() const
{
	return "tls-cosmo-masked";
}

std::string TilesetType_CosmoMasked::getFriendlyName() const
{
	return "Cosmo's Cosmic Adventures Masked Tileset";
}

TilesetType_CosmoMasked::Certainty TilesetType_CosmoMasked::isInstance(stream::input_sptr psGraphics) const
{
	stream::pos len = psGraphics->size();
	if (len == CCA_SIZE_STDMASK) return PossiblyYes;
	return DefinitelyNo;
}

TilesetPtr TilesetType_CosmoMasked::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	psGraphics->seekp(0, stream::start);
	// Zero tiles, 0x0
	return TilesetPtr(
		new Tileset_EGAApogee(psGraphics, CCA_TILE_WIDTH, CCA_TILE_HEIGHT,
			EGA_NUMPLANES_MASKED, CCA_IDEAL_WIDTH, PaletteTablePtr())
	);
}

TilesetPtr TilesetType_CosmoMasked::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	return TilesetPtr(
		new Tileset_EGAApogee(psGraphics, CCA_TILE_WIDTH, CCA_TILE_HEIGHT,
			EGA_NUMPLANES_MASKED, CCA_IDEAL_WIDTH, PaletteTablePtr())
	);
}

} // namespace gamegraphics
} // namespace camoto
