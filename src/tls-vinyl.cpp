/**
 * @file  tls-vinyl.cpp
 * @brief Vinyl Goddess From Mars tileset handler.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/VGFM_Tileset_Format
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
#include <iostream>
#include <camoto/iostream_helpers.hpp>
#include <camoto/util.hpp> // make_unique
#include "img-vga-raw.hpp"
#include "tls-vinyl.hpp"
#include "pal-vga-raw.hpp"

/// Offset of the number of tilesets
#define VGFM_TILECOUNT_OFFSET    0

/// Length of each entry in the FAT (a uint32 offset)
#define VGFM_FAT_ENTRY_LEN       2

/// Offset of first tile in an empty tileset
#define VGFM_FIRST_TILE_OFFSET   2

/// Maximum tiles to load in case of a corrupted file
#define VGFM_SAFETY_MAX_TILES    4096

/// Width of all tiles
#define VGFM_TILE_WIDTH          16

/// Height of all tiles
#define VGFM_TILE_HEIGHT         16

/// Width of a tile row in bytes
#define VGFM_TILE_WIDTH_BYTES    (VGFM_TILE_WIDTH / 2)

namespace camoto {
namespace gamegraphics {

/// Tileset implementation for a VGFM tileset.
class Tileset_Vinyl: virtual public Tileset_FAT
{
	public:
		Tileset_Vinyl(std::unique_ptr<stream::inout> content,
			std::shared_ptr<const Palette> pal);
		virtual ~Tileset_Vinyl();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual Point dimensions() const;
		virtual unsigned int layoutWidth() const;

		// Tileset_FAT
		virtual std::unique_ptr<Image> openImage(const FileHandle& id);
		virtual void flush();
		virtual void preInsertFile(const FATEntry *idBeforeThis,
			FATEntry *pNewEntry);
		virtual void postInsertFile(FATEntry *pNewEntry);
		virtual void postRemoveFile(const FATEntry *pid);

		// Called by Image_VGFMTile
		virtual Pixels convert(unsigned int index) const;
		virtual Pixels convert_mask(unsigned int index) const;
		virtual void convert(unsigned int index, const Pixels& newContent,
			const Pixels& newMask);

	private:
		std::vector<uint8_t> pixels; ///< Pixels for each pixel code
		bool pixelsChanged;          ///< Does the pixel array need to be written back to the file?

		/// Update the number of tiles in the tileset
		void updateFileCount(uint32_t newCount);
};

/// Image implementation for tiles within a VGFM tileset.
class Image_VGFMTile: virtual public Image
{
	public:
		Image_VGFMTile(Tileset_Vinyl *tileset, unsigned int index,
			unsigned int size);
		virtual ~Image_VGFMTile();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual Point dimensions() const;
		virtual Pixels convert() const;
		virtual Pixels convert_mask() const;
		virtual void convert(const Pixels& newContent,
			const Pixels& newMask);
		virtual std::shared_ptr<const Palette> palette() const;

	protected:
		Tileset_Vinyl *tileset;
		unsigned int index; ///< Tile index into tileset
		unsigned int size;
};

Image_VGFMTile::Image_VGFMTile(Tileset_Vinyl *tileset, unsigned int index,
	unsigned int size)
	:	tileset(tileset),
		index(index),
		size(size)
{
}

Image_VGFMTile::~Image_VGFMTile()
{
}

Image::Caps Image_VGFMTile::caps() const
{
	return Caps::HasPalette;
}

ColourDepth Image_VGFMTile::colourDepth() const
{
	return ColourDepth::VGA;
}

Point Image_VGFMTile::dimensions() const
{
	return {VGFM_TILE_WIDTH, VGFM_TILE_HEIGHT};
}

Pixels Image_VGFMTile::convert() const
{
	return this->tileset->convert(this->index);
}

Pixels Image_VGFMTile::convert_mask() const
{
	return this->tileset->convert_mask(this->index);
}

void Image_VGFMTile::convert(const Pixels& newContent,
	const Pixels& newMask)
{
	this->tileset->convert(this->index, newContent, newMask);
	return;
}

std::shared_ptr<const Palette> Image_VGFMTile::palette() const
{
	return this->tileset->palette();
}


TilesetType_Vinyl::TilesetType_Vinyl()
{
}

TilesetType_Vinyl::~TilesetType_Vinyl()
{
}

std::string TilesetType_Vinyl::code() const
{
	return "tls-vinyl";
}

std::string TilesetType_Vinyl::friendlyName() const
{
	return "Vinyl Goddess From Mars tileset";
}

std::vector<std::string> TilesetType_Vinyl::fileExtensions() const
{
	return {"tls"};
}

std::vector<std::string> TilesetType_Vinyl::games() const
{
	return {
		"Vinyl Goddess From Mars",
	};
}

TilesetType_Vinyl::Certainty TilesetType_Vinyl::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();

	// TESTED BY: tls_vinyl_isinstance_c01
	if (len < VGFM_FIRST_TILE_OFFSET + 2) return DefinitelyNo; // too short

	content.seekg(0, stream::start);
	uint16_t numTiles;
	content >> u16le(numTiles);
	// TESTED BY: tls_vinyl_isinstance_c02
	if (numTiles > VGFM_SAFETY_MAX_TILES) return DefinitelyNo;

	stream::pos nextOffset = 2;
	if (numTiles > 0) {
		if (nextOffset >= len) return DefinitelyNo; // truncated
		unsigned int size;
		for (unsigned int i = 0; i < numTiles; i++) {
			content
				>> u16le(size)
			;
			nextOffset += VGFM_FAT_ENTRY_LEN + size;

			// TESTED BY: tls_vinyl_isinstance_c03
			if (nextOffset >= len) return DefinitelyNo; // truncated

			// Don't seek +VGFM_FAT_ENTRY_LEN here because we just read those bytes
			content.seekg(size, stream::cur);
		}
	}

	// TESTED BY: tls_vinyl_isinstance_c00
	return PossiblyYes;
}

std::shared_ptr<Tileset> TilesetType_Vinyl::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->truncate(4);
	content->seekp(0, stream::start);
	*content << u32le(0);

	return this->open(std::move(content), suppData);
}

std::shared_ptr<Tileset> TilesetType_Vinyl::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	std::shared_ptr<const Palette> pal;
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		auto palFile = std::make_unique<Palette_VGA>(
			std::move(suppData[SuppItem::Palette]),
			8
		);
		pal = palFile->palette();
	}

	return std::make_shared<Tileset_Vinyl>(
		std::move(content),
		pal
	);
}

SuppFilenames TilesetType_Vinyl::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	SuppFilenames supps;
	std::string baseName = filename.substr(0, filename.length() - 3);
	supps[SuppItem::Palette] = baseName + "pal";
	return supps;
}


Tileset_Vinyl::Tileset_Vinyl(std::unique_ptr<stream::inout> content,
	std::shared_ptr<const Palette> pal)
	:	Tileset_FAT(std::move(content), VGFM_FIRST_TILE_OFFSET, ARCH_NO_FILENAMES),
		pixelsChanged(false)
{
	this->pal = pal;
	stream::pos len = this->content->size();

	// We still have to perform sanity checks in case the user forced an
	// open even though it failed the signature check.
	if (len < VGFM_FIRST_TILE_OFFSET) throw stream::error("file too short");

	this->content->seekg(0, stream::start);
	uint16_t numTiles;
	*this->content >> u16le(numTiles);
	this->vcFAT.reserve(numTiles);
	if (numTiles > VGFM_SAFETY_MAX_TILES) throw stream::error("too many tiles");

	stream::pos nextOffset = 2;
	if (numTiles > 0) {
		for (unsigned int i = 0; i < numTiles; i++) {
			FATEntry *fat = new FATEntry();
			FileHandle ep(fat);
			fat->bValid = true;
			fat->fAttr = File::Attribute::Default;
			fat->iIndex = i;
			fat->iOffset = nextOffset;
			fat->lenHeader = VGFM_FAT_ENTRY_LEN;
			*this->content
				>> u16le(fat->storedSize)
			;
			nextOffset += VGFM_FAT_ENTRY_LEN + fat->storedSize;
			// Don't seek +VGFM_FAT_ENTRY_LEN here because we just read those bytes
			this->content->seekg(fat->storedSize, stream::cur);
			this->vcFAT.push_back(ep);
		}
	}
	uint16_t lenPixels;
	*this->content >> u16le(lenPixels);
	this->pixels.resize(lenPixels, 0);
	this->content->read(this->pixels.data(), lenPixels);
}

Tileset_Vinyl::~Tileset_Vinyl()
{
	if (this->pixelsChanged) {
		this->flush();
/*
		std::cerr
			<< "ERROR: Vinyl tileset destroyed without flushing changed pixels!"
			<< std::endl;
*/
	}
}

