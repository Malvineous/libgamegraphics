/**
 * @file  tls-catacomb.cpp
 * @brief Catacomb/Catacomb II CGA and EGA tilesets
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Catacomb_Tileset_Format
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
#include "img-ega-planar.hpp"
#include "img-cga.hpp"
#include "tls-catacomb.hpp"

namespace camoto {
namespace gamegraphics {

/// Offset of first tileset in a tileset collection.
#define CAT_FIRST_TILE_OFFSET 0

#define CAT_TILE_WIDTH 8
#define CAT_TILE_HEIGHT 8

/// Number of bytes per tile in the CGA tileset
#define CAT_CGA_TILESIZE 16

/// Number of bytes per tile in the EGA tileset
#define CAT_EGA_TILESIZE 32

/// Number of planes per tile
#define CAT_NUMPLANES 4

/// Number of tiles in the Catacomb I tileset
#define CATI_NUMTILES 1462

/// Number of tiles in the Catacomb II tileset
#define CATII_NUMTILES 1618

CatacombTilesetType::CatacombTilesetType()
{
}

CatacombTilesetType::~CatacombTilesetType()
{
}

std::vector<std::string> CatacombTilesetType::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("cat");
	vcExtensions.push_back("ca2");
	return vcExtensions;
}

std::vector<std::string> CatacombTilesetType::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Catacomb");
	vcGames.push_back("Catacomb II");
	return vcGames;
}

SuppFilenames CatacombTilesetType::getRequiredSupps(const std::string& filenameGraphics) const
{
	// No supplemental types/empty list
	return SuppFilenames();
}


//
// CatacombEGATilesetType
//

CatacombEGATilesetType::CatacombEGATilesetType()
{
}

CatacombEGATilesetType::~CatacombEGATilesetType()
{
}

std::string CatacombEGATilesetType::getCode() const
{
	return "tls-catacomb-ega";
}

std::string CatacombEGATilesetType::getFriendlyName() const
{
	return "Catacomb EGA Tileset";
}

CatacombEGATilesetType::Certainty CatacombEGATilesetType::isInstance(stream::input_sptr psGraphics) const
{
	stream::pos len = psGraphics->size();

	// Catacomb I
	if (len == CATI_NUMTILES * CAT_EGA_TILESIZE) return PossiblyYes;

	// Catacomb II
	if (len == CATII_NUMTILES * CAT_EGA_TILESIZE) return PossiblyYes;

	return DefinitelyNo;
}

TilesetPtr CatacombEGATilesetType::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	psGraphics->seekp(0, stream::start);
	// Zero tiles, 0x0
	return TilesetPtr(new CatacombTileset(psGraphics, CAT_EGA));
}

TilesetPtr CatacombEGATilesetType::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	return TilesetPtr(new CatacombTileset(psGraphics, CAT_EGA));
}


//
// CatacombCGATilesetType
//

CatacombCGATilesetType::CatacombCGATilesetType()
{
}

CatacombCGATilesetType::~CatacombCGATilesetType()
{
}

std::string CatacombCGATilesetType::getCode() const
{
	return "tls-catacomb-cga";
}

std::string CatacombCGATilesetType::getFriendlyName() const
{
	return "Catacomb CGA Tileset";
}

CatacombCGATilesetType::Certainty CatacombCGATilesetType::isInstance(stream::input_sptr psGraphics) const
{
	stream::pos len = psGraphics->size();

	// Catacomb I
	if (len == CATI_NUMTILES * CAT_CGA_TILESIZE) return PossiblyYes;

	// Catacomb II
	if (len == CATII_NUMTILES * CAT_CGA_TILESIZE) return PossiblyYes;

	return DefinitelyNo;
}

TilesetPtr CatacombCGATilesetType::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	psGraphics->seekp(0, stream::start);
	// Zero tiles, 0x0
	return TilesetPtr(new CatacombTileset(psGraphics, CAT_CGA));
}

TilesetPtr CatacombCGATilesetType::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	return TilesetPtr(new CatacombTileset(psGraphics, CAT_CGA));
}


//
// CatacombTileset
//

CatacombTileset::CatacombTileset(stream::inout_sptr data,
	CatacombImageType imageType)
	:	FATTileset(data, CAT_FIRST_TILE_OFFSET),
		imageType(imageType)
{
	unsigned int tileSize;
	switch (this->imageType) {
		case CAT_EGA: tileSize = CAT_EGA_TILESIZE; break;
		case CAT_CGA: tileSize = CAT_CGA_TILESIZE; break;
	}
	stream::pos len = this->data->size();

	this->data->seekg(0, stream::start);
	int numImages = len / tileSize;

	this->items.reserve(numImages);
	for (int i = 0; i < numImages; i++) {
		FATEntry *fat = new FATEntry();
		EntryPtr ep(fat);
		fat->valid = true;
		fat->attr = 0;
		fat->index = i;
		fat->offset = i * tileSize;
		fat->size = tileSize;
		fat->lenHeader = 0;
		this->items.push_back(ep);
	}

}

CatacombTileset::~CatacombTileset()
{
}

int CatacombTileset::getCaps()
{
	return 0;
}

void CatacombTileset::resize(EntryPtr& id, stream::len newSize)
{
	unsigned int tileSize;
	switch (this->imageType) {
		case CAT_EGA: tileSize = CAT_EGA_TILESIZE; break;
		case CAT_CGA: tileSize = CAT_CGA_TILESIZE; break;
	}
	if (newSize != tileSize) {
		throw stream::error("tiles in this tileset are a fixed size");
	}
	return;
}

void CatacombTileset::getTilesetDimensions(unsigned int *width, unsigned int *height)
{
	*width = CAT_TILE_WIDTH;
	*height = CAT_TILE_HEIGHT;
	return;
}

unsigned int CatacombTileset::getLayoutWidth()
{
	return 2;
}

ImagePtr CatacombTileset::createImageInstance(const EntryPtr& id,
	stream::inout_sptr content)
{
	ImagePtr conv;
	switch (this->imageType) {
		case CAT_EGA: {
			PLANE_LAYOUT planes;
			planes[PLANE_BLUE] = 1;
			planes[PLANE_GREEN] = 2;
			planes[PLANE_RED] = 3;
			planes[PLANE_INTENSITY] = 4;
			planes[PLANE_HITMAP] = 0;
			planes[PLANE_OPACITY] = 0;

			EGAPlanarImage *ega = new EGAPlanarImage();
			ega->setParams(
				content, 0, CAT_TILE_WIDTH, CAT_TILE_HEIGHT, planes
			);

			conv.reset(ega);
			break;
		}
		case CAT_CGA: {
			CGAImage *cga = new CGAImage(content, 0, CAT_TILE_WIDTH, CAT_TILE_HEIGHT,
				CGAPal_CyanMagentaBright);
			conv.reset(cga);
			break;
		}
	}
	assert(conv);
	return conv;
}

} // namespace gamegraphics
} // namespace camoto
