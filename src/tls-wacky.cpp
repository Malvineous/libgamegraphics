/**
 * @file   tls-wacky.cpp
 * @brief  Wacky Wheels tileset
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Wacky_Wheels
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

#include "img-magick.hpp"
#include "tls-wacky.hpp"

namespace camoto {
namespace gamegraphics {

#define WW_TILE_WIDTH  32 ///< Width of each tile
#define WW_TILE_HEIGHT 32 ///< Height of each tile

#define WW_TILES_X (320 / WW_TILE_WIDTH)  ///< Number of tiles in a row
#define WW_TILES_Y (200 / WW_TILE_HEIGHT) ///< Number of tile rows

//
// WackyTilesetType
//

WackyTilesetType::WackyTilesetType()
	throw ()
{
}

WackyTilesetType::~WackyTilesetType()
	throw ()
{
}

std::string WackyTilesetType::getCode() const
	throw ()
{
	return "tls-wacky";
}

std::string WackyTilesetType::getFriendlyName() const
	throw ()
{
	return "Wacky Wheels Tileset";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> WackyTilesetType::getFileExtensions() const
	throw ()
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("pcx");
	return vcExtensions;
}

std::vector<std::string> WackyTilesetType::getGameList() const
	throw ()
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Wacky Wheels");
	return vcGames;
}

WackyTilesetType::Certainty WackyTilesetType::isInstance(iostream_sptr psGraphics) const
	throw (std::ios::failure)
{
	psGraphics->seekg(0, std::ios::end);
	unsigned long lenData = psGraphics->tellg();
	psGraphics->seekg(0, std::ios::beg);

	boost::shared_ptr<uint8_t> imgData(new uint8_t[lenData]);
	psGraphics->read((char *)imgData.get(), lenData);

	try {
		Magick::Blob blob;
		blob.update(imgData.get(), lenData);
		Magick::Image img(blob);
		if (
			(img.magick().compare("PCX") == 0) &&
			(img.columns() == 320) &&
			(img.rows() == 200)
		) return PossiblyYes;
	} catch (Magick::Exception& e) {
		throw std::ios::failure(e.what());
	}
	return DefinitelyNo;
}

TilesetPtr WackyTilesetType::create(iostream_sptr psGraphics,
	FN_TRUNCATE fnTruncate, SuppData& suppData) const
	throw (std::ios::failure)
{
	// TODO
	throw std::ios::failure("Not yet implemented");
}

TilesetPtr WackyTilesetType::open(iostream_sptr psGraphics,
	FN_TRUNCATE fnTruncate, SuppData& suppData) const
	throw (std::ios::failure)
{
	return TilesetPtr(new WackyTileset(psGraphics, fnTruncate));
}

SuppFilenames WackyTilesetType::getRequiredSupps(const std::string& filenameGraphics) const
	throw ()
{
	// No supplemental types/empty list
	return SuppFilenames();
}


//
// WackyTileset
//

WackyTileset::WackyTileset(iostream_sptr data, FN_TRUNCATE fnTruncate)
	throw (std::ios::failure) :
		data(data)
{
	assert(this->data->good());

	this->data->seekg(0, std::ios::end);
	unsigned long lenData = this->data->tellg();
	this->data->seekg(0, std::ios::beg);

	boost::shared_ptr<uint8_t> imgData(new uint8_t[lenData]);
	this->data->read((char *)imgData.get(), lenData);

	try {
		Magick::Blob blob;
		blob.update(imgData.get(), lenData);
		this->img.read(blob);
	} catch (Magick::Exception& e) {
		throw std::ios::failure(e.what());
	}


	int numImages = WW_TILES_X * WW_TILES_Y;
	this->items.reserve(numImages);
	for (int i = 0; i < numImages; i++) {
		WackyEntry *fat = new WackyEntry();
		EntryPtr ep(fat);
		fat->isValid = true;
		fat->attr = None;
		fat->index = i;
		this->items.push_back(ep);
	}

}

WackyTileset::~WackyTileset()
	throw ()
{
}

int WackyTileset::getCaps()
	throw ()
{
	return Tileset::HasPalette;
}

const Tileset::VC_ENTRYPTR& WackyTileset::getItems() const
	throw ()
{
	return this->items;
}

ImagePtr WackyTileset::openImage(const EntryPtr& id)
	throw (std::ios::failure)
{
	WackyEntry *fat = dynamic_cast<WackyEntry *>(id.get());
	assert(fat);
	int x = (fat->index % WW_TILES_X) * WW_TILE_WIDTH;
	int y = (fat->index / WW_TILES_X) * WW_TILE_HEIGHT;

	assert(x + WW_TILE_WIDTH <= 320);
	assert(y + WW_TILE_HEIGHT <= 200);

	MagickImage *subimg = new MagickImage(shared_from_this(), this->img, x, y,
		WW_TILE_WIDTH, WW_TILE_HEIGHT);
	return ImagePtr(subimg);
}

Tileset::EntryPtr WackyTileset::insert(const EntryPtr& idBeforeThis, int attr)
	throw (std::ios::failure)
{
	throw std::ios::failure("tiles in this tileset cannot be rearranged (yet?)");
}

void WackyTileset::remove(EntryPtr& id)
	throw (std::ios::failure)
{
	throw std::ios::failure("tiles in this tileset cannot be rearranged (yet?)");
}

void WackyTileset::flush()
	throw (std::ios::failure)
{
	return;
}

void WackyTileset::resize(EntryPtr& id, size_t newSize)
	throw (std::ios::failure)
{
	if (newSize != WW_TILE_WIDTH * WW_TILE_HEIGHT) {
		throw std::ios::failure("tiles in this tileset are a fixed size");
	}
	return;
}

void WackyTileset::getTilesetDimensions(unsigned int *width, unsigned int *height)
	throw ()
{
	*width = WW_TILE_WIDTH;
	*height = WW_TILE_HEIGHT;
	return;
}

unsigned int WackyTileset::getLayoutWidth()
	throw ()
{
	return WW_TILES_X;
}

PaletteTablePtr WackyTileset::getPalette()
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

void WackyTileset::setPalette(PaletteTablePtr newPalette)
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
