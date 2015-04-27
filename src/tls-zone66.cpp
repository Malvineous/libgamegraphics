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
#include <camoto/util.hpp> // make_unique
#include "tileset-fat.hpp"
#include "pal-vga-raw.hpp"
#include "img-vga-raw.hpp"
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

/// Colour depth of VGA palette
#define Z66_PAL_DEPTH 6

/// Palette index to make transparent
#define Z66_TRANSPARENT_COLOUR 0

#define FILETYPE_ZONE66_FS "image/vga-raw-fullscreen"
#define FILETYPE_ZONE66_T  "tile/zone66"

namespace camoto {
namespace gamegraphics {

/// Tileset handler for Zone 66 tilesets.
class Tileset_Zone66: virtual public Tileset_FAT
{
	public:
		Tileset_Zone66(std::unique_ptr<stream::inout> content,
			std::shared_ptr<const Palette> pal);
		virtual ~Tileset_Zone66();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual unsigned int layoutWidth() const;

		// Tileset_FAT
		virtual std::unique_ptr<Image> openImage(FileHandle& id);
		virtual void updateFileOffset(const FATEntry *pid, stream::delta offDelta);
		virtual void preInsertFile(const FATEntry *idBeforeThis,
			FATEntry *pNewEntry);
		virtual void postInsertFile(FATEntry *pNewEntry);
		virtual void postRemoveFile(const FATEntry *pid);

	private:
		/// Update the number of tiles in the tileset
		void updateFileCount(uint32_t newCount);
};


TilesetType_Zone66::TilesetType_Zone66()
{
}

TilesetType_Zone66::~TilesetType_Zone66()
{
}

std::string TilesetType_Zone66::code() const
{
	return "tls-zone66";
}

std::string TilesetType_Zone66::friendlyName() const
{
	return "Zone 66 tileset";
}

std::vector<std::string> TilesetType_Zone66::fileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("z66");
	return vcExtensions;
}

std::vector<std::string> TilesetType_Zone66::games() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Zone 66");
	return vcGames;
}

TilesetType::Certainty TilesetType_Zone66::isInstance(stream::input& content)
	const
{
	stream::pos len = content.size();

	// Too short
	// TESTED BY: tls_zone66_isinstance_c01
	if (len < Z66_FIRST_TILE_OFFSET) return DefinitelyNo;

	content.seekg(0, stream::start);
	uint32_t numFiles;
	content >> u32le(numFiles);

	// Invalid empty tileset
	// TESTED BY: tls_zone66_isinstance_c02
	if ((numFiles == 0) && (len > 8)) return DefinitelyNo;

	uint32_t offset, lastOffset = 0;
	for (unsigned int i = 0; i < numFiles; i++) {
		content >> u32le(offset);

		// The first file always starts at offset 0.
		// TESTED BY: tls_zone66_isinstance_c03
		if ((i == 0) && (offset != 0)) return DefinitelyNo;

		// Make sure the offsets are increasing, otherwise we'd get a negative
		// file size (or the file has been tweaked to make opening difficult, but
		// then there's the -f option to gametls for that.)
		// TESTED BY: tls_zone66_isinstance_c04
		if (offset < lastOffset) return DefinitelyNo;

		// Make sure the tile is contained within the file
		// TESTED BY: tls_zone66_isinstance_c05
		if ((numFiles+1) * 4 + offset > len) return DefinitelyNo;

		lastOffset = offset;
	}

	// TESTED BY: tls_zone66_isinstance_c00
	return DefinitelyYes;
}

std::shared_ptr<Tileset> TilesetType_Zone66::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->truncate(4);
	content->seekp(0, stream::start);
	*content << u32le(0);

	return this->open(std::move(content), suppData);
}

std::shared_ptr<Tileset> TilesetType_Zone66::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	std::shared_ptr<Palette> pal;
	// Only load the palette if one was given
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		auto palFile = std::make_unique<Palette_VGA>(
			std::move(suppData[SuppItem::Palette]), Z66_PAL_DEPTH
		);
		pal = std::make_shared<Palette>();
		*pal = *palFile->palette(); // deep copy palette
		pal->at(Z66_TRANSPARENT_COLOUR).alpha = 0;
	}
	return std::make_shared<Tileset_Zone66>(std::move(content), pal);
}

SuppFilenames TilesetType_Zone66::getRequiredSupps(
	const std::string& filenameTileset) const
{
	SuppFilenames supps;
	supps[SuppItem::Palette] = "mpal.z66";
	return supps;
}


