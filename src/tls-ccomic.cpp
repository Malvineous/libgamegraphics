/**
 * @file   tls-ccomic.cpp
 * @brief  Captain Comic tileset
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Captain_Comic
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
#include "img-ega-planar.hpp"
#include "tls-ccomic.hpp"

namespace camoto {
namespace gamegraphics {

namespace io = boost::iostreams;

/// Number of planes in each image
#define NUMPLANES_SPRITE  5
#define NUMPLANES_TILES   4

/// Offset of first tileset in a tileset collection.
#define CCA_FIRST_TILE_OFFSET 0

#define CCA_TILE_WIDTH 16
#define CCA_TILE_HEIGHT 16

//
// CComicTilesetType
//

CComicTilesetType::CComicTilesetType()
	throw ()
{
}

CComicTilesetType::~CComicTilesetType()
	throw ()
{
}

std::string CComicTilesetType::getCode() const
	throw ()
{
	return "tls-ccomic";
}

std::string CComicTilesetType::getFriendlyName() const
	throw ()
{
	return "Captain Comic Tileset";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> CComicTilesetType::getFileExtensions() const
	throw ()
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("tt2");
	return vcExtensions;
}

std::vector<std::string> CComicTilesetType::getGameList() const
	throw ()
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Captain Comic");
	return vcGames;
}

CComicTilesetType::Certainty CComicTilesetType::isInstance(iostream_sptr psGraphics) const
	throw (std::ios::failure)
{
	psGraphics->seekg(0, std::ios::end);
	io::stream_offset len = psGraphics->tellg();
	if (len % 128 == 4) return PossiblyYes;
	return DefinitelyNo;
}

TilesetPtr CComicTilesetType::create(iostream_sptr psGraphics,
	FN_TRUNCATE fnTruncate, SuppData& suppData) const
	throw (std::ios::failure)
{
	psGraphics->seekp(0, std::ios::beg);
	// Zero tiles, 0x0
	return TilesetPtr(new CComicTileset(psGraphics, fnTruncate, NUMPLANES_TILES));
}

TilesetPtr CComicTilesetType::open(iostream_sptr psGraphics,
	FN_TRUNCATE fnTruncate, SuppData& suppData) const
	throw (std::ios::failure)
{
	return TilesetPtr(new CComicTileset(psGraphics, fnTruncate, NUMPLANES_TILES));
}

SuppFilenames CComicTilesetType::getRequiredSupps(const std::string& filenameGraphics) const
	throw ()
{
	// No supplemental types/empty list
	return SuppFilenames();
}


//
// CComicSpriteType
//

std::string CComicSpriteType::getCode() const
	throw ()
{
	return "tls-ccomic-sprite";
}

std::string CComicSpriteType::getFriendlyName() const
	throw ()
{
	return "Captain Comic Sprite";
}

CComicSpriteType::Certainty CComicSpriteType::isInstance(iostream_sptr psGraphics) const
	throw (std::ios::failure)
{
	psGraphics->seekg(0, std::ios::end);
	io::stream_offset len = psGraphics->tellg();
	if (len % 160 == 0) return PossiblyYes;
	return DefinitelyNo;
}

TilesetPtr CComicSpriteType::create(iostream_sptr psGraphics,
	FN_TRUNCATE fnTruncate, SuppData& suppData) const
	throw (std::ios::failure)
{
	psGraphics->seekp(0, std::ios::beg);
	// Zero tiles, 0x0
	return TilesetPtr(new CComicTileset(psGraphics, fnTruncate, NUMPLANES_SPRITE));
}

TilesetPtr CComicSpriteType::open(iostream_sptr psGraphics,
	FN_TRUNCATE fnTruncate, SuppData& suppData) const
	throw (std::ios::failure)
{
	return TilesetPtr(new CComicTileset(psGraphics, fnTruncate, NUMPLANES_SPRITE));
}


//
// CComicTileset
//

CComicTileset::CComicTileset(iostream_sptr data, FN_TRUNCATE fnTruncate,
	uint8_t numPlanes)
	throw (std::ios::failure) :
		FATTileset(data, fnTruncate, CCA_FIRST_TILE_OFFSET),
		numPlanes(numPlanes)
{
	assert(this->data->good());

	int tileSize = this->numPlanes << 5; // multiply by 32 (bytes per plane)
	int lenHeader = (this->numPlanes == NUMPLANES_TILES) ? 4 : 0;

	this->data->seekg(0, std::ios::end);
	io::stream_offset len = this->data->tellg();

	this->data->seekg(0, std::ios::beg);
	int numImages = len / tileSize;

	this->items.reserve(numImages);
	for (int i = 0; i < numImages; i++) {
		FATEntry *fat = new FATEntry();
		EntryPtr ep(fat);
		fat->isValid = true;
		fat->attr = None;
		fat->index = i;
		fat->offset = lenHeader + i * tileSize;
		fat->size = tileSize;
		fat->lenHeader = 0;
		this->items.push_back(ep);
	}

}

CComicTileset::~CComicTileset()
	throw ()
{
}

int CComicTileset::getCaps()
	throw ()
{
	return 0;
}

void CComicTileset::resize(EntryPtr& id, size_t newSize)
	throw (std::ios::failure)
{
	if (newSize != CCA_TILE_WIDTH / 8 * CCA_TILE_HEIGHT * this->numPlanes) {
		throw std::ios::failure("tiles in this tileset are a fixed size");
	}
	return;
}

void CComicTileset::getTilesetDimensions(unsigned int *width, unsigned int *height)
	throw ()
{
	*width = CCA_TILE_WIDTH;
	*height = CCA_TILE_HEIGHT;
	return;
}

unsigned int CComicTileset::getLayoutWidth()
	throw ()
{
	return 4;
}

ImagePtr CComicTileset::createImageInstance(const EntryPtr& id,
	iostream_sptr content, FN_TRUNCATE fnTruncate)
	throw (std::ios::failure)
{
	PLANE_LAYOUT planes;
	int offset = (this->numPlanes == NUMPLANES_TILES) ? 1 : 0;

	planes[PLANE_BLUE] = 1;
	planes[PLANE_GREEN] = 2;
	planes[PLANE_RED] = 3;
	planes[PLANE_INTENSITY] = 4;
	planes[PLANE_HITMAP] = 0;
	planes[PLANE_OPACITY] = (this->numPlanes == NUMPLANES_TILES) ? 0 : 5;

	EGAPlanarImage *ega = new EGAPlanarImage();
	ImagePtr conv(ega);
	ega->setParams(
		content, fnTruncate, 0, CCA_TILE_WIDTH, CCA_TILE_HEIGHT, planes
	);

	return conv;
}

} // namespace gamegraphics
} // namespace camoto
