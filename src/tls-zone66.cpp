/**
 * @file   tls-zone66.cpp
 * @brief  Zone 66 tileset handler.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Zone_66_Tileset_Format
 *
 * Copyright (C) 2010 Adam Nielsen <malvineous@shikadi.net>
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

#include "img-zone66_tile.hpp"
#include "tls-zone66.hpp"

/// Offset of first tile in an empty tileset.
#define Z66_FIRST_TILE_OFFSET  4

/// Maximum tiles to load in case of a corrupted files.
#define Z66_SAFETY_MAX_TILES   4096

namespace camoto {
namespace gamegraphics {

namespace io = boost::iostreams;

Zone66TilesetType::Zone66TilesetType()
	throw ()
{
}

Zone66TilesetType::~Zone66TilesetType()
	throw ()
{
}

std::string Zone66TilesetType::getCode() const
	throw ()
{
	return "tls-zone66";
}

std::string Zone66TilesetType::getFriendlyName() const
	throw ()
{
	return "Zone 66 tileset";
}

std::vector<std::string> Zone66TilesetType::getFileExtensions() const
	throw ()
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("z66");
	return vcExtensions;
}

std::vector<std::string> Zone66TilesetType::getGameList() const
	throw ()
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Zone 66");
	return vcGames;
}

Zone66TilesetType::Certainty Zone66TilesetType::isInstance(
	iostream_sptr psTileset) const
	throw (std::ios::failure)
{
	psTileset->seekg(0, std::ios::end);
	io::stream_offset len = psTileset->tellg();
	// TESTED BY: TODO
	if (len < Z66_FIRST_TILE_OFFSET) return DefinitelyNo; // too short

	psTileset->seekg(0, std::ios::beg);
	uint32_t numFiles;
	psTileset >> u32le(numFiles);

	if ((numFiles == 0) && (len > 8)) return DefinitelyNo; // invalid empty file

	uint32_t offset, lastOffset = 0;
	for (int i = 0; i < numFiles; i++) {
		psTileset >> u32le(offset);

		// The first file always starts at offset 0.
		// TESTED BY: TODO
		if ((i == 0) && (offset != 0)) return DefinitelyNo;

		// Make sure the offsets are increasing, otherwise we'd get a negative
		// file size (or the file has been tweaked to make opening difficult, but
		// then there's the -f option to gamegfx for that.)
		// TESTED BY: TODO
		if (offset < lastOffset) return DefinitelyNo;

		// Make sure the tile is contained within the file
		// TESTED BY: TODO
		if ((numFiles+1) * 4 + offset > len) return DefinitelyNo;

		lastOffset = offset;
	}

	// TESTED BY: TODO
	return PossiblyYes;
}

TilesetPtr Zone66TilesetType::create(iostream_sptr psTileset,
	FN_TRUNCATE fnTruncate, MP_SUPPDATA& suppData) const
	throw (std::ios::failure)
{
	psTileset->seekp(0, std::ios::beg);
	psTileset << u32le(0);
	ImagePtr palFile(new VGAPalette(
		suppData[SuppItem::Palette].stream,
		suppData[SuppItem::Palette].fnTruncate
	));
	PaletteTablePtr pal = palFile->getPalette();
	return TilesetPtr(new Zone66Tileset(psTileset, fnTruncate, pal));
}

TilesetPtr Zone66TilesetType::open(iostream_sptr psTileset,
	FN_TRUNCATE fnTruncate, MP_SUPPDATA& suppData) const
	throw (std::ios::failure)
{
	ImagePtr palFile(new VGAPalette(
		suppData[SuppItem::Palette].stream,
		suppData[SuppItem::Palette].fnTruncate
	));
	PaletteTablePtr pal = palFile->getPalette();
	return TilesetPtr(new Zone66Tileset(psTileset, fnTruncate, pal));
}

MP_SUPPLIST Zone66TilesetType::getRequiredSupps(
	const std::string& filenameTileset) const
	throw ()
{
	MP_SUPPLIST supps;
	supps[SuppItem::Palette] = "tpal.z66"; // TODO: case sensitivity?
	return supps;
}


Zone66Tileset::Zone66Tileset(iostream_sptr data,
	FN_TRUNCATE fnTruncate, PaletteTablePtr pal)
	throw (std::ios::failure) :
		FATTileset(data, fnTruncate, Z66_FIRST_TILE_OFFSET),
		pal(pal)
{
	this->data->seekg(0, std::ios::end);
	io::stream_offset len = this->data->tellg();

	// We still have to perform sanity checks in case the user forced an
	// open even though it failed the signature check.
	if (len < Z66_FIRST_TILE_OFFSET) throw std::ios::failure("file too short");

	this->data->seekg(0, std::ios::beg);
	uint32_t numTiles;
	this->data >> u32le(numTiles);
	this->items.reserve(numTiles);
	if (numTiles > Z66_SAFETY_MAX_TILES) throw std::ios::failure("too many tiles");

	uint32_t firstOffset = (numTiles+1) * 4;
	uint32_t nextOffset;
	this->data >> u32le(nextOffset);
	nextOffset += firstOffset;
	for (int i = 0; i < numTiles; i++) {
		FATEntry *fat = new FATEntry();
		EntryPtr ep(fat);
		fat->isValid = true;
		fat->attr = None;
		fat->index = i;
		fat->offset = nextOffset;
		fat->lenHeader = 0;
		if (i + 1 == numTiles) {
			// Last entry ends at EOF
			nextOffset = len;
		} else {
			this->data >> u32le(nextOffset);
			nextOffset += firstOffset;
		}
		fat->size = nextOffset - fat->offset;
		this->items.push_back(ep);
	}
}

Zone66Tileset::~Zone66Tileset()
	throw ()
{
}

int Zone66Tileset::getCaps()
	throw ()
{
	return HasPalette;
}

ImagePtr Zone66Tileset::createImageInstance(const EntryPtr& id,
	iostream_sptr content, FN_TRUNCATE fnTruncate)
	throw (std::ios::failure)
{
	ImagePtr img(new Zone66TileImage(content, fnTruncate, this->pal));
	return img;
}

PaletteTablePtr Zone66Tileset::getPalette()
	throw ()
{
	return this->pal;
}

void Zone66Tileset::setPalette(PaletteTablePtr newPalette)
	throw (std::ios::failure)
{
	// This doesn't save anything to the file as the palette is stored externally.
	this->pal = newPalette;
	return;
}

void Zone66Tileset::updateFileOffset(const FATEntry *pid,
	std::streamsize offDelta)
	throw (std::ios::failure)
{
	this->data->seekg((1+pid->index) * 4, std::ios::beg);
	this->data << u32le(pid->offset + offDelta);
	return;
}

Zone66Tileset::FATEntry *Zone66Tileset::preInsertFile(
	const Zone66Tileset::FATEntry *idBeforeThis, Zone66Tileset::FATEntry *pNewEntry)
	throw (std::ios::failure)
{
	// TODO: make room in the FAT
	return pNewEntry;
}

} // namespace gamegraphics
} // namespace camoto
