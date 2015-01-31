/**
 * @file  tls-zone66.cpp
 * @brief Zone 66 tileset handler.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Zone_66_Tileset_Format
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
#include "img-zone66_tile.hpp"
#include "tls-zone66.hpp"

/// Offset of the number of tilesets
#define Z66_TILECOUNT_OFFSET   0

/// Offset where the FAT starts
#define Z66_FAT_OFFSET         4

/// Length of each entry in the FAT (a uint32 offset)
#define Z66_FAT_ENTRY_LEN      4

/// Offset of first tile in an empty tileset
#define Z66_FIRST_TILE_OFFSET  4

/// Maximum tiles to load in case of a corrupted file
#define Z66_SAFETY_MAX_TILES   4096

namespace camoto {
namespace gamegraphics {

TilesetType_Zone66::TilesetType_Zone66()
{
}

TilesetType_Zone66::~TilesetType_Zone66()
{
}

std::string TilesetType_Zone66::getCode() const
{
	return "tls-zone66";
}

std::string TilesetType_Zone66::getFriendlyName() const
{
	return "Zone 66 tileset";
}

std::vector<std::string> TilesetType_Zone66::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("z66");
	return vcExtensions;
}

std::vector<std::string> TilesetType_Zone66::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Zone 66");
	return vcGames;
}

TilesetType_Zone66::Certainty TilesetType_Zone66::isInstance(
	stream::input_sptr psTileset) const
{
	stream::pos len = psTileset->size();
	// TESTED BY: tls_zone66_isinstance_c04
	if (len < Z66_FIRST_TILE_OFFSET) return DefinitelyNo; // too short

	psTileset->seekg(0, stream::start);
	uint32_t numFiles;
	psTileset >> u32le(numFiles);

	if ((numFiles == 0) && (len > 8)) return DefinitelyNo; // invalid empty file

	uint32_t offset, lastOffset = 0;
	for (unsigned int i = 0; i < numFiles; i++) {
		psTileset >> u32le(offset);

		// The first file always starts at offset 0.
		// TESTED BY: tls_zone66_isinstance_c01
		if ((i == 0) && (offset != 0)) return DefinitelyNo;

		// Make sure the offsets are increasing, otherwise we'd get a negative
		// file size (or the file has been tweaked to make opening difficult, but
		// then there's the -f option to gametls for that.)
		// TESTED BY: tls_zone66_isinstance_c02
		if (offset < lastOffset) return DefinitelyNo;

		// Make sure the tile is contained within the file
		// TESTED BY: tls_zone66_isinstance_c03
		if ((numFiles+1) * 4 + offset > len) return DefinitelyNo;

		lastOffset = offset;
	}

	// TESTED BY: tls_zone66_isinstance_c00
	return DefinitelyYes;
}

TilesetPtr TilesetType_Zone66::create(stream::inout_sptr psTileset,
	SuppData& suppData) const
{
	psTileset->truncate(4);
	psTileset->seekp(0, stream::start);
	psTileset << u32le(0);

	PaletteTablePtr pal;
	// Only load the palette if one was given
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new Palette_VGA(suppData[SuppItem::Palette], 6));
		pal = palFile->getPalette();
	}
	return TilesetPtr(new Tileset_Zone66(psTileset, pal));
}

TilesetPtr TilesetType_Zone66::open(stream::inout_sptr psTileset,
	SuppData& suppData) const
{
	PaletteTablePtr pal;
	// Only load the palette if one was given
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new Palette_VGA(suppData[SuppItem::Palette], 6));
		pal = palFile->getPalette();
	}
	return TilesetPtr(new Tileset_Zone66(psTileset, pal));
}

SuppFilenames TilesetType_Zone66::getRequiredSupps(
	const std::string& filenameTileset) const
{
	SuppFilenames supps;
	supps[SuppItem::Palette] = "mpal.z66";
	return supps;
}


Tileset_Zone66::Tileset_Zone66(stream::inout_sptr data,
	PaletteTablePtr pal)
	:	Tileset_FAT(data, Z66_FIRST_TILE_OFFSET),
		pal(pal)
{
	stream::pos len = this->data->size();

	// We still have to perform sanity checks in case the user forced an
	// open even though it failed the signature check.
	if (len < Z66_FIRST_TILE_OFFSET) throw stream::error("file too short");

	this->data->seekg(0, stream::start);
	uint32_t numTiles;
	this->data >> u32le(numTiles);
	this->items.reserve(numTiles);
	if (numTiles > Z66_SAFETY_MAX_TILES) throw stream::error("too many tiles");

	if (numTiles > 0) {
		uint32_t firstOffset = (numTiles+1) * 4;
		uint32_t nextOffset;
		this->data >> u32le(nextOffset);
		nextOffset += firstOffset;
		for (unsigned int i = 0; i < numTiles; i++) {
			FATEntry *fat = new FATEntry();
			EntryPtr ep(fat);
			fat->valid = true;
			fat->attr = 0;
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
}

Tileset_Zone66::~Tileset_Zone66()
{
}

int Tileset_Zone66::getCaps()
{
	return Tileset::HasPalette | Tileset::ColourDepthVGA;
}

ImagePtr Tileset_Zone66::createImageInstance(const EntryPtr& id,
	stream::inout_sptr content)
{
	ImagePtr img(new Image_Zone66Tile(content, this->pal));
	return img;
}

PaletteTablePtr Tileset_Zone66::getPalette()
{
	return this->pal;
}

void Tileset_Zone66::setPalette(PaletteTablePtr newPalette)
{
	// This doesn't save anything to the file as the palette is stored externally.
	this->pal = newPalette;
	return;
}

void Tileset_Zone66::updateFileOffset(const FATEntry *pid,
	stream::len offDelta)
{
	uint32_t fatSize = Z66_FAT_OFFSET + this->items.size() * Z66_FAT_ENTRY_LEN;

	// Because offsets are stored from the end of the FAT (i.e. the first entry
	// will always say offset 0) we need to adjust the value we will be writing.
	uint32_t fatOffset = pid->offset - fatSize;

	this->data->seekg(Z66_FAT_OFFSET + pid->index * Z66_FAT_ENTRY_LEN, stream::start);
	this->data << u32le(fatOffset);
	return;
}

Tileset_Zone66::FATEntry *Tileset_Zone66::preInsertFile(
	const Tileset_Zone66::FATEntry *idBeforeThis, Tileset_Zone66::FATEntry *pNewEntry)
{
	uint32_t fatSize = Z66_FAT_OFFSET + this->items.size() * Z66_FAT_ENTRY_LEN;

	// Because offsets are stored from the end of the FAT (i.e. the first entry
	// will always say offset 0) we need to adjust the value we will be writing.
	//uint32_t fatOffset = pNewEntry->offset - fatSize;

	this->data->seekp(Z66_FAT_OFFSET + pNewEntry->index * Z66_FAT_ENTRY_LEN, stream::start);
	this->data->insert(Z66_FAT_ENTRY_LEN);
	//this->data << u32le(fatOffset);
	// No need to write the offset now as it will be wrong, and will be updated
	// in postInsertFile() anyway.

	// Update the offsets now there's a new FAT entry taking up space.  Although
	// no offsets will change (because they're counting from the end of the FAT -
	// the first entry will always have an offset of 0), we still need to update
	// the in-memory offsets, which *will* change as they count from the start of
	// the file.
	this->shiftFiles(
		NULL,
		fatSize,
		Z66_FAT_ENTRY_LEN,
		0
	);
	// Because the new entry isn't in the vector yet we need to shift it manually
	pNewEntry->offset += Z66_FAT_ENTRY_LEN;

	return pNewEntry;
}

void Tileset_Zone66::postInsertFile(FATEntry *pNewEntry)
{
	// Now the FAT vector has been updated, recalculate the file offsets so they
	// are correct (i.e. entry 0 is still at offset 0).
	this->shiftFiles(NULL, 0, 0, 0);
	this->updateFileCount(this->items.size());
	return;
}

void Tileset_Zone66::postRemoveFile(const FATEntry *pid)
{
	// Update the offsets now there's one less FAT entry taking up space.  This
	// must be called before the FAT is altered, because it will write a new
	// offset into the FAT entry we're about to erase (and if we erase it first
	// it'll overwrite something else.)
	this->shiftFiles(
		NULL,
		Z66_FAT_OFFSET + this->items.size() * Z66_FAT_ENTRY_LEN,
		-Z66_FAT_ENTRY_LEN,
		0
	);

	// Remove the last FAT entry now it is no longer in use
	//this->data->seekp(Z66_FAT_OFFSET + pid->index * Z66_FAT_ENTRY_LEN, stream::start);
	this->data->seekp(Z66_FAT_OFFSET + this->items.size() * Z66_FAT_ENTRY_LEN,
		stream::start);
	this->data->remove(Z66_FAT_ENTRY_LEN);

	this->updateFileCount(this->items.size());
	return;
}

void Tileset_Zone66::updateFileCount(uint32_t newCount)
{
	this->data->seekp(Z66_TILECOUNT_OFFSET, stream::start);
	this->data << u32le(newCount);
	return;
}


} // namespace gamegraphics
} // namespace camoto