Tileset::Caps Tileset_Vinyl::caps() const
{
	return Caps::HasPalette;
}

ColourDepth Tileset_Vinyl::colourDepth() const
{
	return ColourDepth::VGA;
}

Point Tileset_Vinyl::dimensions() const
{
	return {VGFM_TILE_WIDTH, VGFM_TILE_HEIGHT};
}

unsigned int Tileset_Vinyl::layoutWidth() const
{
	return 16;
}

void Tileset_Vinyl::flush()
{
	if (this->pixelsChanged) {

		// Figure out which codes are in use and which aren't
		unsigned int numValidCodes = this->pixels.size() / 4;
		std::vector<bool> usedCodes(numValidCodes, false);

		stream::pos endOfData = VGFM_FIRST_TILE_OFFSET;
		uint8_t tileData[VGFM_TILE_WIDTH * VGFM_TILE_HEIGHT * 3];
		for (auto t : this->vcFAT) {
			auto pFAT = FATEntry::cast(t);

			// Figure out where the tile data ends, since we're looping through the
			// list of tiles already.
			stream::pos proposed = pFAT->iOffset + pFAT->lenHeader + pFAT->storedSize;
			if (proposed > endOfData) endOfData = proposed;

			// Examine each tile for the list of used codes
			this->content->seekg(pFAT->iOffset, stream::start);
			uint16_t lenTile;
			*this->content >> u16le(lenTile);
			if ((lenTile != 0x80) && (lenTile != 0xC0)) {
				std::cerr << "tls-vinyl: Encountered a tile of unknown type 0x"
					<< std::hex << lenTile << " at offset 0x" << pFAT->iOffset
					<< std::dec << std::endl;
				throw stream::error("Encountered a tile of an unknown type!");
			}
			this->content->read(tileData, lenTile);
			for (unsigned int p = 0; p < VGFM_TILE_WIDTH * VGFM_TILE_HEIGHT / 4; p++) {
				uint8_t *base = &tileData[p * (lenTile == 0x80 ? 2 : 3) + (lenTile == 0x80 ? 0 : 1)];
				uint16_t code = base[0] + (base[1] << 8);
				if (code >= numValidCodes) {
					std::cerr << "tls-vinyl: Got invalid code " << code
						<< " (max valid code is one less than " << numValidCodes
						<< ") - tile " << pFAT->iIndex << ", offset " << p << std::endl;
					continue;
				}
				usedCodes[code] = true;
			}
		}

		// See whether any codes are unused
		bool unused = false;
		for (unsigned int c = 0; c < numValidCodes; c++) {
			if (!usedCodes[c]) {
				unused = true;
				break;
			}
		}

		if (unused) {
			// Rewrite all the tiles to only use codes in use
			for (auto t : this->vcFAT) {
				auto pFAT = FATEntry::cast(t);
				this->content->seekg(pFAT->iOffset, stream::start);
				uint16_t lenTile;
				*this->content >> u16le(lenTile);
				if ((lenTile != 0x80) && (lenTile != 0xC0)) {
					std::cerr << "tls-vinyl: Encountered a tile of unknown type 0x"
						<< std::hex << lenTile << " at offset 0x" << pFAT->iOffset
						<< std::dec << std::endl;
					throw stream::error("Encountered a tile of an unknown type!");
				}
				this->content->read(tileData, lenTile);
				for (unsigned int p = 0; p < VGFM_TILE_WIDTH * VGFM_TILE_HEIGHT / 4; p++) {
					unsigned int offset = p * (lenTile == 0x80 ? 2 : 3) + (lenTile == 0x80 ? 0 : 1);
					uint8_t *base = &tileData[offset];
					uint16_t code = base[0] + (base[1] << 8);
					if (code >= numValidCodes) {
						// Ignore, already printed warning in the loop above
						continue;
					}
					uint16_t destCode = code;
					for (unsigned int c2 = 0; c2 < code; c2++) {
						if (!usedCodes[c2]) destCode--;
					}
					if (destCode != code) {
						this->content->seekp(pFAT->iOffset + pFAT->lenHeader + offset, stream::start);
						*this->content << u16le(destCode);
					}
				}
			}

			// Remove all the unused codes from the pixel data
			for (int c = (int)numValidCodes - 1; c >= 0; c--) {
				if (!usedCodes[c]) {
					this->pixels.erase(this->pixels.begin() + c * 4, this->pixels.begin() + (c + 1) * 4);
				}
			}
			// numValidCodes is no longer accurate
		}

		// Write the optimised pixel data

		// Expand or shrink the stream so there is just enough space for the pixel data
		stream::pos lenCurrent = this->content->size();
		stream::pos lenTarget = endOfData + 2 + this->pixels.size();
		if (lenTarget < lenCurrent) {
			this->content->seekp(lenTarget, stream::start);
			this->content->remove(lenCurrent - lenTarget);
		} else if (lenTarget > lenCurrent) {
			this->content->seekp(lenCurrent, stream::start);
			this->content->insert(lenTarget - lenCurrent);
		}
		this->content->seekp(endOfData, stream::start);
		*this->content << u16le(this->pixels.size());
		this->content->write(this->pixels.data(), this->pixels.size());
		this->pixelsChanged = false;
	}

	this->Tileset_FAT::flush();
	return;
}

