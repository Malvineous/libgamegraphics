/**
 * @file  tls-ccaves-main.cpp
 * @brief Crystal Caves top-level tileset handler (containing sub tilesets)
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Crystal_Caves_Tileset_Format
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

#include <cassert>
#include <camoto/iostream_helpers.hpp>
#include <camoto/util.hpp> // make_unique
#include "tls-ccaves-main.hpp"
#include "tls-ccaves-sub.hpp"

namespace camoto {
namespace gamegraphics {

/// How much padding to apply after each tileset (in bytes)
#define CC_PAD  0

/// Maximum number of tilesets to load from a tileset collection.
#define CC_SAFETY_MAX_TILESETCOUNT  1024

/// Offset of first tileset in a tileset collection.
#define CC_FIRST_TILESET_OFFSET 0

/// Offset of the first tile within a single tileset.
#define CC_FIRST_TILE_OFFSET    3

#define FILETYPE_CCAVES_MAIN "tileset/ccaves-sub"

//
// TilesetType_CCavesMain
//

TilesetType_CCavesMain::TilesetType_CCavesMain()
	: pad(CC_PAD)
{
}

TilesetType_CCavesMain::~TilesetType_CCavesMain()
{
}

std::string TilesetType_CCavesMain::code() const
{
	return "tls-ccaves-main";
}

std::string TilesetType_CCavesMain::friendlyName() const
{
	return "Crystal Caves Concatenated Tileset";
}

std::vector<std::string> TilesetType_CCavesMain::fileExtensions() const
{
	return {"gfx"};
}

std::vector<std::string> TilesetType_CCavesMain::games() const
{
	return {
		"Crystal Caves",
	};
}

TilesetType_CCavesMain::Certainty TilesetType_CCavesMain::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();

	// Empty file
	// TESTED BY: tls_ccaves_main_isinstance_c01
	if (len == 0) return PossiblyYes;

	// Too short
	// TESTED BY: tls_ccaves_main_isinstance_c01
	if (len < 3) return DefinitelyNo; // too short

	content.seekg(0, stream::start);
	stream::pos pos = 0;
	while (pos + 3 < len) {
		uint8_t numTiles, width, height;
		content
			>> u8(numTiles)
			>> u8(width)
			>> u8(height)
		;
		pos += 3;

		int delta = width * height * (int)PlaneCount::Masked
			* numTiles + this->pad;

		// If this pushes us past EOF it's not a valid file
		pos += delta;
		if (pos > len) return DefinitelyNo;

		content.seekg(delta, stream::cur);
	}

	// TESTED BY: tls_ccaves_main_isinstance_c00
	return PossiblyYes;
}

std::shared_ptr<Tileset> TilesetType_CCavesMain::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->truncate(0);
	return this->open(std::move(content), suppData);
}

// Preconditions: isInstance() has returned > EC_DEFINITELY_NO
std::shared_ptr<Tileset> TilesetType_CCavesMain::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	return std::make_shared<Tileset_CCavesMain>(
		std::move(content), PlaneCount::Masked, this->pad
	);
}

SuppFilenames TilesetType_CCavesMain::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	return {};
}


//
// Tileset_CCavesMain
//

Tileset_CCavesMain::Tileset_CCavesMain(std::unique_ptr<stream::inout> content,
	PlaneCount numPlanes, stream::len pad)
	:	Tileset_FAT(std::move(content), CC_FIRST_TILESET_OFFSET, ARCH_NO_FILENAMES),
		numPlanes(numPlanes),
		pad(pad)
{
	stream::pos len = this->content->size();

	// Empty tileset
	if (len == 0) return;

	// We still have to perform sanity checks in case the user forced an
	// open even though it failed the signature check.
	if (len < 3) throw stream::error("file too short");

	this->content->seekg(0, stream::start);

	// Peek at all the "files" in the container to work out how large each one is
	stream::pos pos = 0;
	for (int i = 0; pos < len; i++) {
		uint8_t numTiles, widthBytes, height;
		*this->content
			>> u8(numTiles)
			>> u8(widthBytes)
			>> u8(height)
		;
		auto fat = std::make_unique<FATEntry>();
		fat->bValid = true;
		fat->fAttr = File::Attribute::Folder;
		fat->iIndex = i;
		fat->iOffset = pos;
		fat->storedSize = widthBytes * height * (unsigned int)this->numPlanes
			* numTiles + 3 + pad;
		fat->lenHeader = 0;

		auto fat2 = fat.get();
		this->vcFAT.push_back(std::move(fat));

		// Make sure this tileset won't go past EOF or is zero data
		if (pos + fat2->storedSize > len) break;

		this->content->seekg(fat2->storedSize-3, stream::cur);
		pos += fat2->storedSize;

		if (i >= CC_SAFETY_MAX_TILESETCOUNT) {
			throw stream::error("too many tilesets or corrupted graphics file");
		}
	}
}

Tileset_CCavesMain::~Tileset_CCavesMain()
{
}

Tileset::Caps Tileset_CCavesMain::caps() const
{
	return Caps::Default;
}

ColourDepth Tileset_CCavesMain::colourDepth() const
{
	return ColourDepth::EGA;
}

unsigned int Tileset_CCavesMain::layoutWidth() const
{
	return 1;
}

void Tileset_CCavesMain::resize(const FileHandle& id, stream::len newStoredSize,
	stream::len newRealSize)
{
	auto fat = FATEntry::cast(id);
	assert(fat);

	// Make sure the new size is permitted
	uint8_t numTiles, widthBytes, height;
	if (this->content->size() >= fat->iOffset + 3) {
		this->content->seekg(fat->iOffset, stream::start);
		*this->content
			>> u8(numTiles)
			>> u8(widthBytes)
			>> u8(height)
		;
	} else {
		// Temporarily assign one tile the exact size needed
		auto space = (newStoredSize - 3) / (unsigned int)this->numPlanes;
		numTiles = 1;
		widthBytes = space / 8;
		height = space / widthBytes;
	}
	unsigned int tileSize = widthBytes * height * (unsigned int)this->numPlanes;
	if (((newStoredSize - 3) % tileSize) != 0) {
		throw stream::error("tls-ccaves-main sub-tilesets can only be resized in "
			"units of whole tiles only.");
	}

	this->Tileset_FAT::resize(id, newStoredSize, newRealSize);

	// Write the new tile count, so the tileset won't be corrupted in case nothing
	// gets written to this new file.
	unsigned int newNumTiles = (newStoredSize - 3) / tileSize;
	this->content->seekp(fat->iOffset, stream::start);
	*this->content
		<< u8(newNumTiles)
		<< u8(widthBytes)
		<< u8(height)
	;
	return;
}

std::shared_ptr<Tileset> Tileset_CCavesMain::openTileset(const FileHandle& id)
{
	return std::make_unique<Tileset_CCavesSub>(
		this->open(id, true), this->numPlanes
	);
}

const Tileset::FileHandle Tileset_CCavesMain::insert(const FileHandle& idBeforeThis,
	File::Attribute attr)
{
	return this->insert(idBeforeThis, "", 3, FILETYPE_CCAVES_MAIN, attr);
}

} // namespace gamegraphics
} // namespace camoto
