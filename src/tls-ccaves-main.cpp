/**
 * @file   tls-ccaves-main.cpp
 * @brief  Crystal Caves top-level tileset handler (containing sub tilesets)
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Crystal_Caves_Tileset_Format
 *
 * Copyright (C) 2010 Adam Nielsen <malvineous@shikadi.net>
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

#include <iostream>
#include <exception>
#include <cstring>
#include <boost/shared_array.hpp>
#include <boost/iostreams/positioning.hpp>  // stream_offset
#include <boost/bind.hpp>

#include <camoto/debug.hpp>
#include <camoto/iostream_helpers.hpp>
#include <camoto/byteorder.hpp>
#include <camoto/substream.hpp>
#include "tls-ccaves-main.hpp"
#include "tls-ccaves-sub.hpp"

namespace camoto {
namespace gamegraphics {

namespace io = boost::iostreams;

/// Number of planes in each image
#define NUMPLANES_SPRITE  5
#define NUMPLANES_TILE    4

/// Maximum number of tilesets to load from a tileset collection.
#define CC_SAFETY_MAX_TILESETCOUNT  1024

/// Offset of first tileset in a tileset collection.
#define CC_FIRST_TILESET_OFFSET 0

/// Offset of the first tile within a single tileset.
#define CC_FIRST_TILE_OFFSET    3

//
// CCavesMainTilesetType
//

CCavesMainTilesetType::CCavesMainTilesetType()
	throw ()
{
}

CCavesMainTilesetType::~CCavesMainTilesetType()
	throw ()
{
}

std::string CCavesMainTilesetType::getCode() const
	throw ()
{
	return "tls-ccaves-main";
}

std::string CCavesMainTilesetType::getFriendlyName() const
	throw ()
{
	return "Crystal Caves Concatenated Tileset";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> CCavesMainTilesetType::getFileExtensions() const
	throw ()
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("gfx");
	return vcExtensions;
}

std::vector<std::string> CCavesMainTilesetType::getGameList() const
	throw ()
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Crystal Caves");
	//vcGames.push_back("Secret Agent");
	return vcGames;
}

CCavesMainTilesetType::Certainty CCavesMainTilesetType::isInstance(iostream_sptr psGraphics) const
	throw (std::ios::failure)
{
	psGraphics->seekg(0, std::ios::end);
	io::stream_offset len = psGraphics->tellg();

	// TESTED BY: TODO //fmt_grp_duke3d_isinstance_c02
	if (len < 3) return DefinitelyNo; // too short

	psGraphics->seekg(0, std::ios::beg);
	io::stream_offset pos = 0;
	while (pos < len) {
		uint8_t numTiles, width, height;
		psGraphics
			>> u8(numTiles)
			>> u8(width)
			>> u8(height)
		;
		int delta = width*height*NUMPLANES_SPRITE*numTiles;
/*
		// Make sure we don't get stuck
		if (delta == 0) {
			if ((numTiles == 0) && (len == 3)) {
				// This matches a valid empty file
				return EC_POSSIBLY_YES;
			}
			return EC_DEFINITELY_NO;
		}
*/
		// If this pushes us past EOF it's not a valid file
		pos += delta + 3;
		if (pos > len) return DefinitelyNo;

		psGraphics->seekg(delta, std::ios::cur);
	}

	// TESTED BY: TODO //fmt_grp_duke3d_isinstance_c01
	return PossiblyYes;
}

TilesetPtr CCavesMainTilesetType::create(iostream_sptr psGraphics,
	FN_TRUNCATE fnTruncate, MP_SUPPDATA& suppData) const
	throw (std::ios::failure)
{
	throw std::ios::failure("not implemented yet");
	psGraphics->seekp(0, std::ios::beg);
	// Zero tiles, 1 byte wide (8 pixels), 8 rows/pixels high
	psGraphics->write("\x00\x01\x08", 3);
	return TilesetPtr(new CCavesMainTileset(psGraphics, fnTruncate, NUMPLANES_SPRITE));
}

// Preconditions: isInstance() has returned > EC_DEFINITELY_NO
TilesetPtr CCavesMainTilesetType::open(iostream_sptr psGraphics,
	FN_TRUNCATE fnTruncate, MP_SUPPDATA& suppData) const
	throw (std::ios::failure)
{
	return TilesetPtr(new CCavesMainTileset(psGraphics, fnTruncate, NUMPLANES_SPRITE));
}

MP_SUPPLIST CCavesMainTilesetType::getRequiredSupps(const std::string& filenameGraphics) const
	throw ()
{
	// No supplemental types/empty list
	return MP_SUPPLIST();
}


//
// CCavesMainTileset
//

CCavesMainTileset::CCavesMainTileset(iostream_sptr data, FN_TRUNCATE fnTruncate,
	unsigned int numPlanes)
	throw (std::ios::failure) :
		FATTileset(data, fnTruncate, CC_FIRST_TILESET_OFFSET),
		numPlanes(numPlanes)
{
	this->data->seekg(0, std::ios::end);
	io::stream_offset len = this->data->tellg();

	// We still have to perform sanity checks in case the user forced an
	// open even though it failed the signature check.
	if (len < 3) throw std::ios::failure("file too short");

	this->data->seekg(0, std::ios::beg);

	io::stream_offset pos = 0;
	for (int i = 0; pos < len; i++) {
		uint8_t numTiles, width, height;
		this->data
			>> u8(numTiles)
			>> u8(width)
			>> u8(height)
		;
		FATEntry *fat = new FATEntry();
		EntryPtr ep(fat);
		fat->isValid = true;
		fat->attr = SubTileset;
		fat->index = i;
		fat->offset = pos;
		fat->size = width*height*this->numPlanes*numTiles+3;
		fat->lenHeader = 0;

		// Make sure this tileset won't go past EOF or is zero data
		if (pos + fat->size > len) break;
		this->items.push_back(ep);

		this->data->seekg(fat->size-3, std::ios::cur);
		pos += fat->size;

		if (i >= CC_SAFETY_MAX_TILESETCOUNT) {
			throw std::ios::failure("too many tilesets or corrupted graphics file");
		}
	}
}

CCavesMainTileset::~CCavesMainTileset()
	throw ()
{
}

int CCavesMainTileset::getCaps()
	throw ()
{
	return 0;
}

TilesetPtr CCavesMainTileset::createTilesetInstance(const EntryPtr& id,
	iostream_sptr content, FN_TRUNCATE fnTruncate)
	throw (std::ios::failure)
{
	assert(content->good());

	return TilesetPtr(
		new CCavesSubTileset(content, fnTruncate, this->numPlanes)
	);
}


} // namespace gamegraphics
} // namespace camoto