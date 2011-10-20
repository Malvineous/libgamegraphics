/**
 * @file   tls-harry.cpp
 * @brief  Halloween Harry tileset handler.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/CHR_Format_(Halloween_Harry)
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

#include <boost/bind.hpp>
#include <camoto/iostream_helpers.hpp>

#include "pal-gmf-harry.hpp"
#include "img-vga-raw.hpp"
#include "tls-harry.hpp"

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

namespace io = boost::iostreams;

HarryTilesetType::HarryTilesetType()
	throw ()
{
}

HarryTilesetType::~HarryTilesetType()
	throw ()
{
}

std::string HarryTilesetType::getCode() const
	throw ()
{
	return "tls-harry";
}

std::string HarryTilesetType::getFriendlyName() const
	throw ()
{
	return "Halloween Harry tileset";
}

std::vector<std::string> HarryTilesetType::getFileExtensions() const
	throw ()
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("chr");
	return vcExtensions;
}

std::vector<std::string> HarryTilesetType::getGameList() const
	throw ()
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Alien Carnage");
	vcGames.push_back("Halloween Harry");
	return vcGames;
}

HarryTilesetType::Certainty HarryTilesetType::isInstance(
	stream::input_sptr psTileset) const
	throw (stream::error)
{
	stream::pos len = psTileset->size();

	// TESTED BY: tls_zone66_isinstance_c01
	if (len != CHR_WIDTH * CHR_HEIGHT * CHR_NUM_TILES) return DefinitelyNo; // wrong size

	// TESTED BY: tls_zone66_isinstance_c00
	return PossiblyYes;
}

TilesetPtr HarryTilesetType::create(stream::inout_sptr psTileset,
	SuppData& suppData) const
	throw (stream::error)
{
	psTileset->truncate(CHR_WIDTH * CHR_HEIGHT * 256);
	psTileset->seekp(0, stream::start);
	char empty[CHR_WIDTH * CHR_HEIGHT];
	memset(empty, 0x00, CHR_WIDTH * CHR_HEIGHT);
	for (int i = 0; i < 256; i++) psTileset->write(empty, CHR_WIDTH * CHR_HEIGHT);

	PaletteTablePtr pal;
	// Only load the palette if one was given
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new VGAPalette(suppData[SuppItem::Palette]));
		pal = palFile->getPalette();
	}
	return TilesetPtr(new HarryTileset(psTileset, pal));
}

TilesetPtr HarryTilesetType::open(stream::inout_sptr psTileset,
	SuppData& suppData) const
	throw (stream::error)
{
	PaletteTablePtr pal;
	// Only load the palette if one was given
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new GMFHarryPalette(suppData[SuppItem::Palette]));
		pal = palFile->getPalette();
	} else {
		throw stream::error("no palette specified (missing supplementary item)");
	}
	return TilesetPtr(new HarryTileset(psTileset, pal));
}

SuppFilenames HarryTilesetType::getRequiredSupps(
	const std::string& filenameTileset) const
	throw ()
{
	SuppFilenames supps;
	// Convert "missionX.chr" into "mXz1.gmf"
	std::string palName = "m";
	palName += filenameTileset[filenameTileset.length() - 5];
	palName += "z1.gmf";
	supps[SuppItem::Palette] = palName;
	return supps;
}


HarryTileset::HarryTileset(stream::inout_sptr data,
	PaletteTablePtr pal)
	throw (stream::error) :
		FATTileset(data, CHR_FIRST_TILE_OFFSET),
		pal(pal)
{
	assert(this->pal);

	for (int i = 0; i < CHR_NUM_TILES; i++) {
		FATEntry *fat = new FATEntry();
		EntryPtr ep(fat);
		fat->isValid = true;
		fat->attr = None;
		fat->index = i;
		fat->offset = i * CHR_WIDTH * CHR_HEIGHT;
		fat->lenHeader = 0;
		fat->size = CHR_WIDTH * CHR_HEIGHT;
		this->items.push_back(ep);
	}
}

HarryTileset::~HarryTileset()
	throw ()
{
}

int HarryTileset::getCaps()
	throw ()
{
	return HasPalette;
}

ImagePtr HarryTileset::createImageInstance(const EntryPtr& id,
	stream::inout_sptr content)
	throw (stream::error)
{
	ImagePtr img(new VGARawImage(content, CHR_WIDTH, CHR_HEIGHT, this->pal));
	return img;
}

PaletteTablePtr HarryTileset::getPalette()
	throw ()
{
	return this->pal;
}

void HarryTileset::setPalette(PaletteTablePtr newPalette)
	throw (stream::error)
{
	// This doesn't save anything to the file as the palette is stored externally.
	this->pal = newPalette;
	return;
}

} // namespace gamegraphics
} // namespace camoto
