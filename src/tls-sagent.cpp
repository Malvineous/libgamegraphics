/**
 * @file   tls-sagent.cpp
 * @brief  Secret Agent encryption wrapper around Crystal Caves tileset handler
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Crystal_Caves_Tileset_Format
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
#include "tls-sagent.hpp"

/// How much padding to apply after each SAM 8x8 tileset (in bytes)
#define SAM_PAD_TILES8 (2048 - 3 - 2000)

/// How much padding to apply after each SAM 16x16 tileset (in bytes)
#define SAM_PAD_TILES16 (8064 - 3 - 8000)

namespace camoto {
namespace gamegraphics {

//
// SAgent2kTilesetType
//

SAgent2kTilesetType::SAgent2kTilesetType()
{
	this->pad = SAM_PAD_TILES8;
}

SAgent2kTilesetType::~SAgent2kTilesetType()
{
}

std::string SAgent2kTilesetType::getCode() const
{
	return "tls-sagent-2k";
}

std::string SAgent2kTilesetType::getFriendlyName() const
{
	return "Secret Agent 2K Concatenated Tileset";
}

std::vector<std::string> SAgent2kTilesetType::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("gfx");
	return vcExtensions;
}

std::vector<std::string> SAgent2kTilesetType::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Secret Agent");
	return vcGames;
}

SAgent2kTilesetType::Certainty SAgent2kTilesetType::isInstance(
	stream::input_sptr psGraphics) const
{
	return this->CCavesMainTilesetType::isInstance(psGraphics);
}

TilesetPtr SAgent2kTilesetType::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	return this->CCavesMainTilesetType::create(psGraphics, suppData);
}

// Preconditions: isInstance() has returned > EC_DEFINITELY_NO
TilesetPtr SAgent2kTilesetType::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	return this->CCavesMainTilesetType::open(psGraphics, suppData);
}

//
// SAgent8kTilesetType
//

SAgent8kTilesetType::SAgent8kTilesetType()
{
	this->pad = SAM_PAD_TILES16;
}

SAgent8kTilesetType::~SAgent8kTilesetType()
{
}

std::string SAgent8kTilesetType::getCode() const
{
	return "tls-sagent-8k";
}

std::string SAgent8kTilesetType::getFriendlyName() const
{
	return "Secret Agent 8K Concatenated Tileset";
}

std::vector<std::string> SAgent8kTilesetType::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("gfx");
	return vcExtensions;
}

std::vector<std::string> SAgent8kTilesetType::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Secret Agent");
	return vcGames;
}

SAgent8kTilesetType::Certainty SAgent8kTilesetType::isInstance(
	stream::input_sptr psGraphics) const
{
	return this->CCavesMainTilesetType::isInstance(psGraphics);
}

TilesetPtr SAgent8kTilesetType::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	return this->CCavesMainTilesetType::create(psGraphics, suppData);
}

// Preconditions: isInstance() has returned > EC_DEFINITELY_NO
TilesetPtr SAgent8kTilesetType::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	return this->CCavesMainTilesetType::open(psGraphics, suppData);
}

} // namespace gamegraphics
} // namespace camoto
