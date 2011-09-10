/**
 * @file   tls-pcx.cpp
 * @brief  Tiles stored in a PCX image
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

#include <boost/shared_array.hpp>
#include "img-magick.hpp"
#include "tls-pcx.hpp"

namespace camoto {
namespace gamegraphics {

PCXTileset::PCXTileset(iostream_sptr data, FN_TRUNCATE fnTruncate,
	int tileWidth, int tileHeight, int tilesWide, int tilesHigh)
	throw (std::ios::failure) :
	data(data),
	tileWidth(tileWidth),
	tileHeight(tileHeight),
	tilesWide(tilesWide),
	tilesHigh(tilesHigh)
{
	assert(this->data->good());

	this->data->seekg(0, std::ios::end);
	unsigned long lenData = this->data->tellg();
	this->data->seekg(0, std::ios::beg);

	boost::shared_array<uint8_t> imgData(new uint8_t[lenData]);
	this->data->read((char *)imgData.get(), lenData);

	try {
		Magick::Blob blob;
		blob.update(imgData.get(), lenData);
		this->img.read(blob);
	} catch (Magick::Exception& e) {
		throw std::ios::failure(e.what());
	}


	int numImages = this->tilesWide * this->tilesHigh;
	this->items.reserve(numImages);
	for (int i = 0; i < numImages; i++) {
		PCXEntry *fat = new PCXEntry();
		EntryPtr ep(fat);
		fat->isValid = true;
		fat->attr = None;
		fat->index = i;
		this->items.push_back(ep);
	}

}

PCXTileset::~PCXTileset()
	throw ()
{
}

int PCXTileset::getCaps()
	throw ()
{
	return Tileset::HasPalette;
}

const Tileset::VC_ENTRYPTR& PCXTileset::getItems() const
	throw ()
{
	return this->items;
}

ImagePtr PCXTileset::openImage(const EntryPtr& id)
	throw (std::ios::failure)
{
	PCXEntry *fat = dynamic_cast<PCXEntry *>(id.get());
	assert(fat);
	int x = (fat->index % this->tilesWide) * this->tileWidth;
	int y = (fat->index / this->tilesWide) * this->tileHeight;

	assert(x + this->tileWidth <= 320);
	assert(y + this->tileHeight <= 200);

	MagickImage *subimg = new MagickImage(shared_from_this(), this->img, x, y,
		this->tileWidth, this->tileHeight);
	return ImagePtr(subimg);
}

Tileset::EntryPtr PCXTileset::insert(const EntryPtr& idBeforeThis, int attr)
	throw (std::ios::failure)
{
	throw std::ios::failure("tiles in this tileset cannot be rearranged (yet?)");
}

void PCXTileset::remove(EntryPtr& id)
	throw (std::ios::failure)
{
	throw std::ios::failure("tiles in this tileset cannot be rearranged (yet?)");
}

void PCXTileset::flush()
	throw (std::ios::failure)
{
	return;
}

void PCXTileset::resize(EntryPtr& id, size_t newSize)
	throw (std::ios::failure)
{
	if (newSize != this->tileWidth * this->tileHeight) {
		throw std::ios::failure("tiles in this tileset are a fixed size");
	}
	return;
}

void PCXTileset::getTilesetDimensions(unsigned int *width, unsigned int *height)
	throw ()
{
	*width = this->tileWidth;
	*height = this->tileHeight;
	return;
}

unsigned int PCXTileset::getLayoutWidth()
	throw ()
{
	return this->tilesWide;
}

PaletteTablePtr PCXTileset::getPalette()
	throw ()
{
	int numColours = this->img.colorMapSize();

	PaletteTablePtr pal(new PaletteTable());
	pal->reserve(numColours);

	for (int c = 0; c < numColours; c++) {
		Magick::Color s = this->img.colorMap(c);
		PaletteEntry p;
		p.red = s.redQuantum() * 255 / QuantumRange;
		p.green = s.greenQuantum() * 255 / QuantumRange;
		p.blue = s.blueQuantum() * 255 / QuantumRange;
		p.alpha = 255;
		pal->push_back(p);
	}

	return pal;
}

void PCXTileset::setPalette(PaletteTablePtr newPalette)
	throw (std::ios::failure)
{
	int numColours = newPalette->size();

	try {
		this->img.colorMapSize(numColours);

		for (int c = 0; c < numColours; c++) {
			Magick::Color s;
			s.redQuantum((*newPalette)[c].red * QuantumRange / 255);
			s.greenQuantum((*newPalette)[c].green * QuantumRange / 255);
			s.blueQuantum((*newPalette)[c].blue * QuantumRange / 255);
			this->img.colorMap(c, s);
		}
	} catch (Magick::Exception& e) {
		throw std::ios::failure(e.what());
	}

	return;
}

} // namespace gamegraphics
} // namespace camoto
