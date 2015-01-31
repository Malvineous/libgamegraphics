/**
 * @file  tls-zone66-map.cpp
 * @brief Zone 66 map tileset
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Zone_66_Level_Format
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
// TilesetType_Zone66Map
//

TilesetType_Zone66Map::TilesetType_Zone66Map()
{
}

TilesetType_Zone66Map::~TilesetType_Zone66Map()
{
}

std::string TilesetType_Zone66Map::getCode() const
{
	return "tls-zone66-map";
}

std::string TilesetType_Zone66Map::getFriendlyName() const
{
	return "Zone 66 Map Tileset";
}

std::vector<std::string> TilesetType_Zone66Map::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("z66");
	return vcExtensions;
}

std::vector<std::string> TilesetType_Zone66Map::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Zone 66");
	return vcGames;
}

TilesetType_Zone66Map::Certainty TilesetType_Zone66Map::isInstance(stream::input_sptr psGraphics) const
{
	stream::pos len = psGraphics->size();

	// Standard tileset
	if (len % Z66_TILE_SIZE != 0) return DefinitelyNo;

	return PossiblyYes;
}

TilesetPtr TilesetType_Zone66Map::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	psGraphics->seekp(0, stream::start);
	PaletteTablePtr pal;
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new Palette_VGA(suppData[SuppItem::Palette], Z66_PAL_DEPTH));
		pal = palFile->getPalette();
	}
	return TilesetPtr(new Tileset_Zone66Map(psGraphics, pal));
}

TilesetPtr TilesetType_Zone66Map::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	psGraphics->seekp(0, stream::start);
	PaletteTablePtr pal;
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new Palette_VGA(suppData[SuppItem::Palette], Z66_PAL_DEPTH));
		pal = palFile->getPalette();
	}
	return TilesetPtr(new Tileset_Zone66Map(psGraphics, pal));
}

SuppFilenames TilesetType_Zone66Map::getRequiredSupps(const std::string& filenameGraphics) const
{
	SuppFilenames supps;
	supps[SuppItem::Palette] = "mpal.z66";
	return supps;
}


//
// Tileset_Zone66Map
//

Tileset_Zone66Map::Tileset_Zone66Map(stream::inout_sptr data, PaletteTablePtr pal)
	:	Tileset_FAT(data, Z66_FIRST_TILE_OFFSET),
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

Tileset_Zone66Map::~Tileset_Zone66Map()
{
}

int Tileset_Zone66Map::getCaps()
{
	return Tileset::ColourDepthVGA
		| (this->pal ? Tileset::HasPalette : 0);
}

void Tileset_Zone66Map::resize(EntryPtr& id, stream::len newSize)
{
	if (newSize != Z66_TILE_SIZE) {
		throw stream::error("Zone 66 map tiles are a fixed size");
	}
	return;
}

void Tileset_Zone66Map::getTilesetDimensions(unsigned int *width, unsigned int *height)
{
	*width = Z66_TILE_WIDTH;
	*height = Z66_TILE_HEIGHT;
	return;
}

unsigned int Tileset_Zone66Map::getLayoutWidth()
{
	return 18;
}

PaletteTablePtr Tileset_Zone66Map::getPalette()
{
	return this->pal;
}

ImagePtr Tileset_Zone66Map::createImageInstance(const EntryPtr& id,
	stream::inout_sptr content)
{
	return ImagePtr(new Image_VGARaw(content, Z66_TILE_WIDTH, Z66_TILE_HEIGHT,
		this->pal));
}

} // namespace gamegraphics
} // namespace camoto
