/**
 * @file  tls-ega-apogee.cpp
 * @brief Common EGA tileset routines used by Apogee games
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Cosmo_Tileset_Format
 *   http://www.shikadi.net/moddingwiki/Major_Stryker_Tileset_Format
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
#include "tls-ega-apogee.hpp"

namespace camoto {
namespace gamegraphics {

/// Offset of first tileset in a tileset collection.
#define EGA_APOGEE_FIRST_TILE_OFFSET 0

EGAApogeeTileset::EGAApogeeTileset(stream::inout_sptr data,
	unsigned int tileWidth, unsigned int tileHeight, unsigned int numPlanes,
	unsigned int idealWidth, PaletteTablePtr pal)
	:	FATTileset(data, EGA_APOGEE_FIRST_TILE_OFFSET),
		tileWidth(tileWidth),
		tileHeight(tileHeight),
		numPlanes(numPlanes),
		idealWidth(idealWidth),
		pal(pal)
{
	int tileSize = tileWidth / 8 * tileHeight * this->numPlanes;

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

EGAApogeeTileset::~EGAApogeeTileset()
{
}

int EGAApogeeTileset::getCaps()
{
	return Tileset::ColourDepthEGA
		| (this->pal ? Tileset::HasPalette : 0);
}

void EGAApogeeTileset::resize(EntryPtr& id, stream::len newSize)
{
	if (newSize != this->tileWidth / 8 * this->tileHeight * this->numPlanes) {
		throw stream::error("tiles in this tileset are a fixed size");
	}
	return;
}

void EGAApogeeTileset::getTilesetDimensions(unsigned int *width, unsigned int *height)
{
	*width = this->tileWidth;
	*height = this->tileHeight;
	return;
}

unsigned int EGAApogeeTileset::getLayoutWidth()
{
	return this->idealWidth;
}

PaletteTablePtr EGAApogeeTileset::getPalette()
{
	return this->pal;
}

ImagePtr EGAApogeeTileset::createImageInstance(const EntryPtr& id,
	stream::inout_sptr content)
{
	PLANE_LAYOUT planes;
	int offset = (this->numPlanes == EGA_NUMPLANES_SOLID) ? 1 : 0;
	planes[PLANE_BLUE] = 2 - offset;
	planes[PLANE_GREEN] = 3 - offset;
	planes[PLANE_RED] = 4 - offset;
	planes[PLANE_INTENSITY] = 5 - offset;
	planes[PLANE_HITMAP] = 0;
	planes[PLANE_OPACITY] = 1 - offset;

	EGABytePlanarImage *ega = new EGABytePlanarImage();
	ImagePtr conv(ega);
	ega->setParams(
		content, 0, this->tileWidth, this->tileHeight, planes, this->pal
	);

	return conv;
}

} // namespace gamegraphics
} // namespace camoto
