/**
 * @file   tilesetFromList.cpp
 * @brief  Tiles loaded from a list of image files and coordinates.
 *
 * Copyright (C) 2010-2013 Adam Nielsen <malvineous@shikadi.net>
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

#include <boost/bind.hpp>
#include "tilesetFromList.hpp"
#include "subimage.hpp"

namespace camoto {
namespace gamegraphics {

TilesetPtr createTilesetFromList(const TileList& tileList, ImagePtr img,
	unsigned int layoutWidth)
{
	return TilesetPtr(new TilesetFromList(tileList, img, layoutWidth));
}

struct ImageEntry: public BaseTileset::BaseTilesetEntry {
	unsigned int index; ///< Zero-based index of tile
};

TilesetFromList::TilesetFromList(const TileList& tileList, ImagePtr img,
	unsigned int layoutWidth)
	:	tileList(tileList),
		img(img),
		hasImageChanged(false),
		layoutWidth(layoutWidth)
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
	this->fnImageChanged = boost::bind<void>(&TilesetFromList::imageChanged, this);
}

TilesetFromList::~TilesetFromList()
{
}

int TilesetFromList::getCaps()
{
	int caps = 0;
	int imgCaps = this->img->getCaps();
	if (imgCaps & Image::HasPalette) caps |= Tileset::HasPalette;
	return caps;
}

const Tileset::VC_ENTRYPTR& TilesetFromList::getItems() const
{
	return this->items;
}

ImagePtr TilesetFromList::openImage(const EntryPtr& id)
{
	if ((!this->stdImg) && (!this->stdMask)) {
		this->stdImg = this->img->toStandard();
		this->stdMask = this->img->toStandardMask();
	}

	ImageEntry *fat = dynamic_cast<ImageEntry *>(id.get());
	assert(fat);

	if (fat->index >= this->tileList.size()) {
		throw stream::error("tile no longer exists/out of range");
	}

	TilePos& t = this->tileList[fat->index];

#ifdef DEBUG
	unsigned int imgWidth, imgHeight;
	t.img->getDimensions(&imgWidth, &imgHeight);
	assert(t.xOffset + t.width <= imgWidth);
	assert(t.yOffset + t.height <= imgHeight);
#endif

	ImagePtr subimg(new SubImage(this->img, this->stdImg, this->stdMask, t.xOffset,
		t.yOffset, t.width, t.height, this->fnImageChanged));
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
	if (this->hasImageChanged) {
		this->img->fromStandard(this->stdImg, this->stdMask);
		this->hasImageChanged = false;
	}
	return;
}

void TilesetFromList::resize(EntryPtr& id, stream::len newSize)
{
	ImageEntry *fat = dynamic_cast<ImageEntry *>(id.get());
	assert(fat);

	if (fat->index >= this->tileList.size()) {
		throw stream::error("tile no longer exists/out of range");
	}

	TilePos& t = this->tileList[fat->index];
	if (newSize != t.width * t.height) {
		throw stream::error("tiles in this tileset are a fixed size");
	}
	return;
}

unsigned int TilesetFromList::getLayoutWidth()
{
	return this->layoutWidth;
}

PaletteTablePtr TilesetFromList::getPalette()
{
	assert(this->getCaps() & Tileset::HasPalette);
	return this->img->getPalette();
}

void TilesetFromList::setPalette(PaletteTablePtr newPalette)
{
	this->img->setPalette(newPalette);
	return;
}

void TilesetFromList::imageChanged()
{
	this->hasImageChanged = true;
	return;
}

} // namespace gamegraphics
} // namespace camoto
