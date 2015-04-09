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
#include <camoto/util.hpp> // make_unique
#include "tls-ddave.hpp"
#include "filter-block-pad.hpp"
#include "img-ddave.hpp"
#include "pal-vga-raw.hpp"
#include "tileset-fat.hpp"

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

class Tileset_DDave: public Tileset_FAT
{
	public:
		Tileset_DDave(std::unique_ptr<stream::inout> content, ColourDepth depth,
			std::shared_ptr<const Palette> pal);
		virtual ~Tileset_DDave();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual Point dimensions() const;
		virtual unsigned int layoutWidth() const;
		virtual std::unique_ptr<Image> openImage(FileHandle& id);
		virtual void updateFileOffset(const FATEntry *pid, stream::delta offDelta);
		virtual void preInsertFile(const FATEntry *idBeforeThis,
			FATEntry *pNewEntry);
		virtual void postInsertFile(FATEntry *pNewEntry);
		virtual void postRemoveFile(const FATEntry *pid);

	private:
		ColourDepth depth;

		/// Update the number of tiles in the tileset
		void updateFileCount(uint32_t newCount);
};

TilesetType_DDave::TilesetType_DDave()
{
}

TilesetType_DDave::~TilesetType_DDave()
{
}

std::vector<std::string> TilesetType_DDave::fileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("dav");
	return vcExtensions;
}

std::vector<std::string> TilesetType_DDave::games() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Dangerous Dave");
	return vcGames;
}

TilesetType::Certainty TilesetType_DDave::isInstance(stream::input& content)
	const
{
	stream::pos len = content.size();
	// TESTED BY: TODO
	if (len < DD_FIRST_TILE_OFFSET) return DefinitelyNo; // too short

	content.seekg(0, stream::start);
	uint32_t numFiles;
	content >> u32le(numFiles);

	if ((numFiles == 0) && (len > DD_FIRST_TILE_OFFSET)) return DefinitelyNo; // invalid empty file

	uint32_t offset, lastOffset = 0;
	uint32_t firstOffset;
	uint32_t secondOffset = len; // default to EOF as second file's offset
	for (unsigned int i = 0; i < numFiles; i++) {
		content >> u32le(offset);
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

std::shared_ptr<Tileset> TilesetType_DDave::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->truncate(4);
	content->seekp(0, stream::start);
	*content << u32le(0);

	return this->open(std::move(content), suppData);
}

std::shared_ptr<Tileset> TilesetType_DDave::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	auto content_filtered = std::make_unique<stream::filtered>(
		std::move(content),
		std::make_shared<filter_block_unpad>(1, DD_PAD_BLOCK),
		std::make_shared<filter_block_pad>(std::string(1, '\x00'), DD_PAD_BLOCK),
		nullptr
	);

	std::shared_ptr<const Palette> pal;
	// Only load the palette if one was given
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		auto imgPal = std::make_unique<Palette_VGA>(
			std::move(suppData[SuppItem::Palette]), 6);
		pal = imgPal->palette();
	}

	return std::make_shared<Tileset_DDave>(
		std::move(content_filtered),
		this->colourDepth(),
		pal
	);
}

SuppFilenames TilesetType_DDave::getRequiredSupps(
	const std::string& filenameTileset) const
{
	return {};
}


std::string TilesetType_DDaveCGA::code() const
{
	return "tls-ddave-cga";
}

std::string TilesetType_DDaveCGA::friendlyName() const
{
	return "Dangerous Dave CGA tileset";
}

bool TilesetType_DDaveCGA::isInstance(int firstTileSize) const
{
	return (firstTileSize == 64);
}

ColourDepth TilesetType_DDaveCGA::colourDepth() const
{
	return ColourDepth::CGA;
}


std::string TilesetType_DDaveEGA::code() const
{
	return "tls-ddave-ega";
}

std::string TilesetType_DDaveEGA::friendlyName() const
{
	return "Dangerous Dave EGA tileset";
}

bool TilesetType_DDaveEGA::isInstance(int firstTileSize) const
{
	return (firstTileSize == 128);
}

ColourDepth TilesetType_DDaveEGA::colourDepth() const
{
	return ColourDepth::EGA;
}


std::string TilesetType_DDaveVGA::code() const
{
	return "tls-ddave-vga";
}

