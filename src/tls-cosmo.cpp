/**
 * @file   tls-cosmo.cpp
 * @brief  Cosmo's Cosmic Adventures tileset
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Cosmo_Tileset_Format
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
#include <camoto/stream_sub.hpp>
#include "img-ega-byteplanar.hpp"
#include "tls-cosmo.hpp"

namespace camoto {
namespace gamegraphics {

namespace io = boost::iostreams;

/// Number of planes in each image
#define NUMPLANES_SPRITE  5
#define NUMPLANES_TILE    4

/// Offset of first tileset in a tileset collection.
#define CCA_FIRST_TILE_OFFSET 0

#define CCA_TILE_WIDTH 8
#define CCA_TILE_HEIGHT 8

//
// CosmoTilesetType
//

CosmoTilesetType::CosmoTilesetType()
	throw ()
{
}

CosmoTilesetType::~CosmoTilesetType()
	throw ()
{
}

std::string CosmoTilesetType::getCode() const
	throw ()
{
	return "tls-cosmo";
}

std::string CosmoTilesetType::getFriendlyName() const
	throw ()
{
	return "Cosmo's Cosmic Adventures Tileset";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> CosmoTilesetType::getFileExtensions() const
	throw ()
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("mni");
	return vcExtensions;
}

std::vector<std::string> CosmoTilesetType::getGameList() const
	throw ()
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Cosmo's Cosmic Adventures");
	vcGames.push_back("Duke Nukem II");
	return vcGames;
}

CosmoTilesetType::Certainty CosmoTilesetType::isInstance(stream::input_sptr psGraphics) const
	throw (stream::error)
{
	stream::pos len = psGraphics->size();
	if (len == 2000 * 32) return PossiblyYes;
	return DefinitelyNo;
}

TilesetPtr CosmoTilesetType::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
	throw (stream::error)
{
	psGraphics->seekp(0, stream::start);
	// Zero tiles, 0x0
	return TilesetPtr(new CosmoTileset(psGraphics, NUMPLANES_TILE));
}

TilesetPtr CosmoTilesetType::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
	throw (stream::error)
{
	return TilesetPtr(new CosmoTileset(psGraphics, NUMPLANES_TILE));
}

SuppFilenames CosmoTilesetType::getRequiredSupps(const std::string& filenameGraphics) const
	throw ()
{
	// No supplemental types/empty list
	return SuppFilenames();
}


//
// CosmoMaskedTilesetType
//

std::string CosmoMaskedTilesetType::getCode() const
	throw ()
{
	return "tls-cosmo-masked";
}

std::string CosmoMaskedTilesetType::getFriendlyName() const
	throw ()
{
	return "Cosmo's Cosmic Adventures Masked Tileset";
}

CosmoMaskedTilesetType::Certainty CosmoMaskedTilesetType::isInstance(stream::input_sptr psGraphics) const
	throw (stream::error)
{
	stream::pos len = psGraphics->size();
	if (len == 1000 * 40) return PossiblyYes;
	return DefinitelyNo;
}

TilesetPtr CosmoMaskedTilesetType::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
	throw (stream::error)
{
	psGraphics->seekp(0, stream::start);
	// Zero tiles, 0x0
	return TilesetPtr(new CosmoTileset(psGraphics, NUMPLANES_SPRITE));
}

TilesetPtr CosmoMaskedTilesetType::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
	throw (stream::error)
{
	return TilesetPtr(new CosmoTileset(psGraphics, NUMPLANES_SPRITE));
}


//
// CosmoTileset
//

CosmoTileset::CosmoTileset(stream::inout_sptr data,
	uint8_t numPlanes)
	throw (stream::error) :
		FATTileset(data, CCA_FIRST_TILE_OFFSET),
		numPlanes(numPlanes)
{
	int tileSize = this->numPlanes << 3; // multiply by eight (bytes per plane)

	stream::pos len = this->data->size();

	this->data->seekg(0, stream::start);
	int numImages = len / tileSize;

	this->items.reserve(numImages);
	for (int i = 0; i < numImages; i++) {
		FATEntry *fat = new FATEntry();
		EntryPtr ep(fat);
		fat->isValid = true;
		fat->attr = None;
		fat->index = i;
		fat->offset = i * tileSize;
		fat->size = tileSize;
		fat->lenHeader = 0;
		this->items.push_back(ep);
	}

}

CosmoTileset::~CosmoTileset()
	throw ()
{
}

int CosmoTileset::getCaps()
	throw ()
{
	return 0;
}

void CosmoTileset::resize(EntryPtr& id, size_t newSize)
	throw (stream::error)
{
	if (newSize != CCA_TILE_WIDTH / 8 * CCA_TILE_HEIGHT * this->numPlanes) {
		throw stream::error("tiles in this tileset are a fixed size");
	}
	return;
}

void CosmoTileset::getTilesetDimensions(unsigned int *width, unsigned int *height)
	throw ()
{
	*width = CCA_TILE_WIDTH;
	*height = CCA_TILE_HEIGHT;
	return;
}

unsigned int CosmoTileset::getLayoutWidth()
	throw ()
{
	return 40;
}

ImagePtr CosmoTileset::createImageInstance(const EntryPtr& id,
	stream::inout_sptr content)
	throw (stream::error)
{
	PLANE_LAYOUT planes;
	int offset = (this->numPlanes == NUMPLANES_TILE) ? 1 : 0;
	planes[PLANE_BLUE] = 2 - offset;
	planes[PLANE_GREEN] = 3 - offset;
	planes[PLANE_RED] = 4 - offset;
	planes[PLANE_INTENSITY] = 5 - offset;
	planes[PLANE_HITMAP] = 0;
	planes[PLANE_OPACITY] = 1 - offset;

	EGABytePlanarImage *ega = new EGABytePlanarImage();
	ImagePtr conv(ega);
	ega->setParams(
		content, 0, CCA_TILE_WIDTH, CCA_TILE_HEIGHT, planes
	);

	return conv;
}

} // namespace gamegraphics
} // namespace camoto