std::unique_ptr<Image> Tileset_Vinyl::openImage(const FileHandle& id)
{
	auto fat = FATEntry::cast(id);
	assert(fat);

	return std::make_unique<Image_VGFMTile>(
		this, fat->iIndex, fat->storedSize
	);
}

void Tileset_Vinyl::preInsertFile(const FATEntry *idBeforeThis,
	FATEntry *pNewEntry)
{
	pNewEntry->lenHeader = VGFM_FAT_ENTRY_LEN;

	this->content->seekp(pNewEntry->iOffset, stream::start);
	this->content->insert(pNewEntry->lenHeader);
	*this->content << u16le(pNewEntry->storedSize);

	// Update all the offsets to correct for the extra bytes we just added
	this->shiftFiles(
		pNewEntry,
		pNewEntry->iOffset,
		pNewEntry->lenHeader,
		0
	);

	return;
}

void Tileset_Vinyl::postInsertFile(FATEntry *pNewEntry)
{
	this->updateFileCount(this->vcFAT.size());
	this->pixelsChanged = true; // optimise pixels on flush()
	return;
}

void Tileset_Vinyl::postRemoveFile(const FATEntry *pid)
{
	this->updateFileCount(this->vcFAT.size());
	this->pixelsChanged = true; // optimise pixels on flush()
	return;
}

