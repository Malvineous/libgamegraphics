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

TilesetType_Catacomb::TilesetType_Catacomb()
{
}

TilesetType_Catacomb::~TilesetType_Catacomb()
{
}

std::vector<std::string> TilesetType_Catacomb::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("cat");
	vcExtensions.push_back("ca2");
	return vcExtensions;
}

std::vector<std::string> TilesetType_Catacomb::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Catacomb");
	vcGames.push_back("Catacomb II");
	return vcGames;
}

SuppFilenames TilesetType_Catacomb::getRequiredSupps(const std::string& filenameGraphics) const
{
	// No supplemental types/empty list
	return SuppFilenames();
}


//
// TilesetType_CatacombEGA
//

TilesetType_CatacombEGA::TilesetType_CatacombEGA()
{
}

TilesetType_CatacombEGA::~TilesetType_CatacombEGA()
{
}

std::string TilesetType_CatacombEGA::getCode() const
{
	return "tls-catacomb-ega";
}

std::string TilesetType_CatacombEGA::getFriendlyName() const
{
	return "Catacomb EGA Tileset";
}

TilesetType_CatacombEGA::Certainty TilesetType_CatacombEGA::isInstance(stream::input_sptr psGraphics) const
{
	stream::pos len = psGraphics->size();

	// Catacomb I
	if (len == CATI_NUMTILES * CAT_EGA_TILESIZE) return PossiblyYes;

	// Catacomb II
	if (len == CATII_NUMTILES * CAT_EGA_TILESIZE) return PossiblyYes;

	return DefinitelyNo;
}

TilesetPtr TilesetType_CatacombEGA::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	psGraphics->seekp(0, stream::start);
	// Zero tiles, 0x0
	return TilesetPtr(new Tileset_Catacomb(psGraphics, CAT_EGA));
}

TilesetPtr TilesetType_CatacombEGA::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	return TilesetPtr(new Tileset_Catacomb(psGraphics, CAT_EGA));
}


//
// TilesetType_CatacombCGA
//

TilesetType_CatacombCGA::TilesetType_CatacombCGA()
{
}

TilesetType_CatacombCGA::~TilesetType_CatacombCGA()
{
}

std::string TilesetType_CatacombCGA::getCode() const
{
	return "tls-catacomb-cga";
}

std::string TilesetType_CatacombCGA::getFriendlyName() const
{
	return "Catacomb CGA Tileset";
}

TilesetType_CatacombCGA::Certainty TilesetType_CatacombCGA::isInstance(stream::input_sptr psGraphics) const
{
	stream::pos len = psGraphics->size();

	// Catacomb I
	if (len == CATI_NUMTILES * CAT_CGA_TILESIZE) return PossiblyYes;

	// Catacomb II
	if (len == CATII_NUMTILES * CAT_CGA_TILESIZE) return PossiblyYes;

	return DefinitelyNo;
}

TilesetPtr TilesetType_CatacombCGA::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	psGraphics->seekp(0, stream::start);
	// Zero tiles, 0x0
	return TilesetPtr(new Tileset_Catacomb(psGraphics, CAT_CGA));
}

TilesetPtr TilesetType_CatacombCGA::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	return TilesetPtr(new Tileset_Catacomb(psGraphics, CAT_CGA));
}


//
// Tileset_Catacomb
//

Tileset_Catacomb::Tileset_Catacomb(stream::inout_sptr data,
	ImageType_Catacomb imageType)
	:	Tileset_FAT(data, CAT_FIRST_TILE_OFFSET),
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

Tileset_Catacomb::~Tileset_Catacomb()
{
}

int Tileset_Catacomb::getCaps()
{
	return 0;
}

void Tileset_Catacomb::resize(EntryPtr& id, stream::len newSize)
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

void Tileset_Catacomb::getTilesetDimensions(unsigned int *width, unsigned int *height)
{
	*width = CAT_TILE_WIDTH;
	*height = CAT_TILE_HEIGHT;
	return;
}

unsigned int Tileset_Catacomb::getLayoutWidth()
{
	return 2;
}

ImagePtr Tileset_Catacomb::createImageInstance(const EntryPtr& id,
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

			Image_EGAPlanar *ega = new Image_EGAPlanar();
			ega->setParams(
				content, 0, CAT_TILE_WIDTH, CAT_TILE_HEIGHT, planes
			);

			conv.reset(ega);
			break;
		}
		case CAT_CGA: {
			Image_CGA *cga = new Image_CGA(content, 0, CAT_TILE_WIDTH, CAT_TILE_HEIGHT,
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
