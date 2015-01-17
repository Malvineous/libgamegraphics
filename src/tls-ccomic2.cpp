/**
 * @file   tls-ccomic2.cpp
 * @brief  Captain Comic II tileset
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Captain_Comic_II
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
#include <camoto/stream_filtered.hpp>
#include "img-ega-planar.hpp"
#include "filter-ccomic2.hpp"
#include "tls-ccomic2.hpp"

namespace camoto {
namespace gamegraphics {

/// Number of planes in each image
#define NUMPLANES_TILES   4

/// Offset of first tileset in a tileset collection.
#define CC2_FIRST_TILE_OFFSET 6

#define CC2_TILE_WIDTH 16
#define CC2_TILE_HEIGHT 16

//
// CComic2TilesetType
//

CComic2TilesetType::CComic2TilesetType()
{
}

CComic2TilesetType::~CComic2TilesetType()
{
}

std::string CComic2TilesetType::getCode() const
{
	return "tls-ccomic2";
}

std::string CComic2TilesetType::getFriendlyName() const
{
	return "Captain Comic II Tileset";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> CComic2TilesetType::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("0");
	return vcExtensions;
}

std::vector<std::string> CComic2TilesetType::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Captain Comic 2");
	return vcGames;
}

CComic2TilesetType::Certainty CComic2TilesetType::isInstance(stream::input_sptr psGraphics) const
{
	stream::pos len = psGraphics->size();
	if (len > 65535) return DefinitelyNo; // file too large (probably)
	psGraphics->seekg(0, stream::start);
	uint16_t num;
	for (int i = 0; i < 3; i++) {
		psGraphics >> u16le(num);
		if (num > 512) return DefinitelyNo; // too many tiles (probably)
	}
	return Unsure;
}

TilesetPtr CComic2TilesetType::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	psGraphics->seekp(0, stream::start);
	psGraphics
		<< u16le(0)
		<< u16le(0)
		<< u16le(0)
	;
	// Zero tiles, 0x0
	return TilesetPtr(new CComic2Tileset(psGraphics, NUMPLANES_TILES));
}

TilesetPtr CComic2TilesetType::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	filter_sptr filtRead(new filter_ccomic2_unrle(CC2_FIRST_TILE_OFFSET));
	filter_sptr filtWrite(new filter_ccomic2_rle(CC2_FIRST_TILE_OFFSET));
	stream::filtered_sptr decoded(new stream::filtered());
	decoded->open(psGraphics, filtRead, filtWrite, NULL);

	return TilesetPtr(new CComic2Tileset(decoded, NUMPLANES_TILES));
}

SuppFilenames CComic2TilesetType::getRequiredSupps(const std::string& filenameGraphics) const
{
	// No supplemental types/empty list
	return SuppFilenames();
}


//
// CComic2Tileset
//

CComic2Tileset::CComic2Tileset(stream::inout_sptr data,
	uint8_t numPlanes)
	:	FATTileset(data, CC2_FIRST_TILE_OFFSET),
		numPlanes(numPlanes)
{
	int tileSize = this->numPlanes << 5; // multiply by 32 (bytes per plane)
	int lenHeader = CC2_FIRST_TILE_OFFSET;

	stream::pos len = this->data->size() - lenHeader;

	int numImages = len / tileSize;

	this->items.reserve(numImages);
	for (int i = 0; i < numImages; i++) {
		FATEntry *fat = new FATEntry();
		EntryPtr ep(fat);
		fat->valid = true;
		fat->attr = 0;
		fat->index = i;
		fat->offset = lenHeader + i * tileSize;
		fat->size = tileSize;
		fat->lenHeader = 0;
		this->items.push_back(ep);
	}

}

CComic2Tileset::~CComic2Tileset()
{
}

int CComic2Tileset::getCaps()
{
	return Tileset::ColourDepthEGA;
}

void CComic2Tileset::resize(EntryPtr& id, stream::len newSize)
{
	if (newSize != CC2_TILE_WIDTH / 8 * CC2_TILE_HEIGHT * this->numPlanes) {
		throw stream::error("tiles in this tileset are a fixed size");
	}
	return;
}

void CComic2Tileset::getTilesetDimensions(unsigned int *width, unsigned int *height)
{
	*width = CC2_TILE_WIDTH;
	*height = CC2_TILE_HEIGHT;
	return;
}

unsigned int CComic2Tileset::getLayoutWidth()
{
	return 4;
}

ImagePtr CComic2Tileset::createImageInstance(const EntryPtr& id,
	stream::inout_sptr content)
{
	PLANE_LAYOUT planes;

	planes[PLANE_BLUE] = 1;
	planes[PLANE_GREEN] = 2;
	planes[PLANE_RED] = 3;
	planes[PLANE_INTENSITY] = 4;
	planes[PLANE_HITMAP] = 0;
	planes[PLANE_OPACITY] = (this->numPlanes == NUMPLANES_TILES) ? 0 : 5;

	EGAPlanarImage *ega = new EGAPlanarImage();
	ImagePtr conv(ega);
	ega->setParams(
		content, 0, CC2_TILE_WIDTH, CC2_TILE_HEIGHT, planes
	);

	return conv;
}

} // namespace gamegraphics
} // namespace camoto