Pixels Tileset_Vinyl::convert(unsigned int index) const
{
	auto fat = FATEntry::cast(this->vcFAT[index]);
	assert(fat);

	unsigned int height = fat->storedSize / VGFM_TILE_WIDTH_BYTES;
	int dataSize = VGFM_TILE_WIDTH * height;

	Pixels inData(fat->storedSize);
	Pixels outData(dataSize, '\x00');

	this->content->seekg(fat->iOffset + VGFM_FAT_ENTRY_LEN, stream::start);
	this->content->read(inData.data(), fat->storedSize);

	unsigned int len;
	if (fat->storedSize == 0x80) len = 0x80/2;
	else if (fat->storedSize == 0xC0) len = 0xC0/3;
	else {
		throw stream::error("Encountered a VGFM tile of an unsupported size - "
			"please report this error!");
	}

	auto imgData = outData.data();
	auto maxCode = this->pixels.size() / 4;
	for (unsigned int i = 0; i < len; i++) {
		unsigned int code;
		if (fat->storedSize == 0x80) {
			code = inData[i * 2] | (inData[i * 2 + 1] << 8);
		} else { // fat->storedSize == 0xC0
			code = inData[i * 3 + 1] | (inData[i * 3 + 2] << 8);
		}
		if (code > maxCode) code = 0; // just in case, should never happen
		memcpy(imgData, &this->pixels[code * 4], 4);
		imgData += 4;
	}
	return outData;
}