std::string TilesetType_DDaveVGA::friendlyName() const
{
	return "Dangerous Dave VGA tileset";
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

ColourDepth TilesetType_DDaveVGA::colourDepth() const
{
	return ColourDepth::VGA;
}


Tileset_DDave::Tileset_DDave(std::unique_ptr<stream::inout> content,
	ColourDepth depth, std::shared_ptr<const Palette> pal)
	:	Archive_FAT(std::move(content), DD_FIRST_TILE_OFFSET, ARCH_NO_FILENAMES),
		//Tileset_FAT(content, DD_FIRST_TILE_OFFSET),
		depth(depth)
{
	this->pal = pal;
	stream::pos len = this->content->size();

	// We still have to perform sanity checks in case the user forced an
	// open even though it failed the signature check.
	// TESTED BY: tls_ddave_vga_isinstance_c01
	if (len < DD_FIRST_TILE_OFFSET) throw stream::error("file too short");

	this->content->seekg(0, stream::start);
	uint32_t numTiles;
	*this->content >> u32le(numTiles);
	this->vcFAT.reserve(numTiles);
	// TESTED BY: tls_ddave_vga_isinstance_c02
	if (numTiles > DD_SAFETY_MAX_TILES) throw stream::error("too many tiles");

	if (numTiles > 0) {
		uint32_t nextOffset;
		*this->content >> u32le(nextOffset);
		for (unsigned int i = 0; i < numTiles; i++) {
			FATEntry *fat = new FATEntry();
			FileHandle ep(fat);
			fat->bValid = true;
			fat->fAttr = File::Attribute::Default;
			fat->iIndex = i;
			fat->iOffset = nextOffset;
			fat->type = "tile/ddave";
			switch (depth) {
				case ColourDepth::CGA: fat->type += "-cga"; break;
				case ColourDepth::EGA: fat->type += "-ega"; break;
				case ColourDepth::VGA: fat->type += "-vga"; break;
				default: break;
			}
			fat->lenHeader = 0;
			if (i + 1 == numTiles) {
				// Last entry ends at EOF
				nextOffset = len;
			} else {
				*this->content >> u32le(nextOffset);
			}
			fat->storedSize = nextOffset - fat->iOffset;
			fat->realSize = fat->storedSize;
#warning Real size should be smaller than storedSize because of the removed padding byte
			this->vcFAT.push_back(ep);
		}
	}
}

Tileset_DDave::~Tileset_DDave()
{
}

Tileset::Caps Tileset_DDave::caps() const
{
	return (this->pal ? Tileset::Caps::HasPalette : Tileset::Caps::Default);
}

ColourDepth Tileset_DDave::colourDepth() const
{
	return this->depth;
}

Point Tileset_DDave::dimensions() const
{
	return {0, 0};
}

unsigned int Tileset_DDave::layoutWidth() const
{
	return 4;
}

std::unique_ptr<Image> Tileset_DDave::openImage(FileHandle& id)
{
	auto fat = FATEntry::cast(id);
	bool fixedSize = fat->iIndex < DD_FIRST_TILE_WITH_DIMS;
	switch (this->depth) {

		case ColourDepth::CGA:
			return std::make_unique<Image_DDaveCGA>(
				this->open(id, true), fixedSize
			);

		case ColourDepth::EGA:
			return std::make_unique<Image_DDaveEGA>(
				this->open(id, true), fixedSize
			);

		case ColourDepth::VGA:
			return std::make_unique<Image_DDaveVGA>(
				this->open(id, true), fixedSize, this->pal
			);

		default:
			break;
	}
	throw stream::error("Unknown colour depth for DDave tileset (this "
		"shouldn't happen!)");
}

void Tileset_DDave::updateFileOffset(const FATEntry *pid, stream::delta offDelta)
{
	this->content->seekg(DD_FAT_OFFSET + pid->iIndex * DD_FAT_ENTRY_LEN, stream::start);
	*this->content << u32le(pid->iOffset);
	return;
}

void Tileset_DDave::preInsertFile(const FATEntry *idBeforeThis,
	FATEntry *pNewEntry)
{
	pNewEntry->lenHeader = 0;

	this->content->seekp(DD_FAT_OFFSET + pNewEntry->iIndex * DD_FAT_ENTRY_LEN, stream::start);
	this->content->insert(DD_FAT_ENTRY_LEN);

	// Because the new entry isn't in the vector yet we need to shift it manually
	pNewEntry->iOffset += DD_FAT_ENTRY_LEN;

	// Write the new offset
	*this->content << u32le(pNewEntry->iOffset);

	// Update the offsets now there's a new FAT entry taking up space.
	this->shiftFiles(
		NULL,
		DD_FAT_OFFSET + this->vcFAT.size() * DD_FAT_ENTRY_LEN,
		DD_FAT_ENTRY_LEN,
		0
	);

	return;
}

void Tileset_DDave::postInsertFile(FATEntry *pNewEntry)
{
	this->updateFileCount(this->vcFAT.size());
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
		DD_FAT_OFFSET + this->vcFAT.size() * DD_FAT_ENTRY_LEN,
		-DD_FAT_ENTRY_LEN,
		0
	);

	// Remove the last FAT entry now it is no longer in use
	//this->content->seekp(DD_FAT_OFFSET + pid->index * DD_FAT_ENTRY_LEN, stream::start);
	this->content->seekp(DD_FAT_OFFSET + this->vcFAT.size() * DD_FAT_ENTRY_LEN,
		stream::start);
	this->content->remove(DD_FAT_ENTRY_LEN);

	this->updateFileCount(this->vcFAT.size());
	return;
}

void Tileset_DDave::updateFileCount(uint32_t newCount)
{
	this->content->seekp(DD_TILECOUNT_OFFSET, stream::start);
	*this->content << u32le(newCount);
	return;
}


} // namespace gamegraphics
} // namespace camoto
