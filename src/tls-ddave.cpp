/**
 * @file  tls-ddave.cpp
 * @brief Dangerous Dave tileset handler.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Dangerous_Dave_Graphics_Format
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
#include <camoto/stream_filtered.hpp>
#include "filter-pad.hpp"
#include "img-ega-rowplanar.hpp"
#include "tls-ddave.hpp"
#include "img-ddave.hpp"
#include "pal-vga-raw.hpp"

/// Offset of the number of tilesets
#define DD_TILECOUNT_OFFSET    0

/// Offset where the FAT starts
#define DD_FAT_OFFSET          4

/// Length of each entry in the FAT (a uint32 offset)
#define DD_FAT_ENTRY_LEN       4

/// Offset of first tile in an empty tileset
#define DD_FIRST_TILE_OFFSET   4

/// Maximum tiles to load in case of a corrupted file
#define DD_SAFETY_MAX_TILES    4096

/// First tile with width and height fields
#define DD_FIRST_TILE_WITH_DIMS  53

/// Insert/remove a padding byte after every block of this size
#define DD_PAD_BLOCK 65536

namespace camoto {
namespace gamegraphics {

TilesetType_DDave::TilesetType_DDave()
{
}

TilesetType_DDave::~TilesetType_DDave()
{
}

std::vector<std::string> TilesetType_DDave::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("dav");
	return vcExtensions;
}

std::vector<std::string> TilesetType_DDave::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Dangerous Dave");
	return vcGames;
}

TilesetType_DDave::Certainty TilesetType_DDave::isInstance(
	stream::input_sptr psTileset) const
{
	stream::pos len = psTileset->size();
	// TESTED BY: TODO
	if (len < DD_FIRST_TILE_OFFSET) return DefinitelyNo; // too short

	psTileset->seekg(0, stream::start);
	uint32_t numFiles;
	psTileset >> u32le(numFiles);

	if ((numFiles == 0) && (len > DD_FIRST_TILE_OFFSET)) return DefinitelyNo; // invalid empty file

	uint32_t offset, lastOffset = 0;
	uint32_t firstOffset;
	uint32_t secondOffset = len; // default to EOF as second file's offset
	for (unsigned int i = 0; i < numFiles; i++) {
		psTileset >> u32le(offset);
		if (i == 0) firstOffset = offset;
		else if (i == 1) secondOffset = offset;

		// Make sure the offsets are increasing, otherwise we'd get a negative
		// file size (or the file has been tweaked to make opening difficult, but
		// then there's the -f option to gametls for that.)
		// TESTED BY: TODO
		if (offset < lastOffset) return DefinitelyNo;

		// Make sure the tile is contained within the file
		// TESTED BY: TODO
		if (offset > len) return DefinitelyNo;

		lastOffset = offset;
	}

	// Check the size of the first tile to see what format it's in.
	if (numFiles > 0) {
		// Call function in DDave[CEV]GATilesetType
		if (!this->isInstance(secondOffset - firstOffset)) return DefinitelyNo;
	}

	// TESTED BY: tls_ddave_isinstance_c00
	return DefinitelyYes;
}

SuppFilenames TilesetType_DDave::getRequiredSupps(
	const std::string& filenameTileset) const
{
	SuppFilenames supps;
	return supps;
}


std::string TilesetType_DDaveCGA::getCode() const
{
	return "tls-ddave-cga";
}

std::string TilesetType_DDaveCGA::getFriendlyName() const
{
	return "Dangerous Dave CGA tileset";
}

TilesetPtr TilesetType_DDaveCGA::create(stream::inout_sptr psTileset,
	SuppData& suppData) const
{
	psTileset->truncate(4);
	psTileset->seekp(0, stream::start);
	psTileset << u32le(0);

	PaletteTablePtr pal = createPalette_CGA(CGAPal_CyanMagentaBright);

	filter_sptr filtRead(new filter_unpad(1, DD_PAD_BLOCK));
	filter_sptr filtWrite(new filter_pad(std::string("\x00", 1), DD_PAD_BLOCK));
	stream::filtered_sptr decoded(new stream::filtered());
	decoded->open(psTileset, filtRead, filtWrite, NULL);

	return TilesetPtr(new Tileset_DDave(decoded, Tileset_DDave::CGA, pal));
}

TilesetPtr TilesetType_DDaveCGA::open(stream::inout_sptr psTileset,
	SuppData& suppData) const
{
	PaletteTablePtr pal = createPalette_CGA(CGAPal_CyanMagentaBright);

	filter_sptr filtRead(new filter_unpad(1, DD_PAD_BLOCK));
	filter_sptr filtWrite(new filter_pad(std::string("\x00", 1), DD_PAD_BLOCK));
	stream::filtered_sptr decoded(new stream::filtered());
	decoded->open(psTileset, filtRead, filtWrite, NULL);

	return TilesetPtr(new Tileset_DDave(decoded, Tileset_DDave::CGA, pal));
}

bool TilesetType_DDaveCGA::isInstance(int firstTileSize) const
{
	return (firstTileSize == 64);
}


std::string TilesetType_DDaveEGA::getCode() const
{
	return "tls-ddave-ega";
}

std::string TilesetType_DDaveEGA::getFriendlyName() const
{
	return "Dangerous Dave EGA tileset";
}

TilesetPtr TilesetType_DDaveEGA::create(stream::inout_sptr psTileset,
	SuppData& suppData) const
{
	psTileset->truncate(4);
	psTileset->seekp(0, stream::start);
	psTileset << u32le(0);

	filter_sptr filtRead(new filter_unpad(1, DD_PAD_BLOCK));
	filter_sptr filtWrite(new filter_pad(std::string("\x00", 1), DD_PAD_BLOCK));
	stream::filtered_sptr decoded(new stream::filtered());
	decoded->open(psTileset, filtRead, filtWrite, NULL);

	return TilesetPtr(new Tileset_DDave(decoded, Tileset_DDave::EGA, PaletteTablePtr()));
}

TilesetPtr TilesetType_DDaveEGA::open(stream::inout_sptr psTileset,
	SuppData& suppData) const
{
	filter_sptr filtRead(new filter_unpad(1, DD_PAD_BLOCK));
	filter_sptr filtWrite(new filter_pad(std::string("\x00", 1), DD_PAD_BLOCK));
	stream::filtered_sptr decoded(new stream::filtered());
	decoded->open(psTileset, filtRead, filtWrite, NULL);

	return TilesetPtr(new Tileset_DDave(decoded, Tileset_DDave::EGA, PaletteTablePtr()));
}

bool TilesetType_DDaveEGA::isInstance(int firstTileSize) const
{
	return (firstTileSize == 128);
}


std::string TilesetType_DDaveVGA::getCode() const
{
	return "tls-ddave-vga";
}

std::string TilesetType_DDaveVGA::getFriendlyName() const
{
	return "Dangerous Dave VGA tileset";
}

TilesetPtr TilesetType_DDaveVGA::create(stream::inout_sptr psTileset,
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

	filter_sptr filtRead(new filter_unpad(1, DD_PAD_BLOCK));
	filter_sptr filtWrite(new filter_pad(std::string("\x00", 1), DD_PAD_BLOCK));
	stream::filtered_sptr decoded(new stream::filtered());
	decoded->open(psTileset, filtRead, filtWrite, NULL);

	return TilesetPtr(new Tileset_DDave(decoded, Tileset_DDave::VGA, pal));
}

TilesetPtr TilesetType_DDaveVGA::open(stream::inout_sptr psTileset,
	SuppData& suppData) const
{
	PaletteTablePtr pal;
	// Only load the palette if one was given
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new Palette_VGA(suppData[SuppItem::Palette], 6));
		pal = palFile->getPalette();
	}

	filter_sptr filtRead(new filter_unpad(1, DD_PAD_BLOCK));
	filter_sptr filtWrite(new filter_pad(std::string("\x00", 1), DD_PAD_BLOCK));
	stream::filtered_sptr decoded(new stream::filtered());
	decoded->open(psTileset, filtRead, filtWrite, NULL);

	return TilesetPtr(new Tileset_DDave(decoded, Tileset_DDave::VGA, pal));
}

SuppFilenames TilesetType_DDaveVGA::getRequiredSupps(
	const std::string& filenameTileset) const
{
	SuppFilenames supps;
	supps[SuppItem::Palette] = "vga.pal";
	return supps;
}

bool TilesetType_DDaveVGA::isInstance(int firstTileSize) const
{
	return (firstTileSize == 256);
}


Tileset_DDave::Tileset_DDave(stream::inout_sptr data,
	ImageType imgType, PaletteTablePtr pal)
	:	Tileset_FAT(data, DD_FIRST_TILE_OFFSET),
		imgType(imgType),
		pal(pal)
{
	stream::pos len = this->data->size();

	// We still have to perform sanity checks in case the user forced an
	// open even though it failed the signature check.
	// TESTED BY: tls_ddave_vga_isinstance_c01
	if (len < DD_FIRST_TILE_OFFSET) throw stream::error("file too short");

	this->data->seekg(0, stream::start);
	uint32_t numTiles;
	this->data >> u32le(numTiles);
	this->items.reserve(numTiles);
	// TESTED BY: tls_ddave_vga_isinstance_c02
	if (numTiles > DD_SAFETY_MAX_TILES) throw stream::error("too many tiles");

	if (numTiles > 0) {
		uint32_t nextOffset;
		this->data >> u32le(nextOffset);
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
			}
			fat->size = nextOffset - fat->offset;
			this->items.push_back(ep);
		}
	}
}

Tileset_DDave::~Tileset_DDave()
{
}

int Tileset_DDave::getCaps()
{
	return 0
		| (this->pal ? Tileset::HasPalette : 0)
		| ((this->imgType == CGA) ? Tileset::ColourDepthCGA : 0)
		| ((this->imgType == EGA) ? Tileset::ColourDepthEGA : 0)
		| ((this->imgType == VGA) ? Tileset::ColourDepthVGA : 0)
	;
}

ImagePtr Tileset_DDave::createImageInstance(const EntryPtr& id,
	stream::inout_sptr content)
{
	FATEntry *fat = dynamic_cast<FATEntry *>(id.get());
	assert(fat);

	ImagePtr conv;
	bool fixedSize = fat->index < DD_FIRST_TILE_WITH_DIMS;
	switch (this->imgType) {
		case CGA:
			conv.reset(new Image_DDaveCGA(content, fixedSize));
			break;
		case EGA:
			conv.reset(new Image_DDaveEGA(content, fixedSize));
			break;
		case VGA: {
			conv.reset(new Image_DDaveVGA(content, fixedSize, this->pal));
			break;
		}
	}
	return conv;
}

PaletteTablePtr Tileset_DDave::getPalette()
{
	return this->pal;
}

void Tileset_DDave::updateFileOffset(const FATEntry *pid,
	stream::len offDelta)
{
	this->data->seekg(DD_FAT_OFFSET + pid->index * DD_FAT_ENTRY_LEN, stream::start);
	this->data << u32le(pid->offset);
	return;
}

Tileset_DDave::FATEntry *Tileset_DDave::preInsertFile(
	const Tileset_DDave::FATEntry *idBeforeThis, Tileset_DDave::FATEntry *pNewEntry)
{
	pNewEntry->lenHeader = 0;

	this->data->seekp(DD_FAT_OFFSET + pNewEntry->index * DD_FAT_ENTRY_LEN, stream::start);
	this->data->insert(DD_FAT_ENTRY_LEN);

	// Because the new entry isn't in the vector yet we need to shift it manually
	pNewEntry->offset += DD_FAT_ENTRY_LEN;

	// Write the new offset
	this->data << u32le(pNewEntry->offset);

	// Update the offsets now there's a new FAT entry taking up space.
	this->shiftFiles(
		NULL,
		DD_FAT_OFFSET + this->items.size() * DD_FAT_ENTRY_LEN,
		DD_FAT_ENTRY_LEN,
		0
	);

	return pNewEntry;
}

void Tileset_DDave::postInsertFile(FATEntry *pNewEntry)
{
	this->updateFileCount(this->items.size());
	return;
}

void Tileset_DDave::postRemoveFile(const FATEntry *pid)
{
	// Update the offsets now there's one less FAT entry taking up space.  This
	// must be called before the FAT is altered, because it will write a new
	// offset into the FAT entry we're about to erase (and if we erase it first
	// it'll overwrite something else.)
	this->shiftFiles(
		NULL,
		DD_FAT_OFFSET + this->items.size() * DD_FAT_ENTRY_LEN,
		-DD_FAT_ENTRY_LEN,
		0
	);

	// Remove the last FAT entry now it is no longer in use
	//this->data->seekp(DD_FAT_OFFSET + pid->index * DD_FAT_ENTRY_LEN, stream::start);
	this->data->seekp(DD_FAT_OFFSET + this->items.size() * DD_FAT_ENTRY_LEN,
		stream::start);
	this->data->remove(DD_FAT_ENTRY_LEN);

	this->updateFileCount(this->items.size());
	return;
}

void Tileset_DDave::updateFileCount(uint32_t newCount)
{
	this->data->seekp(DD_TILECOUNT_OFFSET, stream::start);
	this->data << u32le(newCount);
	return;
}


} // namespace gamegraphics
} // namespace camoto
