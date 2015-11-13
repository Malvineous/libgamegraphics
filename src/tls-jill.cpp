/**
 * @file  tls-jill.cpp
 * @brief Jill of the Jungle tileset
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/SHA_Format
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
#include <camoto/stream_sub.hpp>
#include <camoto/util.hpp> // make_unique
#include "img-ega-byteplanar.hpp"
#include "pal-vga-raw.hpp"
#include "tls-jill.hpp"

namespace camoto {
namespace gamegraphics {

/// Number of tilesets in a SHA file
#define JILL_NUM_TILESETS  128

/// Offset of first tileset in a tileset collection.
#define JILL_FIRST_TILESET_OFFSET 0

/// Offset of first tile in a tileset.
#define JILL_FIRST_TILE_OFFSET 12

// Tileset flags
#define JILL_F_FONT  1  ///< This image is a font
#define JILL_F_BL    4  ///< This image is a level/block tile

#define BYTE_OFFSET 16  ///< 16 == VGA, 8 == EGA, 0 == CGA

/// Colour depth of VGA palette
#define JILL_PAL_DEPTH 6

#define FILETYPE_JILL "tileset/jill-sub"

//
// TilesetType_Jill
//

TilesetType_Jill::TilesetType_Jill()
{
}

TilesetType_Jill::~TilesetType_Jill()
{
}

std::string TilesetType_Jill::code() const
{
	return "tls-jill";
}

std::string TilesetType_Jill::friendlyName() const
{
	return "Jill of the Jungle Tileset";
}

std::vector<std::string> TilesetType_Jill::fileExtensions() const
{
	return {"sha"};
}

std::vector<std::string> TilesetType_Jill::games() const
{
	return {
		"Jill of the Jungle",
	};
}

TilesetType_Jill::Certainty TilesetType_Jill::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();
	if (len < 128 * 6) return DefinitelyNo; // missing offset table

	uint32_t offset;
	for (int i = 0; i < 128; i++) {
		content >> u32le(offset);
		if (offset > len) return DefinitelyNo; // offset past EOF
	}

	return DefinitelyYes;
}

std::shared_ptr<Tileset> TilesetType_Jill::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->seekp(0, stream::start);
	for (int i = 0; i < 128; i++) content->write("\0\0\0\0\0\0", 6);
	return this->open(std::move(content), suppData);
}

std::shared_ptr<Tileset> TilesetType_Jill::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	std::shared_ptr<const Palette> pal;
	// Only load the palette if one was given
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		auto palFile = std::make_shared<Palette_VGA>(
			std::move(suppData[SuppItem::Palette]), JILL_PAL_DEPTH
		);
		pal = palFile->palette();
	}
	return std::make_shared<Tileset_Jill>(std::move(content), pal);
}

SuppFilenames TilesetType_Jill::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	SuppFilenames supps;
	std::string baseName = filename.substr(0, filename.length() - 3);
	supps[SuppItem::Palette] = baseName + "pal";
	return supps;
}


//
// Tileset_Jill
//

Tileset_Jill::Tileset_Jill(std::unique_ptr<stream::inout> content,
	std::shared_ptr<const Palette> pal)
	:	Tileset_FAT(std::move(content), JILL_FIRST_TILESET_OFFSET, ARCH_NO_FILENAMES),
		loadedPal(false)
{
	this->pal = pal;
	if (this->pal) loadedPal = true;

	this->vcFAT.reserve(JILL_NUM_TILESETS);
	for (int i = 0; i < JILL_NUM_TILESETS; i++) {
		uint32_t offset;
		this->content->seekg(4 * i, stream::start);
		*this->content >> u32le(offset);

		uint16_t len;
		this->content->seekg((128 * 4) + 2 * i, stream::start);
		*this->content >> u16le(len);

		auto fat = std::make_unique<FATEntry>();
		fat->bValid = true;
		fat->fAttr = File::Attribute::Folder;
		if ((offset == 0) && (len == 0)) fat->fAttr |= File::Attribute::Vacant;
		fat->iIndex = i;
		fat->lenHeader = 0;

		fat->iOffset = offset;
		fat->storedSize = len;

		this->vcFAT.push_back(std::move(fat));
	}
}

Tileset_Jill::~Tileset_Jill()
{
}

Tileset::Caps Tileset_Jill::caps() const
{
	if (!this->loadedPal) {
		const_cast<Tileset_Jill *>(this)->loadPalette();
	}
	return this->pal ? Caps::HasPalette : Caps::Default;
}

ColourDepth Tileset_Jill::colourDepth() const
{
	return ColourDepth::VGA;
}

unsigned int Tileset_Jill::layoutWidth() const
{
	return 1;
}

std::shared_ptr<Tileset> Tileset_Jill::openTileset(const FileHandle& id)
{
	// Make sure nobody tries to open an empty slot!
	assert(!(id->fAttr & File::Attribute::Vacant));

	return std::make_shared<Tileset_JillSub>(this->open(id, true),
		this->palette());
}

const Tileset::FileHandle Tileset_Jill::insert(const FileHandle& idBeforeThis,
	File::Attribute attr)
{
	return this->insert(idBeforeThis, "", 0, FILETYPE_JILL, attr);
}

void Tileset_Jill::updateFileOffset(const FATEntry *pid, stream::len offDelta)
{
	this->content->seekp(pid->iIndex * 4, stream::start);
	*this->content << u32le(pid->iOffset);
	return;
}

void Tileset_Jill::updateFileSize(const FATEntry *pid, stream::len sizeDelta)
{
	this->content->seekp((128 * 4) + pid->iIndex * 2, stream::start);
	*this->content << u16le(pid->storedSize);
	return;
}

std::shared_ptr<const Palette> Tileset_Jill::palette() const
{
	if (!this->loadedPal) {
		const_cast<Tileset_Jill *>(this)->loadPalette();
	}
	return this->pal;
}

void Tileset_Jill::loadPalette()
{
	this->loadedPal = true; // stop below functions from calling us recursively

	// Xargon stores the palette in tileset 5, so query that now and if it looks
	// like a palette, use that.
	if (this->vcFAT.size() > 5) {
		auto tlsPal = this->openTileset(this->vcFAT[5]);
		if (tlsPal) {
			auto entries = tlsPal->files();
			if (entries.size() > 0) {
				auto palEntry = tlsPal->openImage(entries[0]);
				if (palEntry && (palEntry->caps() & Image::Caps::HasPalette)) {
					this->pal = palEntry->palette();
				}
			}
		}
	}
	return;
}


//
// Tileset_JillSub
//

Tileset_JillSub::Tileset_JillSub(std::unique_ptr<stream::inout> content,
	std::shared_ptr<const Palette> pal)
	:	Tileset_FAT(std::move(content), JILL_FIRST_TILE_OFFSET, ARCH_NO_FILENAMES)
{
	this->pal = pal;

	uint8_t numImages;
	uint16_t numRots;
	uint16_t lenCGA, lenEGA, lenVGA;
	uint8_t bppColourMap;
	uint16_t flags;

	*this->content
		>> u8(numImages)
		>> u16le(numRots)
		>> u16le(lenCGA)
		>> u16le(lenEGA)
		>> u16le(lenVGA)
		>> u8(bppColourMap)
		>> u16le(flags)
	;

	unsigned int offset = 12;
	this->colourMap.clear();
	if (flags & JILL_F_FONT) {
		// Predefined colour map
		int lenColourMap = 1 << bppColourMap;
		this->colourMap.resize(lenColourMap);
		for (int i = 0; i < lenColourMap; i++) this->colourMap[i] = i;

	} else if (bppColourMap == 8) {
		// Predefined colour map
		this->colourMap.resize(256);
		for (int i = 0; i < 256; i++) this->colourMap[i] = i;

	} else {
		// Read the colour map
		int lenColourMap = 1 << bppColourMap;
		this->colourMap.resize(lenColourMap);
		for (int i = 0; i < lenColourMap; i++) {
			uint32_t value;
			*this->content >> u32le(value);
			this->colourMap[i] = (value >> BYTE_OFFSET) & 0xFF;
		}
		offset += lenColourMap * 4;
	}

	this->vcFAT.reserve(numImages);
	for (int i = 0; i < numImages; i++) {
		uint8_t width, height;
		*this->content >> u8(width) >> u8(height);
		auto fat = std::make_unique<FATEntry>();
		fat->bValid = true;
		fat->fAttr = File::Attribute::Default;
		fat->iIndex = i;
		fat->iOffset = offset;
		fat->storedSize = width * height + 3; // always 8bpp
		fat->lenHeader = 0;
		offset += fat->storedSize;
		this->content->seekg(fat->storedSize - 2, stream::cur); // -2 because we read them above
		this->vcFAT.push_back(std::move(fat));
	}

}

Tileset_JillSub::~Tileset_JillSub()
{
}

Tileset::Caps Tileset_JillSub::caps() const
{
	return Caps::Default;
}

ColourDepth Tileset_JillSub::colourDepth() const
{
	return ColourDepth::VGA;
}

unsigned int Tileset_JillSub::layoutWidth() const
{
	return 10;
}

std::unique_ptr<Image> Tileset_JillSub::openImage(const FileHandle& id)
{
	auto fat = FATEntry::cast(id);
	assert(fat);

	auto contentImage = this->open(id, true);
	contentImage->seekg(0, stream::start);

	uint8_t width, height;
	*contentImage >> u8(width) >> u8(height);

	if (fat->storedSize == 64 * 12 + 3) {
		// This could actually be palette data
		if ((width == 64) && (height == 12)) {
			// Yes, definitely a palette
			return std::make_unique<Palette_VGA>(
				std::make_unique<stream::sub>(
					std::move(contentImage), 3, 768, nullptr // nullptr because this is a fixed size
				),
				6
			);
		}
	}

	contentImage->seekg(1, stream::cur);
	auto numPixels = width * height;

	// Convert image here
	Pixels pix;
	pix.resize(numPixels, 0);
	auto lenContentImage = std::min<stream::len>(numPixels, fat->storedSize);

	contentImage->read(pix.data(), lenContentImage);

	// Apply the colour map
	uint8_t *p = pix.data();
	for (int i = 0; i < numPixels; i++) {
		*p = this->colourMap[*p];
		p++;
	}

	Pixels mask;
	mask.resize(numPixels, 0);
	return std::make_unique<Image_Jill>(
		Point{width, height},
		std::move(pix), std::move(mask),
		this->palette(), [](){
			// TODO (changed func)
		}
	);
}

void Tileset_JillSub::preInsertFile(const FATEntry *idBeforeThis,
	FATEntry *pNewEntry)
{
	if (this->vcFAT.size() >= 255) {
		throw stream::error("maximum number of tiles reached");
	}

	// Update the header
	this->content->seekp(0, stream::start);
	*this->content << u8((uint8_t)this->vcFAT.size() + 1);

	return;
}

void Tileset_JillSub::postRemoveFile(const FATEntry *pid)
{
	// Update the header
	this->content->seekp(0, stream::start);
	*this->content << u8((uint8_t)this->vcFAT.size());
	return;
}


//
// Image_Jill
//

Image_Jill::Image_Jill(const Point& dims, Pixels&& pix, Pixels&& mask,
	std::shared_ptr<const Palette> pal, fn_changed_t fnChanged)
	:	dims(dims),
		pix(std::move(pix)),
		mask(std::move(mask)),
		fnChanged(fnChanged)
{
	this->pal = pal;
}

Image_Jill::~Image_Jill()
{
}

Image::Caps Image_Jill::caps() const
{
	return Caps::SetDimensions;
}

ColourDepth Image_Jill::colourDepth() const
{
	return ColourDepth::VGA;
}

Point Image_Jill::dimensions() const
{
	return this->dims;
}

void Image_Jill::dimensions(const Point& newDimensions)
{
	assert(this->caps() & Caps::SetDimensions);
	if ((newDimensions.x == 64) && (newDimensions.y == 12)) {
		// We can't store images this size (in 8-bit/VGA mode at least) because
		// if the game encounters these dimensions the image data is immediately
		// loaded into the VGA palette registers instead!
		throw stream::error("Sorry, this tileset format cannot store images that "
			"are exactly 64x12 pixels in size.  You will have to use a different "
			"size.");
	}
	this->dims = newDimensions;
	return;
}

Pixels Image_Jill::convert() const
{
	return this->pix;
}

Pixels Image_Jill::convert_mask() const
{
	return this->mask;
}

void Image_Jill::convert(const Pixels& newContent, const Pixels& newMask)
{
	this->pix = newContent;
	this->mask = newMask;
	this->fnChanged();
	return;
}


} // namespace gamegraphics
} // namespace camoto
