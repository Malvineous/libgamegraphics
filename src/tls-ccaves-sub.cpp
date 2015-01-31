/**
 * @file  tls-ccaves-sub.cpp
 * @brief Crystal Caves sub tileset handler (containing images)
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
#include "img-ega-byteplanar.hpp"
#include "tls-ccaves-sub.hpp"

namespace camoto {
namespace gamegraphics {

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
// TilesetType_CCavesSub
//

TilesetType_CCavesSub::TilesetType_CCavesSub()
{
}

TilesetType_CCavesSub::~TilesetType_CCavesSub()
{
}

std::string TilesetType_CCavesSub::getCode() const
{
	return "tls-ccaves-sub";
}

std::string TilesetType_CCavesSub::getFriendlyName() const
{
	return "Crystal Caves Tileset";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> TilesetType_CCavesSub::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("gfx");
	return vcExtensions;
}

std::vector<std::string> TilesetType_CCavesSub::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Crystal Caves");
	//vcGames.push_back("Secret Agent");
	return vcGames;
}

TilesetType_CCavesSub::Certainty TilesetType_CCavesSub::isInstance(stream::input_sptr psGraphics) const
{
	stream::pos len = psGraphics->size();

	// TESTED BY: tls_ccaves_sub_isinstance_c02
	if (len < 3) return DefinitelyNo; // too short

	psGraphics->seekg(0, stream::start);
	uint8_t numTiles, width, height;
	psGraphics
		>> u8(numTiles)
		>> u8(width)
		>> u8(height)
	;

	// TESTED BY: tls_ccaves_sub_isinstance_c01
	if ((unsigned)(3 + width * height * NUMPLANES_SPRITE * numTiles) != len)
		return DefinitelyNo;

	// TESTED BY: tls_ccaves_sub_isinstance_c00
	return PossiblyYes;
}

TilesetPtr TilesetType_CCavesSub::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	psGraphics->seekp(0, stream::start);
	// Zero tiles, 0x0
	psGraphics->write("\x00\x00\x00", 3);
	return TilesetPtr(new Tileset_CCavesSub(psGraphics, NUMPLANES_SPRITE));
}

TilesetPtr TilesetType_CCavesSub::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	return TilesetPtr(new Tileset_CCavesSub(psGraphics, NUMPLANES_SPRITE));
}

SuppFilenames TilesetType_CCavesSub::getRequiredSupps(const std::string& filenameGraphics) const
{
	// No supplemental types/empty list
	return SuppFilenames();
}


//
// Tileset_CCavesSub
//

Tileset_CCavesSub::Tileset_CCavesSub(stream::inout_sptr data,
	uint8_t numPlanes)
	:	Tileset_FAT(data, CC_FIRST_TILE_OFFSET),
		numPlanes(numPlanes)
{
	stream::pos len = this->data->size();

	// We still have to perform sanity checks in case the user forced an
	// open even though it failed the signature check.
	if (len < 3) throw stream::error("file too short");

	this->data->seekg(0, stream::start);
	uint8_t numImages;
	this->data
		>> u8(numImages)
		>> u8(this->width)
		>> u8(this->height)
	;

	int tileSize = this->width * this->height * this->numPlanes;
	this->items.reserve(numImages);
	for (int i = 0; i < numImages; i++) {
		FATEntry *fat = new FATEntry();
		EntryPtr ep(fat);
		fat->valid = true;
		fat->attr = 0;
		fat->index = i;
		fat->offset = 3 + i * tileSize;
		fat->size = tileSize;
		fat->lenHeader = 0;
		this->items.push_back(ep);
	}

}

Tileset_CCavesSub::~Tileset_CCavesSub()
{
}

int Tileset_CCavesSub::getCaps()
{
	return Tileset::ChangeDimensions | Tileset::ColourDepthEGA;
}

void Tileset_CCavesSub::resize(EntryPtr& id, stream::len newSize)
{
	if (newSize != (unsigned)(this->width * this->height * this->numPlanes)) {
		throw stream::error("tiles in this tileset are a fixed size");
	}
	return;
}

void Tileset_CCavesSub::getTilesetDimensions(unsigned int *width, unsigned int *height)
{
	*width = this->width * 8;
	*height = this->height;
	return;
}

void Tileset_CCavesSub::setTilesetDimensions(unsigned int width, unsigned int height)
{
	// TODO: confirm this, it could just leave the unused bits blank
	if (width % 8) throw stream::error("width must be a multiple of 8");
	this->width = width / 8;
	this->height = height;

	// Update the header
	this->data->seekp(1, stream::start);
	this->data
		<< u8(this->width)
		<< u8(this->height)
	;

	// Resize our containing stream to fit the new dimensions
	this->data->truncate(3 + this->items.size() * this->numPlanes *
		this->width * this->height);

	return;
}

unsigned int Tileset_CCavesSub::getLayoutWidth()
{
	return 10;
}

ImagePtr Tileset_CCavesSub::createImageInstance(const EntryPtr& id,
	stream::inout_sptr content)
{
	PLANE_LAYOUT planes;
	planes[PLANE_BLUE] = 2;
	planes[PLANE_GREEN] = 3;
	planes[PLANE_RED] = 4;
	planes[PLANE_INTENSITY] = 5;
	planes[PLANE_HITMAP] = 0;
	planes[PLANE_OPACITY] = -1;

	Image_EGABytePlanar *ega = new Image_EGABytePlanar();
	ImagePtr conv(ega);
	ega->setParams(
		content, 0, this->width * 8, this->height, planes, PaletteTablePtr()
	);

	return conv;
}

Tileset_CCavesSub::FATEntry *Tileset_CCavesSub::preInsertFile(
	const Tileset_CCavesSub::FATEntry *idBeforeThis, Tileset_CCavesSub::FATEntry *pNewEntry)
{
	if (this->items.size() >= 255) {
		throw stream::error("maximum number of tiles reached");
	}

	// All tiles are a fixed size in this format.
	pNewEntry->size = this->width * this->height * this->numPlanes;

	// Update the header
	this->data->seekp(0, stream::start);
	this->data << u8(this->items.size() + 1);

	return pNewEntry;
}

void Tileset_CCavesSub::postRemoveFile(const FATEntry *pid)
{
	// Update the header
	this->data->seekp(0, stream::start);
	this->data << u8(this->items.size());
	return;
}


} // namespace gamegraphics
} // namespace camoto
