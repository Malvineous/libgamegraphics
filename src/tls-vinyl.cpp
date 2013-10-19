/**
 * @file   tls-vinyl.cpp
 * @brief  Vinyl Goddess From Mars tileset handler.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/VGFM_Tileset_Format
 *
 * Copyright (C) 2010-2013 Adam Nielsen <malvineous@shikadi.net>
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

#include <iostream>
#include <boost/scoped_array.hpp>
#include <camoto/iostream_helpers.hpp>
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

/// Image implementation for tiles within a VGFM tileset.
class VGFMTileImage: virtual public BaseImage
{
	public:
		VGFMTileImage(VinylTileset *tileset, unsigned int index,
			unsigned int size, PaletteTablePtr pal);
		virtual ~VGFMTileImage();

		virtual int getCaps();
		virtual void getDimensions(unsigned int *width, unsigned int *height);
		virtual StdImageDataPtr toStandard();
		virtual StdImageDataPtr toStandardMask();
		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask);
		virtual PaletteTablePtr getPalette();

	protected:
		VinylTileset *tileset;
		unsigned int index; ///< Tile index into tileset
		unsigned int size;
		PaletteTablePtr pal;
};

VGFMTileImage::VGFMTileImage(VinylTileset *tileset, unsigned int index,
	unsigned int size, PaletteTablePtr pal)
	:	tileset(tileset),
		index(index),
		size(size),
		pal(pal)
{
}

VGFMTileImage::~VGFMTileImage()
{
}

int VGFMTileImage::getCaps()
{
	return Image::ColourDepthVGA | Image::HasPalette;
}

void VGFMTileImage::getDimensions(unsigned int *width, unsigned int *height)
{
	*width = VGFM_TILE_WIDTH;
	*height = VGFM_TILE_HEIGHT;//this->size / VGFM_TILE_WIDTH_BYTES;
	return;
}

StdImageDataPtr VGFMTileImage::toStandard()
{
	return this->tileset->toStandard(this->index);
}

StdImageDataPtr VGFMTileImage::toStandardMask()
{
	return this->tileset->toStandardMask(this->index);
}

void VGFMTileImage::fromStandard(StdImageDataPtr newContent,
	StdImageDataPtr newMask)
{
	this->tileset->fromStandard(this->index, newContent, newMask);
	return;
}

PaletteTablePtr VGFMTileImage::getPalette()
{
	return this->pal;
}


VinylTilesetType::VinylTilesetType()
{
}

VinylTilesetType::~VinylTilesetType()
{
}

std::string VinylTilesetType::getCode() const
{
	return "tls-vinyl";
}

std::string VinylTilesetType::getFriendlyName() const
{
	return "Vinyl Goddess From Mars tileset";
}

std::vector<std::string> VinylTilesetType::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("tls");
	return vcExtensions;
}

std::vector<std::string> VinylTilesetType::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Vinyl Goddess From Mars");
	return vcGames;
}

VinylTilesetType::Certainty VinylTilesetType::isInstance(
	stream::input_sptr psTileset) const
{
	stream::pos len = psTileset->size();

	// TESTED BY: tls_vinyl_isinstance_c01
	if (len < VGFM_FIRST_TILE_OFFSET + 2) return DefinitelyNo; // too short

	psTileset->seekg(0, stream::start);
	uint16_t numTiles;
	psTileset >> u16le(numTiles);
	// TESTED BY: tls_vinyl_isinstance_c02
	if (numTiles > VGFM_SAFETY_MAX_TILES) return DefinitelyNo;

	stream::pos nextOffset = 2;
	if (numTiles > 0) {
		if (nextOffset >= len) return DefinitelyNo; // truncated
		unsigned int size;
		for (unsigned int i = 0; i < numTiles; i++) {
			psTileset
				>> u16le(size)
			;
			nextOffset += VGFM_FAT_ENTRY_LEN + size;

			// TESTED BY: tls_vinyl_isinstance_c03
			if (nextOffset >= len) return DefinitelyNo; // truncated

			// Don't seek +VGFM_FAT_ENTRY_LEN here because we just read those bytes
			psTileset->seekg(size, stream::cur);
		}
	}

	// TESTED BY: tls_vinyl_isinstance_c00
	return PossiblyYes;
}

TilesetPtr VinylTilesetType::create(stream::inout_sptr psTileset,
	SuppData& suppData) const
{
	psTileset->truncate(4);
	psTileset->seekp(0, stream::start);
	psTileset << u32le(0);

	PaletteTablePtr pal;
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new VGAPalette(suppData[SuppItem::Palette], 8));
		pal = palFile->getPalette();
	} else {
		throw stream::error("no palette specified (missing supplementary item)");
	}

	return TilesetPtr(new VinylTileset(psTileset, pal));
}

TilesetPtr VinylTilesetType::open(stream::inout_sptr psTileset,
	SuppData& suppData) const
{
	PaletteTablePtr pal;
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new VGAPalette(suppData[SuppItem::Palette], 8));
		pal = palFile->getPalette();
	} else {
		throw stream::error("no palette specified (missing supplementary item)");
	}

	return TilesetPtr(new VinylTileset(psTileset, pal));
}

SuppFilenames VinylTilesetType::getRequiredSupps(
	const std::string& filenameTileset) const
{
	SuppFilenames supps;
	std::string baseName = filenameTileset.substr(0, filenameTileset.length() - 3);
	supps[SuppItem::Palette] = baseName + "pal";
	return supps;
}


VinylTileset::VinylTileset(stream::inout_sptr data, PaletteTablePtr pal)
	:	FATTileset(data, VGFM_FIRST_TILE_OFFSET),
		pal(pal),
		pixelsChanged(false)
{
	stream::pos len = this->data->size();

	// We still have to perform sanity checks in case the user forced an
	// open even though it failed the signature check.
	if (len < VGFM_FIRST_TILE_OFFSET) throw stream::error("file too short");

	this->data->seekg(0, stream::start);
	uint16_t numTiles;
	this->data >> u16le(numTiles);
	this->items.reserve(numTiles);
	if (numTiles > VGFM_SAFETY_MAX_TILES) throw stream::error("too many tiles");

	stream::pos nextOffset = 2;
	if (numTiles > 0) {
		for (unsigned int i = 0; i < numTiles; i++) {
			FATEntry *fat = new FATEntry();
			EntryPtr ep(fat);
			fat->valid = true;
			fat->attr = Tileset::Default;
			fat->index = i;
			fat->offset = nextOffset;
			fat->lenHeader = VGFM_FAT_ENTRY_LEN;
			this->data
				>> u16le(fat->size)
			;
			nextOffset += VGFM_FAT_ENTRY_LEN + fat->size;
			// Don't seek +VGFM_FAT_ENTRY_LEN here because we just read those bytes
			this->data->seekg(fat->size, stream::cur);
			this->items.push_back(ep);
		}
	}
	uint16_t lenPixels;
	this->data >> u16le(lenPixels);
	this->pixels.resize(lenPixels, 0);
	this->data->read(this->pixels.data(), lenPixels);
}

VinylTileset::~VinylTileset()
{
	if (this->pixelsChanged) {
		std::cerr
			<< "ERROR: Vinyl tileset destroyed without flushing changed pixels!"
			<< std::endl;
	}
}

int VinylTileset::getCaps()
{
	return Tileset::HasPalette | Tileset::ColourDepthVGA;
}

void VinylTileset::flush()
{
	if (this->pixelsChanged) {

		// Figure out which codes are in use and which aren't
		unsigned int numValidCodes = this->pixels.size() / 4;
		boost::scoped_array<bool> usedCodes(new bool[numValidCodes]);
		for (unsigned int c = 0; c < numValidCodes; c++) usedCodes[c] = false;

		stream::pos endOfData = VGFM_FIRST_TILE_OFFSET;
		uint8_t tileData[VGFM_TILE_WIDTH * VGFM_TILE_HEIGHT * 3];
		for (VC_ENTRYPTR::const_iterator
			t = this->items.begin(); t != this->items.end(); t++
		) {
			const FATEntry *pFAT = dynamic_cast<const FATEntry *>(t->get());

			// Figure out where the tile data ends, since we're looping through the
			// list of tiles already.
			stream::pos proposed = pFAT->offset + pFAT->lenHeader + pFAT->size;
			if (proposed > endOfData) endOfData = proposed;

			// Examine each tile for the list of used codes
			this->data->seekg(pFAT->offset, stream::start);
			uint16_t lenTile;
			this->data >> u16le(lenTile);
			if ((lenTile != 0x80) && (lenTile != 0xC0)) {
				std::cerr << "tls-vinyl: Encountered a tile of unknown type 0x"
					<< std::hex << lenTile << " at offset 0x" << pFAT->offset
					<< std::dec << std::endl;
				throw stream::error("Encountered a tile of an unknown type!");
			}
			this->data->read(tileData, lenTile);
			for (unsigned int p = 0; p < VGFM_TILE_WIDTH * VGFM_TILE_HEIGHT / 4; p++) {
				uint8_t *base = &tileData[p * (lenTile == 0x80 ? 2 : 3) + (lenTile == 0x80 ? 0 : 1)];
				uint16_t code = base[0] + (base[1] << 8);
				if (code >= numValidCodes) {
					std::cerr << "tls-vinyl: Got invalid code " << code
						<< " (max valid code is " << numValidCodes - 1 << ") - tile "
						<< pFAT->index << ", offset " << p << std::endl;
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
			for (VC_ENTRYPTR::const_iterator
				t = this->items.begin(); t != this->items.end(); t++
			) {
				const FATEntry *pFAT = dynamic_cast<const FATEntry *>(t->get());
				this->data->seekg(pFAT->offset, stream::start);
				uint16_t lenTile;
				this->data >> u16le(lenTile);
				if ((lenTile != 0x80) && (lenTile != 0xC0)) {
					std::cerr << "tls-vinyl: Encountered a tile of unknown type 0x"
						<< std::hex << lenTile << " at offset 0x" << pFAT->offset
						<< std::dec << std::endl;
					throw stream::error("Encountered a tile of an unknown type!");
				}
				this->data->read(tileData, lenTile);
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
						this->data->seekp(pFAT->offset + pFAT->lenHeader + offset, stream::start);
						this->data << u16le(destCode);
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
		stream::pos lenCurrent = this->data->size();
		stream::pos lenTarget = endOfData + 2 + this->pixels.size();
		if (lenTarget < lenCurrent) {
			this->data->seekp(lenTarget, stream::start);
			this->data->remove(lenCurrent - lenTarget);
		} else if (lenTarget > lenCurrent) {
			this->data->seekp(lenCurrent, stream::start);
			this->data->insert(lenTarget - lenCurrent);
		}
		this->data->seekp(endOfData, stream::start);
		this->data << u16le(this->pixels.size());
		this->data->write(this->pixels.data(), this->pixels.size());
		this->pixelsChanged = false;
	}

	this->FATTileset::flush();
	return;
}

ImagePtr VinylTileset::createImageInstance(const EntryPtr& id,
	stream::inout_sptr content)
{
	FATEntry *fat = dynamic_cast<FATEntry *>(id.get());
	assert(fat);

	return ImagePtr(new VGFMTileImage(this, fat->index, fat->size, this->pal));
}

PaletteTablePtr VinylTileset::getPalette()
{
	return this->pal;
}

VinylTileset::FATEntry *VinylTileset::preInsertFile(
	const VinylTileset::FATEntry *idBeforeThis, VinylTileset::FATEntry *pNewEntry)
{
	pNewEntry->lenHeader = VGFM_FAT_ENTRY_LEN;

	this->data->seekp(pNewEntry->offset, stream::start);
	this->data->insert(pNewEntry->lenHeader);
	this->data << u16le(pNewEntry->size); // should be zero but we'll write it anyway

	// Update all the offsets to correct for the extra bytes we just added
	this->shiftFiles(
		pNewEntry,
		pNewEntry->offset,
		pNewEntry->lenHeader,
		0
	);

	return pNewEntry;
}

void VinylTileset::postInsertFile(FATEntry *pNewEntry)
{
	this->updateFileCount(this->items.size());
	this->pixelsChanged = true; // optimise pixels on flush()
	return;
}

void VinylTileset::postRemoveFile(const FATEntry *pid)
{
	this->updateFileCount(this->items.size());
	this->pixelsChanged = true; // optimise pixels on flush()
	return;
}

StdImageDataPtr VinylTileset::toStandard(unsigned int index)
{
	FATEntry *fatEntry = dynamic_cast<FATEntry *>(this->items[index].get());
	assert(fatEntry);

	unsigned int height = fatEntry->size / VGFM_TILE_WIDTH_BYTES;
	int dataSize = VGFM_TILE_WIDTH * height;

	uint8_t *inData = new uint8_t[fatEntry->size];
	StdImageDataPtr in(inData);
	uint8_t *outData = new uint8_t[dataSize];
	StdImageDataPtr ret(outData);
	this->data->seekg(fatEntry->offset + VGFM_FAT_ENTRY_LEN, stream::start);
	this->data->read(inData, fatEntry->size);

	unsigned int len;
	if (fatEntry->size == 0x80) len = 0x80/2;
	else if (fatEntry->size == 0xC0) len = 0xC0/3;
	else {
		throw stream::error("Encountered a VGFM tile of an unsupported size - "
			"please report this error!");
	}

	for (unsigned int i = 0; i < len; i++) {
		unsigned int code;
		if (fatEntry->size == 0x80) {
			code = inData[i * 2] | (inData[i * 2 + 1] << 8);
		} else { // fatEntry->size == 0xC0
			code = inData[i * 3 + 1] | (inData[i * 3 + 2] << 8);
		}
		memcpy(outData, &this->pixels[code * 4], 4);
		outData += 4;
	}
	return ret;
}

StdImageDataPtr VinylTileset::toStandardMask(unsigned int index)
{
	FATEntry *fatEntry = dynamic_cast<FATEntry *>(this->items[index].get());
	assert(fatEntry);

	int dataSize = VGFM_TILE_WIDTH * VGFM_TILE_HEIGHT;

	uint8_t *imgData = new uint8_t[dataSize];
	StdImageDataPtr ret(imgData);

	if (fatEntry->size == 0x80) {
		// Return an entirely opaque mask
		memset(imgData, 0, dataSize);
	} else if (fatEntry->size == 0xC0) {
		// Decode the mask bytes
		uint8_t *inData = new uint8_t[fatEntry->size];
		StdImageDataPtr in(inData);
		this->data->seekg(fatEntry->offset + VGFM_FAT_ENTRY_LEN, stream::start);
		this->data->read(inData, fatEntry->size);
		for (unsigned int i = 0; i < 0xC0/3; i++) {
			int maskVal = inData[i * 3];
			for (int b = 0; b < 4; b++) {
				*imgData++ = ((maskVal >> b) & 1) ^ 1;
			}
		}
	}

	return ret;
}

void VinylTileset::fromStandard(unsigned int index, StdImageDataPtr newContent,
	StdImageDataPtr newMask)
{
	FATEntry *fatEntry = dynamic_cast<FATEntry *>(this->items[index].get());
	assert(fatEntry);

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
	this->resize(this->items[index], len);

	this->data->seekg(fatEntry->offset, stream::start);
	this->data << u16le(len);

	for (unsigned int i = 0; i < VGFM_TILE_WIDTH * VGFM_TILE_HEIGHT; i += 4) {
		if (hasMask) {
			// Write the mask byte first
			uint8_t val = 0;
			for (unsigned int j = 0; j < 4; j++) {
				if ((newMask[i + j] & Image::Mask_Visibility) == Image::Mask_Vis_Transparent) {
					val |= 1 << j;
				}
			}
			this->data << u8(val);
		}

		// Try to find a code that matches these pixels
		long code = -1;
		unsigned int numValidCodes = this->pixels.size() / 4;
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
			//those.
			code = numValidCodes;
			numValidCodes++;
			this->pixels.push_back(newContent[i + 0]);
			this->pixels.push_back(newContent[i + 1]);
			this->pixels.push_back(newContent[i + 2]);
			this->pixels.push_back(newContent[i + 3]);
			this->pixelsChanged = true;
		}
		assert(this->data->tellg() - fatEntry->offset < len + VGFM_FAT_ENTRY_LEN);
		this->data << u16le(code);
	}
	return;
}

void VinylTileset::updateFileCount(uint32_t newCount)
{
	this->data->seekp(VGFM_TILECOUNT_OFFSET, stream::start);
	this->data << u16le(newCount);
	return;
}


} // namespace gamegraphics
} // namespace camoto
