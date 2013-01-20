/**
 * @file   tls-img.cpp
 * @brief  Tiles stored as a grid in an image file.
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
#include "tls-img.hpp"
#include "subimage.hpp"

namespace camoto {
namespace gamegraphics {

struct ImageEntry: public BaseTileset::BaseTilesetEntry {
	int index; ///< Zero-based index of tile
};

TilesetFromImage::TilesetFromImage(ImagePtr img, unsigned int tileWidth,
	unsigned int tileHeight, unsigned int tilesWide, unsigned int tilesHigh)
	:	img(img),
		tileWidth(tileWidth),
		tileHeight(tileHeight),
		tilesWide(tilesWide),
		tilesHigh(tilesHigh),
		hasImageChanged(false)
{
	unsigned int numImages = this->tilesWide * this->tilesHigh;
	this->items.reserve(numImages);
	for (unsigned int i = 0; i < numImages; i++) {
		ImageEntry *fat = new ImageEntry();
		EntryPtr ep(fat);
		fat->valid = true;
		fat->attr = 0;
		fat->index = i;
		this->items.push_back(ep);
	}
	this->fnImageChanged = boost::bind<void>(&TilesetFromImage::imageChanged, this);
}

TilesetFromImage::~TilesetFromImage()
{
}

int TilesetFromImage::getCaps()
{
	int caps = 0;
	int imgCaps = this->img->getCaps();
	if (imgCaps & Image::HasPalette) caps |= Tileset::HasPalette;
	return caps;
}

const Tileset::VC_ENTRYPTR& TilesetFromImage::getItems() const
{
	return this->items;
}

ImagePtr TilesetFromImage::openImage(const EntryPtr& id)
{
	if ((!this->stdImg) && (!this->stdMask)) {
		this->stdImg = this->img->toStandard();
		this->stdMask = this->img->toStandardMask();
	}
	ImageEntry *fat = dynamic_cast<ImageEntry *>(id.get());
	assert(fat);
	int x = (fat->index % this->tilesWide) * this->tileWidth;
	int y = (fat->index / this->tilesWide) * this->tileHeight;

#ifdef DEBUG
	unsigned int imgWidth, imgHeight;
	this->img->getDimensions(&imgWidth, &imgHeight);
	assert(x + this->tileWidth <= imgWidth);
	assert(y + this->tileHeight <= imgHeight);
#endif

	ImagePtr subimg(new SubImage(this->img, this->stdImg, this->stdMask, x, y,
		this->tileWidth, this->tileHeight, this->fnImageChanged));
	return subimg;
}

Tileset::EntryPtr TilesetFromImage::insert(const EntryPtr& idBeforeThis, int attr)
{
	throw stream::error("tiles in this tileset cannot be rearranged (yet?)");
}

void TilesetFromImage::remove(EntryPtr& id)
{
	throw stream::error("tiles in this tileset cannot be rearranged (yet?)");
}

void TilesetFromImage::flush()
{
	if (this->hasImageChanged) {
		this->img->fromStandard(this->stdImg, this->stdMask);
		this->hasImageChanged = false;
	}
	return;
}

void TilesetFromImage::resize(EntryPtr& id, stream::len newSize)
{
	if (newSize != this->tileWidth * this->tileHeight) {
		throw stream::error("tiles in this tileset are a fixed size");
	}
	return;
}

void TilesetFromImage::getTilesetDimensions(unsigned int *width, unsigned int *height)
{
	*width = this->tileWidth;
	*height = this->tileHeight;
	return;
}

unsigned int TilesetFromImage::getLayoutWidth()
{
	return this->tilesWide;
}

PaletteTablePtr TilesetFromImage::getPalette()
{
	return this->img->getPalette();
}

void TilesetFromImage::setPalette(PaletteTablePtr newPalette)
{
	this->img->setPalette(newPalette);
	return;
}

void TilesetFromImage::imageChanged()
{
	this->hasImageChanged = true;
	return;
}

} // namespace gamegraphics
} // namespace camoto
