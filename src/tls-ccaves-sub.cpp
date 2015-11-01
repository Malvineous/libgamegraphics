/**
 * @file  tls-ccaves-sub.cpp
 * @brief Crystal Caves sub tileset handler (containing images)
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

#include <camoto/iostream_helpers.hpp>
#include <camoto/util.hpp> // make_unique
#include "img-ega-byteplanar.hpp"
#include "tls-ccaves-sub.hpp"

namespace camoto {
namespace gamegraphics {

/// Maximum number of tilesets to load from a tileset collection.
#define CC_SAFETY_MAX_TILESETCOUNT  1024

/// Offset of first tileset in a tileset collection.
#define CC_FIRST_TILESET_OFFSET 0

/// Offset of the first tile within a single tileset.
#define CC_FIRST_TILE_OFFSET    3

#define FILETYPE_CCAVES_SUB "tile/ccaves"

//
// TilesetType_CCavesSub
//

TilesetType_CCavesSub::TilesetType_CCavesSub()
{
}

TilesetType_CCavesSub::~TilesetType_CCavesSub()
{
}

std::string TilesetType_CCavesSub::code() const
{
	return "tls-ccaves-sub";
}

std::string TilesetType_CCavesSub::friendlyName() const
{
	return "Crystal Caves Tileset";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> TilesetType_CCavesSub::fileExtensions() const
{
	return {"gfx"};
}

std::vector<std::string> TilesetType_CCavesSub::games() const
{
	return {
		"Crystal Caves",
	};
}

TilesetType_CCavesSub::Certainty TilesetType_CCavesSub::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();

	// Too short
	// TESTED BY: tls_ccaves_sub_isinstance_c02
	if (len < 3) return DefinitelyNo;

	content.seekg(0, stream::start);
	uint8_t numTiles, widthBytes, height;
	content
		>> u8(numTiles)
		>> u8(widthBytes)
		>> u8(height)
	;

	unsigned int lenTile = widthBytes * height * (unsigned int)PlaneCount::Masked;

	// Length doesn't match value calculated from header
	// TESTED BY: tls_ccaves_sub_isinstance_c01
	if ((unsigned)(3 + lenTile * numTiles) != len)
		return DefinitelyNo;

	// TESTED BY: tls_ccaves_sub_isinstance_c00
	return PossiblyYes;
}

std::shared_ptr<Tileset> TilesetType_CCavesSub::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->seekp(0, stream::start);
	// Zero tiles, 0x0
	content->write("\x00\x00\x00", 3);
	return this->open(std::move(content), suppData);
}

std::shared_ptr<Tileset> TilesetType_CCavesSub::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	return std::make_shared<Tileset_CCavesSub>(
		std::move(content), PlaneCount::Masked
	);
}

SuppFilenames TilesetType_CCavesSub::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	return {};
}


//
// Tileset_CCavesSub
//

Tileset_CCavesSub::Tileset_CCavesSub(std::unique_ptr<stream::inout> content,
	PlaneCount numPlanes)
	:	Tileset_FAT(std::move(content), CC_FIRST_TILE_OFFSET, ARCH_NO_FILENAMES),
		Tileset_FAT_FixedTileSize(0),
		numPlanes(numPlanes)
{
	stream::pos len = this->content->size();

	// We still have to perform sanity checks in case the user forced an
	// open even though it failed the signature check.
	if (len < 3) throw stream::error("file too short");

	this->content->seekg(0, stream::start);
	uint8_t numImages, widthBytes, height;
	*this->content
		>> u8(numImages)
		>> u8(widthBytes)
		>> u8(height)
	;
	this->dims.x = widthBytes * 8;
	this->dims.y = height;
	this->lenTile = widthBytes * height * (unsigned int)this->numPlanes;

	this->vcFAT.reserve(numImages);
	for (int i = 0; i < numImages; i++) {
		auto fat = std::make_unique<FATEntry>();
		fat->bValid = true;
		fat->fAttr = File::Attribute::Default;
		fat->iIndex = i;
		fat->iOffset = 3 + i * this->lenTile;
		fat->storedSize = this->lenTile;
		fat->lenHeader = 0;
		fat->type = FILETYPE_CCAVES_SUB;
		this->vcFAT.push_back(std::move(fat));
	}

}

Tileset_CCavesSub::~Tileset_CCavesSub()
{
}

Tileset::Caps Tileset_CCavesSub::caps() const
{
	return Caps::HasDimensions | Caps::SetDimensions;
}

ColourDepth Tileset_CCavesSub::colourDepth() const
{
	return ColourDepth::EGA;
}

Point Tileset_CCavesSub::dimensions() const
{
	return this->dims;
}

void Tileset_CCavesSub::dimensions(const Point& newDimensions)
{
	// TODO: confirm this, it could just leave the unused bits blank
	if (newDimensions.x % 8) throw stream::error("width must be a multiple of 8");
	this->dims = newDimensions;

	// Update the header
	this->content->seekp(1, stream::start);
	auto widthBytes = (this->dims.x + 7) / 8;
	*this->content
		<< u8(widthBytes)
		<< u8(this->dims.y)
	;

	this->changeTileSize(widthBytes * this->dims.y * (unsigned int)this->numPlanes);

	// Resize our containing stream to fit the new dimensions
	this->content->truncate(3 + this->vcFAT.size() * this->lenTile);
	return;
}

unsigned int Tileset_CCavesSub::layoutWidth() const
{
	return 10;
}

std::unique_ptr<Image> Tileset_CCavesSub::openImage(const FileHandle& id)
{
	EGAPlaneLayout planes = {
		(this->numPlanes == PlaneCount::Masked)
			? EGAPlanePurpose::Opaque1 : EGAPlanePurpose::Unused,
		EGAPlanePurpose::Blue1,
		EGAPlanePurpose::Green1,
		EGAPlanePurpose::Red1,
		EGAPlanePurpose::Intensity1,
	};

	return std::make_unique<Image_EGA_BytePlanar>(
		this->open(id, true), 0, this->dimensions(), planes, this->palette()
	);
}

const Tileset::FileHandle Tileset_CCavesSub::insert(const FileHandle& idBeforeThis,
	File::Attribute attr)
{
	return this->insert(idBeforeThis, "", this->lenTile, FILETYPE_CCAVES_SUB, attr);
}

void Tileset_CCavesSub::preInsertFile(
	const Tileset_CCavesSub::FATEntry *idBeforeThis,
	Tileset_CCavesSub::FATEntry *pNewEntry)
{
	if (this->vcFAT.size() >= 255) {
		throw stream::error("maximum number of tiles reached");
	}

	// Update the header
	this->content->seekp(0, stream::start);
	*this->content << u8((uint8_t)(this->vcFAT.size() + 1));
	return;
}

void Tileset_CCavesSub::postRemoveFile(const FATEntry *pid)
{
	// Update the header
	this->content->seekp(0, stream::start);
	*this->content << u8((uint8_t)this->vcFAT.size());
	return;
}


} // namespace gamegraphics
} // namespace camoto
