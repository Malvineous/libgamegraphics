/**
 * @file  tls-sagent.cpp
 * @brief Secret Agent encryption wrapper around Crystal Caves tileset handler
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Crystal_Caves_Tileset_Format
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
#include "tls-sagent.hpp"

/// How much padding to apply after each SAM 8x8 tileset (in bytes)
#define SAM_PAD_TILES8 (2048 - 3 - 2000)

/// How much padding to apply after each SAM 16x16 tileset (in bytes)
#define SAM_PAD_TILES16 (8064 - 3 - 8000)

namespace camoto {
namespace gamegraphics {

//
// TilesetType_SAgent2k
//

TilesetType_SAgent2k::TilesetType_SAgent2k()
{
	this->pad = SAM_PAD_TILES8;
}

TilesetType_SAgent2k::~TilesetType_SAgent2k()
{
}

std::string TilesetType_SAgent2k::code() const
{
	return "tls-sagent-2k";
}

std::string TilesetType_SAgent2k::friendlyName() const
{
	return "Secret Agent 2K Concatenated Tileset";
}

std::vector<std::string> TilesetType_SAgent2k::fileExtensions() const
{
	return {"gfx"};
}

std::vector<std::string> TilesetType_SAgent2k::games() const
{
	return {
		"Secret Agent",
	};
}


//
// TilesetType_SAgent8k
//

TilesetType_SAgent8k::TilesetType_SAgent8k()
{
	this->pad = SAM_PAD_TILES16;
}

TilesetType_SAgent8k::~TilesetType_SAgent8k()
{
}

std::string TilesetType_SAgent8k::code() const
{
	return "tls-sagent-8k";
}

std::string TilesetType_SAgent8k::friendlyName() const
{
	return "Secret Agent 8K Concatenated Tileset";
}

std::vector<std::string> TilesetType_SAgent8k::fileExtensions() const
{
	return {
		"gfx",
		"da1",
	};
}

std::vector<std::string> TilesetType_SAgent8k::games() const
{
	return {
		"Secret Agent",
		"Dark Ages",
	};
}

} // namespace gamegraphics
} // namespace camoto
