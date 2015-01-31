/**
 * @file  tls-harry-chr.cpp
 * @brief Halloween Harry .CHR tileset handler.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/CHR_Format_(Halloween_Harry)
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
#include "tls-harry-chr.hpp"
#include "pal-gmf-harry.hpp"
#include "img-vga-raw.hpp"

/// Offset of first tile in an empty tileset
#define CHR_FIRST_TILE_OFFSET  0

/// Number of tiles in the file
#define CHR_NUM_TILES   255

/// Width of each image in the tileset
#define CHR_WIDTH  16

/// Height of each image in the tileset
#define CHR_HEIGHT 16

namespace camoto {
namespace gamegraphics {

TilesetType_HarryCHR::TilesetType_HarryCHR()
{
}

TilesetType_HarryCHR::~TilesetType_HarryCHR()
{
}

std::string TilesetType_HarryCHR::getCode() const
{
	return "tls-harry-chr";
}

std::string TilesetType_HarryCHR::getFriendlyName() const
{
	return "Halloween Harry CHR tileset";
}

std::vector<std::string> TilesetType_HarryCHR::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("chr");
	return vcExtensions;
}

std::vector<std::string> TilesetType_HarryCHR::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Alien Carnage");
	vcGames.push_back("Halloween Harry");
	return vcGames;
}

TilesetType_HarryCHR::Certainty TilesetType_HarryCHR::isInstance(
	stream::input_sptr psTileset) const
{
	stream::pos len = psTileset->size();

	// TESTED BY: tls_harry_chr_isinstance_c01
	if (len != CHR_WIDTH * CHR_HEIGHT * CHR_NUM_TILES) return DefinitelyNo; // wrong size

	// TESTED BY: tls_harry_chr_isinstance_c00
	return PossiblyYes;
}

TilesetPtr TilesetType_HarryCHR::create(stream::inout_sptr psTileset,
	SuppData& suppData) const
{
	psTileset->truncate(CHR_WIDTH * CHR_HEIGHT * 256);
	psTileset->seekp(0, stream::start);
	char empty[CHR_WIDTH * CHR_HEIGHT];
	memset(empty, 0x00, CHR_WIDTH * CHR_HEIGHT);
	for (int i = 0; i < 256; i++) psTileset->write(empty, CHR_WIDTH * CHR_HEIGHT);

	PaletteTablePtr pal;
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new Palette_VGA(suppData[SuppItem::Palette], 6));
		pal = palFile->getPalette();
	} else {
		throw stream::error("no palette specified (missing supplementary item)");
	}
	return TilesetPtr(new Tileset_HarryCHR(psTileset, pal));
}

TilesetPtr TilesetType_HarryCHR::open(stream::inout_sptr psTileset,
	SuppData& suppData) const
{
	PaletteTablePtr pal;
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new Palette_HarryGMF(suppData[SuppItem::Palette]));
		pal = palFile->getPalette();
	} else {
		throw stream::error("no palette specified (missing supplementary item)");
	}
	return TilesetPtr(new Tileset_HarryCHR(psTileset, pal));
}

SuppFilenames TilesetType_HarryCHR::getRequiredSupps(
	const std::string& filenameTileset) const
{
	SuppFilenames supps;
	// Convert "missionX.chr" into "mXz1.gmf"
	std::string palName = "m";
	palName += filenameTileset[filenameTileset.length() - 5];
	palName += "z1.gmf";
	supps[SuppItem::Palette] = palName;
	return supps;
}


Tileset_HarryCHR::Tileset_HarryCHR(stream::inout_sptr data,
	PaletteTablePtr pal)
	:	Tileset_FAT(data, CHR_FIRST_TILE_OFFSET),
		pal(pal)
{
	assert(this->pal);

	for (int i = 0; i < CHR_NUM_TILES; i++) {
		FATEntry *fat = new FATEntry();
		EntryPtr ep(fat);
		fat->valid = true;
		fat->attr = 0;
		fat->index = i;
		fat->offset = i * CHR_WIDTH * CHR_HEIGHT;
		fat->lenHeader = 0;
		fat->size = CHR_WIDTH * CHR_HEIGHT;
		this->items.push_back(ep);
	}
}

Tileset_HarryCHR::~Tileset_HarryCHR()
{
}

int Tileset_HarryCHR::getCaps()
{
	return Tileset::HasPalette | Tileset::ColourDepthVGA;
}

void Tileset_HarryCHR::getTilesetDimensions(unsigned int *width, unsigned int *height)
{
	*width = CHR_WIDTH;
	*height = CHR_HEIGHT;
	return;
}

unsigned int Tileset_HarryCHR::getLayoutWidth()
{
	return 18;
}

PaletteTablePtr Tileset_HarryCHR::getPalette()
{
	return this->pal;
}

ImagePtr Tileset_HarryCHR::createImageInstance(const EntryPtr& id,
	stream::inout_sptr content)
{
	ImagePtr img(new Image_VGARaw(content, CHR_WIDTH, CHR_HEIGHT, this->pal));
	return img;
}

} // namespace gamegraphics
} // namespace camoto
