/**
 * @file  image-from_tileset.cpp
 * @brief Combine some tiles together to form an Image.
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

#include <cassert>
#include "image-from_tileset.hpp"

namespace camoto {
namespace gamegraphics {

Image_FromTileset::Image_FromTileset(std::shared_ptr<Tileset> tileset,
	unsigned int first, unsigned int span, Point dimsInTiles)
	:	tileset(tileset),
		first(first),
		span(span),
		dimsInTiles(dimsInTiles)
{
	auto tileDims = this->tileset->dimensions();
	if ((tileDims.x == 0) || (tileDims.y == 0)) {
		throw stream::error("Cannot construct an image from tiles unless all tiles "
			"are the same size!");
	}
	this->dimsInPixels.x = tileDims.x * this->dimsInTiles.x;
	this->dimsInPixels.y = tileDims.y * this->dimsInTiles.y;
}

Image_FromTileset::~Image_FromTileset()
{
}

Image::Caps Image_FromTileset::caps() const
{
	return (this->palette() ? Caps::HasPalette : Caps::Default);
}

ColourDepth Image_FromTileset::colourDepth() const
{
	return this->tileset->colourDepth();
}

Point Image_FromTileset::dimensions() const
{
	return this->dimsInPixels;
}

void Image_FromTileset::dimensions(const Point& newDimensions)
{
	assert(this->caps() & Caps::SetDimensions);
	throw stream::error("An image constructed from a tileset cannot be resized.  "
		"Try resizing the underlying tileset.");
}

Pixels Image_FromTileset::convert() const
{
	if (this->pixels.size() == 0) {
		// Populate cache
		auto noconst_this = const_cast<Image_FromTileset*>(this);
		noconst_this->doConversion(true);
	}
	return this->pixels;
}

Pixels Image_FromTileset::convert_mask() const
{
	if (this->mask.size() == 0) {
		// Populate cache
		auto noconst_this = const_cast<Image_FromTileset*>(this);
		noconst_this->doConversion(true);
	}
	return this->mask;
}

void Image_FromTileset::convert(const Pixels& newContent,
	const Pixels& newMask)
{
	this->pixels = newContent;
	this->mask = newMask;
	this->doConversion(false);
	return;
}

void Image_FromTileset::doConversion(bool toImage)
{
	auto tileDims = this->tileset->dimensions();
	if (toImage) {
		this->pixels.resize(this->dimsInPixels.x * this->dimsInPixels.y);
		this->mask.resize(this->dimsInPixels.x * this->dimsInPixels.y);
	}
	auto tiles = this->tileset->files();
	unsigned int firstTileOnRow = this->first;
	for (int y = 0; y < this->dimsInTiles.y; y++) {
		for (int x = 0; x < this->dimsInTiles.x; x++) {
			auto tileIndex = firstTileOnRow + x;
			auto& tileHandle = tiles[tileIndex];
			// TODO: assert tileHandle->index (if present) matches tileIndex
			auto tile = this->tileset->openImage(tileHandle);
			Pixels tileImg, tileMask;
			if (toImage) {
				tileImg = tile->convert();
				tileMask = tile->convert_mask();
			} else {
				tileImg.resize(tileDims.x * tileDims.y);
				tileMask.resize(tileDims.x * tileDims.y);
			}
			unsigned int dstOffset = y * tileDims.y * this->dimsInPixels.x
				+ x * tileDims.x;
			auto blit = [this, toImage, dstOffset, &tileDims]
				(Pixels& dst, Pixels& src)
			{
				for (int ty = 0; ty < tileDims.y; ty++) {
					auto pDst = &dst[dstOffset + ty * this->dimsInPixels.x];
					auto pSrc = &src[ty * tileDims.x];
					if (!toImage) std::swap(pDst, pSrc);
					memcpy(pDst, pSrc, tileDims.x);
				}
				return;
			};
			blit(this->pixels, tileImg);
			blit(this->mask, tileMask);
			if (!toImage) {
				tile->convert(tileImg, tileMask);
			}
		}
		firstTileOnRow += this->span;
	}
	return;
}

} // namespace gamegraphics
} // namespace camoto
