/**
 * @file   tls-ccaves.cpp
 * @brief  Crystal Caves tileset handler.
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
#include "img-ega-byteplanar.hpp"
#include "tls-ccaves.hpp"

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
// CCavesTilesetType
//

CCavesTilesetType::CCavesTilesetType()
	throw ()
{
}

CCavesTilesetType::~CCavesTilesetType()
	throw ()
{
}

std::string CCavesTilesetType::getCode() const
	throw ()
{
	return "tls-ccaves";
}

std::string CCavesTilesetType::getFriendlyName() const
	throw ()
{
	return "Crystal Caves Tileset";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> CCavesTilesetType::getFileExtensions() const
	throw ()
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("gfx");
	return vcExtensions;
}

std::vector<std::string> CCavesTilesetType::getGameList() const
	throw ()
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Crystal Caves");
	//vcGames.push_back("Secret Agent");
	return vcGames;
}

CCavesTilesetType::Certainty CCavesTilesetType::isInstance(iostream_sptr psGraphics) const
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

TilesetPtr CCavesTilesetType::create(iostream_sptr psGraphics,
	FN_TRUNCATE fnTruncate, MP_SUPPDATA& suppData) const
	throw (std::ios::failure)
{
	throw std::ios::failure("not implemented yet");
	psGraphics->seekp(0, std::ios::beg);
	// Zero tiles, 1 byte wide (8 pixels), 8 rows/pixels high
	psGraphics->write("\x00\x01\x08", 3);
	return TilesetPtr(new CCavesTileset(psGraphics, fnTruncate, NUMPLANES_SPRITE));
}

// Preconditions: isInstance() has returned > EC_DEFINITELY_NO
TilesetPtr CCavesTilesetType::open(iostream_sptr psGraphics,
	FN_TRUNCATE fnTruncate, MP_SUPPDATA& suppData) const
	throw (std::ios::failure)
{
	return TilesetPtr(new CCavesTileset(psGraphics, fnTruncate, NUMPLANES_SPRITE));
}

MP_SUPPLIST CCavesTilesetType::getRequiredSupps(const std::string& filenameGraphics) const
	throw ()
{
	// No supplemental types/empty list
	return MP_SUPPLIST();
}


//
// CCavesTileset
//

CCavesTileset::CCavesTileset(iostream_sptr data, FN_TRUNCATE fnTruncate,
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

CCavesTileset::~CCavesTileset()
	throw ()
{
}

int CCavesTileset::getCaps()
	throw ()
{
	return 0;
}

TilesetPtr CCavesTileset::createTilesetInstance(const EntryPtr& id,
	iostream_sptr content, FN_TRUNCATE fnTruncate)
	throw (std::ios::failure)
{
	assert(content->good());

	return TilesetPtr(
		new CCavesTiles(content, fnTruncate, this->numPlanes)
	);
}



//
// CCavesTiles
//

CCavesTiles::CCavesTiles(iostream_sptr data, FN_TRUNCATE fnTruncate,
	uint8_t numPlanes)
	throw (std::ios::failure) :
		FATTileset(data, fnTruncate, CC_FIRST_TILE_OFFSET),
		numPlanes(numPlanes)
{
	assert(this->data->good());

	this->data->seekg(0, std::ios::end);
	io::stream_offset len = this->data->tellg();

	// We still have to perform sanity checks in case the user forced an
	// open even though it failed the signature check.
	if (len < 3) throw std::ios::failure("file too short");

	this->data->seekg(0, std::ios::beg);
	uint8_t numImages;
	this->data
		>> u8(numImages)
		>> u8(this->width)
		>> u8(this->height)
	;

	if (!this->data->good()) throw std::ios::failure("unable to read header");

	int tileSize = this->width * this->height * this->numPlanes;
	this->items.reserve(numImages);
	for (int i = 0; i < numImages; i++) {
		FATEntry *fat = new FATEntry();
		EntryPtr ep(fat);
		fat->isValid = true;
		fat->attr = None;
		fat->index = i;
		fat->offset = 3 + i * tileSize;
		fat->size = tileSize;
		fat->lenHeader = 0;
		this->items.push_back(ep);
	}

}

CCavesTiles::~CCavesTiles()
	throw ()
{
}

int CCavesTiles::getCaps()
	throw ()
{
	return ChangeDimensions;
}

void CCavesTiles::resize(EntryPtr& id, size_t newSize)
	throw (std::ios::failure)
{
	if (newSize != this->width * this->height * this->numPlanes) {
		throw std::ios::failure("tiles in this tileset are a fixed size");
	}
	return;
}

void CCavesTiles::getTilesetDimensions(unsigned int *width, unsigned int *height)
	throw ()
{
	*width = this->width * 8;
	*height = this->height;
	return;
}

void CCavesTiles::setTilesetDimensions(unsigned int width, unsigned int height)
	throw (std::ios::failure)
{
	// TODO: confirm this, it could just leave the unused bits blank
	if (width % 8) throw std::ios::failure("width must be a multiple of 8");
	this->width = width / 8;
	this->height = height;

	// Update the header
	this->data->seekp(1);
	this->data
		<< u8(this->width)
		<< u8(this->height)
	;

	// Resize our containing stream to fit the new dimensions
	this->fnTruncate(3 + this->items.size() * this->numPlanes *
		this->width * this->height);

	return;
}

unsigned int CCavesTiles::getLayoutWidth()
	throw ()
{
	return 10;
}

ImagePtr CCavesTiles::createImageInstance(const EntryPtr& id,
	iostream_sptr content, FN_TRUNCATE fnTruncate)
	throw (std::ios::failure)
{
	PLANE_LAYOUT planes;
	planes[PLANE_BLUE] = 2;
	planes[PLANE_GREEN] = 3;
	planes[PLANE_RED] = 4;
	planes[PLANE_INTENSITY] = 5;
	planes[PLANE_HITMAP] = 0;
	planes[PLANE_OPACITY] = 1;

	ImagePtr conv(new EGABytePlanarImage(
		content, fnTruncate, this->width * 8, this->height, planes
	));

	return conv;
}

CCavesTiles::FATEntry *CCavesTiles::preInsertFile(
	const CCavesTiles::FATEntry *idBeforeThis, CCavesTiles::FATEntry *pNewEntry)
	throw (std::ios::failure)
{
	if (this->items.size() >= 255) {
		throw std::ios::failure("maximum number of tiles reached");
	}

	// All tiles are a fixed size in this format.
	pNewEntry->size = this->width * this->height * this->numPlanes;

	// Update the header
	this->data->seekp(0);
	this->data << u8(this->items.size() + 1);

	return pNewEntry;
}

void CCavesTiles::postRemoveFile(const FATEntry *pid)
	throw (std::ios::failure)
{
	// Update the header
	this->data->seekp(0);
	this->data << u8(this->items.size());
	return;
}


} // namespace gamegraphics
} // namespace camoto
