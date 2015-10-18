/**
 * @file  tls-stryker.cpp
 * @brief Major Stryker tileset
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Stryker_Tileset_Format
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
#define MS_TILE_SIZE(p) (MS_TILE_WIDTH / 8 * MS_TILE_HEIGHT * (unsigned int)(p))

/// Ideal width of the standard/default tileset, in number of tiles
#define MS_STD_IDEAL_WIDTH 20

/// Size in bytes of standard Stryker tileset (20 tiles by 12 tiles)
#define MS_SIZE_STANDARD ((20 * 12) * MS_TILE_SIZE(PlaneCount::Solid))

/// Size in bytes of Stryker backdrop (16 tiles by 10 tiles)
#define MS_SIZE_BACKDROP ((16 * 10) * MS_TILE_SIZE(PlaneCount::Solid))

/// Size in bytes of masked Stryker tileset (20 tiles by 12 tiles)
#define MS_SIZE_STDMASK ((20 * 12) * MS_TILE_SIZE(PlaneCount::Masked))

//
// TilesetType_Stryker
//

TilesetType_Stryker::TilesetType_Stryker()
{
}

TilesetType_Stryker::~TilesetType_Stryker()
{
}

std::string TilesetType_Stryker::code() const
{
	return "tls-stryker";
}

std::string TilesetType_Stryker::friendlyName() const
{
	return "Major Stryker Tileset";
}

std::vector<std::string> TilesetType_Stryker::fileExtensions() const
{
	return {"dr1"};
}

std::vector<std::string> TilesetType_Stryker::games() const
{
	return {
		"Major Stryker",
	};
}

TilesetType::Certainty TilesetType_Stryker::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();

	if (len == MS_SIZE_STANDARD) return PossiblyYes;
	if (len == MS_SIZE_BACKDROP) return PossiblyYes;

	return DefinitelyNo;
}

std::shared_ptr<Tileset> TilesetType_Stryker::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	// Zero tiles, 0x0
	content->truncate(0);
	return this->open(std::move(content), suppData);
}

std::shared_ptr<Tileset> TilesetType_Stryker::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	stream::pos len = content->size();

	return std::make_unique<Tileset_EGAApogee>(
		std::move(content),
		Point{MS_TILE_WIDTH, MS_TILE_HEIGHT},
		PlaneCount::Solid,
		(len == MS_SIZE_BACKDROP) ? 16 : MS_STD_IDEAL_WIDTH,
		nullptr
	);
}

SuppFilenames TilesetType_Stryker::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	// No supplemental types/empty list
	return {};
}


//
// TilesetType_StrykerMasked
//

std::string TilesetType_StrykerMasked::code() const
{
	return "tls-stryker-masked";
}

std::string TilesetType_StrykerMasked::friendlyName() const
{
	return "Major Stryker Masked Tileset";
}

TilesetType_StrykerMasked::Certainty TilesetType_StrykerMasked::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();
	if (len == MS_SIZE_STDMASK) return PossiblyYes;
	return DefinitelyNo;
}

std::shared_ptr<Tileset> TilesetType_StrykerMasked::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	return std::make_unique<Tileset_EGAApogee>(
		std::move(content),
		Point{MS_TILE_WIDTH, MS_TILE_HEIGHT},
		PlaneCount::Masked, MS_STD_IDEAL_WIDTH, nullptr
	);
}

} // namespace gamegraphics
} // namespace camoto