Tileset_Zone66::Tileset_Zone66(std::unique_ptr<stream::inout> content,
	std::shared_ptr<const Palette> pal)
	:	Tileset_FAT(std::move(content), Z66_FIRST_TILE_OFFSET, ARCH_NO_FILENAMES)
{
	this->pal = pal;
	stream::pos len = this->content->size();

	// We still have to perform sanity checks in case the user forced an
	// open even though it failed the signature check.
	if (len < Z66_FIRST_TILE_OFFSET) throw stream::error("file too short");

	this->content->seekg(0, stream::start);
	uint32_t numTiles;
	*this->content >> u32le(numTiles);
	this->vcFAT.reserve(numTiles);
	if (numTiles > Z66_SAFETY_MAX_TILES) throw stream::error("too many tiles");

	if (numTiles > 0) {
		uint32_t firstOffset = (numTiles+1) * 4;
		uint32_t nextOffset;
		*this->content >> u32le(nextOffset);
		nextOffset += firstOffset;
		for (unsigned int i = 0; i < numTiles; i++) {
			auto fat = std::make_unique<FATEntry>();
			fat->bValid = true;
			fat->fAttr = File::Attribute::Default;
			fat->iIndex = i;
			fat->iOffset = nextOffset;
			fat->lenHeader = 0;
			if (i + 1 == numTiles) {
				// Last entry ends at EOF
				nextOffset = len;
			} else {
				*this->content >> u32le(nextOffset);
				nextOffset += firstOffset;
			}
			fat->storedSize = nextOffset - fat->iOffset;
			fat->type = fat->storedSize == 64000 ? FILETYPE_ZONE66_FS : FILETYPE_ZONE66_T;
			this->vcFAT.push_back(std::move(fat));
		}
	}
}

Tileset_Zone66::~Tileset_Zone66()
{
}

Tileset::Caps Tileset_Zone66::caps() const
{
	return Tileset::Caps::HasPalette;
}

ColourDepth Tileset_Zone66::colourDepth() const
{
	return ColourDepth::VGA;
}

unsigned int Tileset_Zone66::layoutWidth() const
{
	return 16;
}

std::unique_ptr<Image> Tileset_Zone66::openImage(FileHandle& id)
{
	auto contentImage = this->open(id, true);
	if (contentImage->size() == 64000) {
		// This tile is raw-vga
		return std::make_unique<Image_VGARaw>(
			std::move(contentImage), Point{320, 200}, this->palette()
		);
	} else {
		// This tile is normal
		return std::make_unique<Image_Zone66Tile>(
			std::move(contentImage), this->palette()
		);
	}
}

void Tileset_Zone66::updateFileOffset(const FATEntry *pid,
	stream::delta offDelta)
{
	uint32_t fatSize = Z66_FAT_OFFSET + this->vcFAT.size() * Z66_FAT_ENTRY_LEN;

	// Because offsets are stored from the end of the FAT (i.e. the first entry
	// will always say offset 0) we need to adjust the value we will be writing.
	uint32_t fatOffset = pid->iOffset - fatSize;

	this->content->seekg(Z66_FAT_OFFSET + pid->iIndex * Z66_FAT_ENTRY_LEN, stream::start);
	*this->content << u32le(fatOffset);
	return;
}

void Tileset_Zone66::preInsertFile(const FATEntry *idBeforeThis,
	FATEntry *pNewEntry)
{
	uint32_t fatSize = Z66_FAT_OFFSET + this->vcFAT.size() * Z66_FAT_ENTRY_LEN;

	// Because offsets are stored from the end of the FAT (i.e. the first entry
	// will always say offset 0) we need to adjust the value we will be writing.
	//uint32_t fatOffset = pNewEntry->iOffset - fatSize;

	this->content->seekp(Z66_FAT_OFFSET + pNewEntry->iIndex * Z66_FAT_ENTRY_LEN, stream::start);
	this->content->insert(Z66_FAT_ENTRY_LEN);
	//this->content << u32le(fatOffset);
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
	pNewEntry->iOffset += Z66_FAT_ENTRY_LEN;

	return;
}

void Tileset_Zone66::postInsertFile(FATEntry *pNewEntry)
{
	// Now the FAT vector has been updated, recalculate the file offsets so they
	// are correct (i.e. entry 0 is still at offset 0).
	this->shiftFiles(NULL, 0, 0, 0);
	this->updateFileCount(this->vcFAT.size());
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
		Z66_FAT_OFFSET + this->vcFAT.size() * Z66_FAT_ENTRY_LEN,
		-Z66_FAT_ENTRY_LEN,
		0
	);

	// Remove the last FAT entry now it is no longer in use
	//this->content->seekp(Z66_FAT_OFFSET + pid->iIndex * Z66_FAT_ENTRY_LEN, stream::start);
	this->content->seekp(Z66_FAT_OFFSET + this->vcFAT.size() * Z66_FAT_ENTRY_LEN,
		stream::start);
	this->content->remove(Z66_FAT_ENTRY_LEN);

	this->updateFileCount(this->vcFAT.size());
	return;
}

void Tileset_Zone66::updateFileCount(uint32_t newCount)
{
	this->content->seekp(Z66_TILECOUNT_OFFSET, stream::start);
	*this->content << u32le(newCount);
	return;
}


} // namespace gamegraphics
} // namespace camoto
