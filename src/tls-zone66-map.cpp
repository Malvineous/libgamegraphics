/**
 * @file   tls-zone66-map.cpp
 * @brief  Zone 66 map tileset
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Zone_66_Level_Format
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

#include <camoto/iostream_helpers.hpp>
#include "img-vga-raw.hpp"
#include "pal-vga-raw.hpp"
#include "tls-zone66-map.hpp"

namespace camoto {
namespace gamegraphics {

/// Number of planes in each image
#define NUMPLANES_SPRITE  5
#define NUMPLANES_TILE    4

/// Offset of first tileset in a tileset collection.
#define Z66_FIRST_TILE_OFFSET 0

/// Width of each tile, in pixels
#define Z66_TILE_WIDTH  32

/// Height of each tile, in pixels
#define Z66_TILE_HEIGHT 32

/// Size of each tile, in bytes
#define Z66_TILE_SIZE (Z66_TILE_WIDTH * Z66_TILE_HEIGHT)

/// Colour depth of VGA palette
#define Z66_PAL_DEPTH 6

//
// Zone66MapTilesetType
//

Zone66MapTilesetType::Zone66MapTilesetType()
{
}

Zone66MapTilesetType::~Zone66MapTilesetType()
{
}

std::string Zone66MapTilesetType::getCode() const
{
	return "tls-zone66-map";
}

std::string Zone66MapTilesetType::getFriendlyName() const
{
	return "Zone 66 Map Tileset";
}

std::vector<std::string> Zone66MapTilesetType::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("z66");
	return vcExtensions;
}

std::vector<std::string> Zone66MapTilesetType::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Zone 66");
	return vcGames;
}

Zone66MapTilesetType::Certainty Zone66MapTilesetType::isInstance(stream::input_sptr psGraphics) const
{
	stream::pos len = psGraphics->size();

	// Standard tileset
	if (len % Z66_TILE_SIZE != 0) return DefinitelyNo;

	return PossiblyYes;
}

TilesetPtr Zone66MapTilesetType::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	psGraphics->seekp(0, stream::start);
	PaletteTablePtr pal;
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new VGAPalette(suppData[SuppItem::Palette], Z66_PAL_DEPTH));
		pal = palFile->getPalette();
	}
	return TilesetPtr(new Zone66MapTileset(psGraphics, pal));
}

TilesetPtr Zone66MapTilesetType::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	psGraphics->seekp(0, stream::start);
	PaletteTablePtr pal;
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new VGAPalette(suppData[SuppItem::Palette], Z66_PAL_DEPTH));
		pal = palFile->getPalette();
	}
	return TilesetPtr(new Zone66MapTileset(psGraphics, pal));
}

SuppFilenames Zone66MapTilesetType::getRequiredSupps(const std::string& filenameGraphics) const
{
	SuppFilenames supps;
	supps[SuppItem::Palette] = "mpal.z66";
	return supps;
}


//
// Zone66MapTileset
//

Zone66MapTileset::Zone66MapTileset(stream::inout_sptr data, PaletteTablePtr pal)
	:	FATTileset(data, Z66_FIRST_TILE_OFFSET),
		pal(pal)
{
	stream::pos len = this->data->size();

	this->data->seekg(0, stream::start);
	int numImages = len / Z66_TILE_SIZE;

	this->items.reserve(numImages);
	for (int i = 0; i < numImages; i++) {
		FATEntry *fat = new FATEntry();
		EntryPtr ep(fat);
		fat->valid = true;
		fat->attr = 0;
		fat->index = i;
		fat->offset = i * Z66_TILE_SIZE;
		fat->size = Z66_TILE_SIZE;
		fat->lenHeader = 0;
		this->items.push_back(ep);
	}

}

Zone66MapTileset::~Zone66MapTileset()
{
}

int Zone66MapTileset::getCaps()
{
	return Tileset::ColourDepthVGA
		| (this->pal ? Tileset::HasPalette : 0);
}

void Zone66MapTileset::resize(EntryPtr& id, stream::len newSize)
{
	if (newSize != Z66_TILE_SIZE) {
		throw stream::error("Zone 66 map tiles are a fixed size");
	}
	return;
}

void Zone66MapTileset::getTilesetDimensions(unsigned int *width, unsigned int *height)
{
	*width = Z66_TILE_WIDTH;
	*height = Z66_TILE_HEIGHT;
	return;
}

unsigned int Zone66MapTileset::getLayoutWidth()
{
	return 18;
}

PaletteTablePtr Zone66MapTileset::getPalette()
{
	return this->pal;
}

ImagePtr Zone66MapTileset::createImageInstance(const EntryPtr& id,
	stream::inout_sptr content)
{
	return ImagePtr(new VGARawImage(content, Z66_TILE_WIDTH, Z66_TILE_HEIGHT,
		this->pal));
}

} // namespace gamegraphics
} // namespace camoto
