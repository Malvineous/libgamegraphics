/**
 * @file   tilesetFromList.cpp
 * @brief  Tiles loaded from a list of image files and coordinates.
 *
 * Copyright (C) 2010-2012 Adam Nielsen <malvineous@shikadi.net>
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

#include "tilesetFromList.hpp"
#include "subimage.hpp"

namespace camoto {
namespace gamegraphics {

struct ImageEntry: public BaseTileset::BaseTilesetEntry {
	unsigned int index; ///< Zero-based index of tile
};

TilesetFromList::TilesetFromList(const TileList& tileList)
	:	tileList(tileList)
{
	unsigned int numImages = this->tileList.size();
	this->items.reserve(numImages);
	for (unsigned int i = 0; i < numImages; i++) {
		ImageEntry *fat = new ImageEntry();
		EntryPtr ep(fat);
		fat->valid = true;
		fat->attr = 0;
		fat->index = i;
		this->items.push_back(ep);
	}
}

TilesetFromList::~TilesetFromList()
{
}

int TilesetFromList::getCaps()
{
	int caps = 0;
	if (this->tileList.size() > 0) {
		ImagePtr first = this->tileList[0].img;
		assert(first);
		int imgCaps = first->getCaps();
		if (imgCaps & Image::HasPalette) caps |= Tileset::HasPalette;
	}
	return caps;
}

const Tileset::VC_ENTRYPTR& TilesetFromList::getItems() const
{
	return this->items;
}

ImagePtr TilesetFromList::openImage(const EntryPtr& id)
{
	ImageEntry *fat = dynamic_cast<ImageEntry *>(id.get());
	assert(fat);

	if (fat->index >= this->tileList.size()) {
		throw stream::error("tile no longer exists/out of range");
	}

	Tile& t = this->tileList[fat->index];

#ifdef DEBUG
	unsigned int imgWidth, imgHeight;
	t.img->getDimensions(&imgWidth, &imgHeight);
	assert(t.xOffset + t.width <= imgWidth);
	assert(t.yOffset + t.height <= imgHeight);
#endif

	ImagePtr subimg(new SubImage(t.img, t.xOffset, t.yOffset, t.width, t.height));
	return subimg;
}

Tileset::EntryPtr TilesetFromList::insert(const EntryPtr& idBeforeThis, int attr)
{
	throw stream::error("tiles in this tileset cannot be rearranged");
}

void TilesetFromList::remove(EntryPtr& id)
{
	throw stream::error("tiles in this tileset cannot be rearranged");
}

void TilesetFromList::flush()
{
	return;
}

void TilesetFromList::resize(EntryPtr& id, stream::len newSize)
{
	ImageEntry *fat = dynamic_cast<ImageEntry *>(id.get());
	assert(fat);

	if (fat->index >= this->tileList.size()) {
		throw stream::error("tile no longer exists/out of range");
	}

	Tile& t = this->tileList[fat->index];
	if (newSize != t.width * t.height) {
		throw stream::error("tiles in this tileset are a fixed size");
	}
	return;
}

PaletteTablePtr TilesetFromList::getPalette()
{
	assert(this->getCaps() & Tileset::HasPalette);
	// Should not get here if tileList[0] is out of range
	return this->tileList[0].img->getPalette();
}

void TilesetFromList::setPalette(PaletteTablePtr newPalette)
{
	// Set the palette for all subimages that support it
	ImagePtr lastImage;
	for (TileList::const_iterator i = tileList.begin(); i != tileList.end(); i++) {
		if (lastImage != i->img) {
			if (i->img->getCaps() & Image::CanSetPalette) {
				i->img->setPalette(newPalette);
			}
			// Try to minimise the number of tiles setPalette() is called on the
			// same image.
			lastImage = i->img;
		}
	}

	return;
}

} // namespace gamegraphics
} // namespace camoto