Pixels Tileset_Vinyl::convert_mask(unsigned int index) const
{
	auto fat = FATEntry::cast(this->vcFAT[index]);
	assert(fat);

	int dataSize = VGFM_TILE_WIDTH * VGFM_TILE_HEIGHT;

	Pixels outData(dataSize, '\x00');

	if (fat->storedSize == 0x80) {
		// Return an entirely opaque mask
		for (int i = 0; i < dataSize; i++) assert(outData[i] == 0); // be sure
	} else if (fat->storedSize == 0xC0) {
		// Decode the mask bytes
		Pixels inData(fat->storedSize);
		this->content->seekg(fat->iOffset + VGFM_FAT_ENTRY_LEN, stream::start);
		this->content->read(inData.data(), fat->storedSize);
		auto imgData = outData.data();
		for (unsigned int i = 0; i < 0xC0/3; i++) {
			int maskVal = inData[i * 3];
			for (int b = 0; b < 4; b++) {
				*imgData++ = ((maskVal >> b) & 1) ^ 1;
			}
		}
	}

	return outData;
}

void Tileset_Vinyl::convert(unsigned int index, const Pixels& newContent,
	const Pixels& newMask)
{
	auto fat = FATEntry::cast(this->vcFAT[index]);
	assert(fat);

	bool hasMask = false;
	for (unsigned int i = 0; i < VGFM_TILE_WIDTH * VGFM_TILE_HEIGHT; i++) {
		if (newMask[i] != 0x01) {
			hasMask = true;
			break;
		}
	}
	unsigned int len = hasMask ? 0xC0 : 0x80;
	// Don't include the embedded FAT here as that is added to the requested
	// length internally.
	this->resize(this->vcFAT[index], len, len);

	this->content->seekg(fat->iOffset, stream::start);
	*this->content << u16le(len);

	unsigned int numValidCodes = this->pixels.size() / 4;
	for (unsigned int i = 0; i < VGFM_TILE_WIDTH * VGFM_TILE_HEIGHT; i += 4) {
		if (hasMask) {
			// Write the mask byte first
			uint8_t val = 0;
			for (unsigned int j = 0; j < 4; j++) {
				if (!(newMask[i + j] & (int)Image::Mask::Transparent)) {
					val |= 1 << j;
				}
			}
			*this->content << u8(val);
		}

		// Try to find a code that matches these pixels
		long code = -1;
		for (unsigned int c = 0; c < numValidCodes; c++) {
			if (
				(newContent[i + 0] == this->pixels[c * 4 + 0]) &&
				(newContent[i + 1] == this->pixels[c * 4 + 1]) &&
				(newContent[i + 2] == this->pixels[c * 4 + 2]) &&
				(newContent[i + 3] == this->pixels[c * 4 + 3])
			) {
				code = c;
				break;
			}
		}
		if (code == -1) {
			// Need to allocate a new code.  We won't bother searching for unused
			// codes, because the optimiser in flush() will take care of removing
			// those.
			code = numValidCodes;
			numValidCodes++;
			this->pixels.push_back(newContent[i + 0]);
			this->pixels.push_back(newContent[i + 1]);
			this->pixels.push_back(newContent[i + 2]);
			this->pixels.push_back(newContent[i + 3]);
			this->pixelsChanged = true;
		}
		assert(this->content->tellg() - fat->iOffset < len + VGFM_FAT_ENTRY_LEN);
		*this->content << u16le(code);
	}
	return;
}

void Tileset_Vinyl::updateFileCount(uint32_t newCount)
{
	this->content->seekp(VGFM_TILECOUNT_OFFSET, stream::start);
	*this->content << u16le(newCount);
	return;
}


} // namespace gamegraphics
} // namespace camoto
