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
#include <camoto/util.hpp> // make_unique
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
#define CCA_TILE_SIZE(p) (CCA_TILE_WIDTH / 8 * CCA_TILE_HEIGHT * (unsigned int)(p))

/// Size in bytes of standard Cosmo tileset (40 tiles by 50 tiles)
#define CCA_SIZE_STANDARD ((40 * 50) * CCA_TILE_SIZE(PlaneCount::Solid))

/// Size in bytes of Cosmo backdrop (40 tiles by 18 tiles)
#define CCA_SIZE_BACKDROP ((40 * 18) * CCA_TILE_SIZE(PlaneCount::Solid))

/// Size in bytes of Duke Nukem II backdrop and CZone solid (40 tiles by 25 tiles)
#define DN2_SIZE_BACKDROP ((40 * 25) * CCA_TILE_SIZE(PlaneCount::Solid))

/// Size in bytes of masked Cosmo tileset (40 tiles by 25 tiles)
#define CCA_SIZE_STDMASK ((40 * 25) * CCA_TILE_SIZE(PlaneCount::Masked))

/// Size in bytes of masked Duke II tileset (40 tiles by 4 tiles)
#define DN2_SIZE_CZONE_MASK ((40 * 4) * CCA_TILE_SIZE(PlaneCount::Masked))

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

std::string TilesetType_Cosmo::code() const
{
	return "tls-cosmo";
}

std::string TilesetType_Cosmo::friendlyName() const
{
	return "Cosmo's Cosmic Adventures Tileset";
}

std::vector<std::string> TilesetType_Cosmo::fileExtensions() const
{
	return {"mni"};
}

std::vector<std::string> TilesetType_Cosmo::games() const
{
	return {
		"Cosmo's Cosmic Adventures",
		"Duke Nukem II"
	};
}

TilesetType_Cosmo::Certainty TilesetType_Cosmo::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();

	// Standard tileset
	if (len == CCA_SIZE_STANDARD) return PossiblyYes;

	// Map backdrop (Cosmo)
	if (len == CCA_SIZE_BACKDROP) return PossiblyYes;

	// Map backdrop / CZone solid (Duke II)
	if (len == DN2_SIZE_BACKDROP) return PossiblyYes;

	return DefinitelyNo;
}

std::shared_ptr<Tileset> TilesetType_Cosmo::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	// Zero tiles, 0x0
	content->truncate(0);
	return this->open(std::move(content), suppData);
}

std::shared_ptr<Tileset> TilesetType_Cosmo::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	return std::make_unique<Tileset_EGAApogee>(
		std::move(content),
		Point{CCA_TILE_WIDTH, CCA_TILE_HEIGHT},
		PlaneCount::Solid, CCA_IDEAL_WIDTH, nullptr
	);
}

SuppFilenames TilesetType_Cosmo::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	return {};
}


//
// TilesetType_CosmoMasked
//

std::string TilesetType_CosmoMasked::code() const
{
	return "tls-cosmo-masked";
}

std::string TilesetType_CosmoMasked::friendlyName() const
{
	return "Cosmo's Cosmic Adventures Masked Tileset";
}

TilesetType_CosmoMasked::Certainty TilesetType_CosmoMasked::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();
	if (len == CCA_SIZE_STDMASK) return PossiblyYes;
	if (len == DN2_SIZE_CZONE_MASK) return PossiblyYes;
	return DefinitelyNo;
}

std::shared_ptr<Tileset> TilesetType_CosmoMasked::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	return std::make_unique<Tileset_EGAApogee>(
		std::move(content),
		Point{CCA_TILE_WIDTH, CCA_TILE_HEIGHT},
		PlaneCount::Masked, CCA_IDEAL_WIDTH, nullptr
	);
}

} // namespace gamegraphics
} // namespace camoto
