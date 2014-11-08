/**
 * @file   tls-jill.cpp
 * @brief  Jill of the Jungle tileset
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/SHA_Format
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

#include <camoto/iostream_helpers.hpp>
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

//
// JillTilesetType
//

JillTilesetType::JillTilesetType()
{
}

JillTilesetType::~JillTilesetType()
{
}

std::string JillTilesetType::getCode() const
{
	return "tls-jill";
}

std::string JillTilesetType::getFriendlyName() const
{
	return "Jill of the Jungle Tileset";
}

std::vector<std::string> JillTilesetType::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("sha");
	return vcExtensions;
}

std::vector<std::string> JillTilesetType::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Jill of the Jungle");
	return vcGames;
}

JillTilesetType::Certainty JillTilesetType::isInstance(stream::input_sptr psGraphics) const
{
	stream::pos len = psGraphics->size();
	if (len < 128 * 6) return DefinitelyNo; // missing offset table

	uint32_t offset;
	for (int i = 0; i < 128; i++) {
		psGraphics >> u32le(offset);
		if (offset > len) return DefinitelyNo; // offset past EOF
	}

	return DefinitelyYes;
}

TilesetPtr JillTilesetType::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	PaletteTablePtr pal;
	// Only load the palette if one was given
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new VGAPalette(suppData[SuppItem::Palette], 6));
		pal = palFile->getPalette();
	}

	psGraphics->seekp(0, stream::start);
	for (int i = 0; i < 128; i++) psGraphics->write("\0\0\0\0\0\0", 6);
	return TilesetPtr(new JillTileset(psGraphics, pal));
}

TilesetPtr JillTilesetType::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	PaletteTablePtr pal;
	// Only load the palette if one was given
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new VGAPalette(suppData[SuppItem::Palette], 6));
		pal = palFile->getPalette();
	}

	return TilesetPtr(new JillTileset(psGraphics, pal));
}

SuppFilenames JillTilesetType::getRequiredSupps(
	const std::string& filenameTileset) const
{
	SuppFilenames supps;
	std::string baseName = filenameTileset.substr(0, filenameTileset.length() - 3);
	supps[SuppItem::Palette] = baseName + "pal";
	return supps;
}


//
// JillTileset
//

JillTileset::JillTileset(stream::inout_sptr data, PaletteTablePtr pal)
	:	FATTileset(data, JILL_FIRST_TILESET_OFFSET),
		pal(pal)
{
	this->items.reserve(JILL_NUM_TILESETS);
	for (int i = 0; i < JILL_NUM_TILESETS; i++) {
		uint32_t offset;
		this->data->seekg(4 * i, stream::start);
		this->data >> u32le(offset);

		uint16_t len;
		this->data->seekg((128 * 4) + 2 * i, stream::start);
		this->data >> u16le(len);

		FATEntry *fat = new FATEntry();
		EntryPtr ep(fat);
		fat->valid = true;
		fat->attr = Tileset::SubTileset;
		if ((offset == 0) && (len == 0)) fat->attr |= Tileset::EmptySlot;
		fat->index = i;
		fat->lenHeader = 0;

		fat->offset = offset;
		fat->size = len;

		this->items.push_back(ep);
	}

}

JillTileset::~JillTileset()
{
}

int JillTileset::getCaps()
{
	return this->pal ? Tileset::HasPalette : 0;
}

PaletteTablePtr JillTileset::getPalette()
{
	return this->pal;
}

TilesetPtr JillTileset::createTilesetInstance(const EntryPtr& id,
	stream::inout_sptr content)
{
	// Make sure nobody tries to open an empty slot!
	assert((id->getAttr() & Tileset::EmptySlot) == 0);

	//stream::sub_sptr sub = boost::dynamic_pointer_cast<stream::sub>(content);
	return TilesetPtr(new JillTiles(content));
}

void JillTileset::updateFileOffset(const FATEntry *pid, stream::len offDelta)
{
	this->data->seekp(pid->index * 4, stream::start);
	this->data << u32le(pid->offset);
	return;
}

void JillTileset::updateFileSize(const FATEntry *pid, stream::len sizeDelta)
{
	this->data->seekp((128 * 4) + pid->index * 2, stream::start);
	this->data << u16le(pid->size);
	return;
}


//
// JillTiles
//

JillTiles::JillTiles(stream::inout_sptr data)
	:	FATTileset(data, JILL_FIRST_TILE_OFFSET)
{
	uint8_t numImages;
	uint16_t numRots;
	uint16_t lenCGA, lenEGA, lenVGA;
	uint8_t bppColourMap;
	uint16_t flags;

	this->data
		>> u8(numImages)
		>> u16le(numRots)
		>> u16le(lenCGA)
		>> u16le(lenEGA)
		>> u16le(lenVGA)
		>> u8(bppColourMap)
		>> u16le(flags)
	;

	unsigned int offset = 12;
	if (flags & JILL_F_FONT) {
		// Predefined colour map
		int lenColourMap = 1 << bppColourMap;
		this->colourMap.reset(new uint8_t[lenColourMap]);
		for (int i = 0; i < lenColourMap; i++) this->colourMap[i] = i;

	} else if (bppColourMap == 8) {
		// Predefined colour map
		this->colourMap.reset(new uint8_t[256]);
		for (int i = 0; i < 256; i++) this->colourMap[i] = i;

	} else {
		// Read the colour map
		int lenColourMap = 1 << bppColourMap;
		this->colourMap.reset(new uint8_t[lenColourMap]);
		for (int i = 0; i < lenColourMap; i++) {
			uint32_t value;
			this->data >> u32le(value);
			this->colourMap[i] = (value >> BYTE_OFFSET) & 0xFF;
		}
		offset += lenColourMap * 4;
	}

	this->items.reserve(numImages);
	for (int i = 0; i < numImages; i++) {
		uint8_t width, height;
		this->data >> u8(width) >> u8(height);
		FATEntry *fat = new FATEntry();
		EntryPtr ep(fat);
		fat->valid = true;
		fat->attr = 0;
		fat->index = i;
		fat->offset = offset;
		fat->size = width * height + 3; // always 8bpp
		fat->lenHeader = 0;
		this->items.push_back(ep);
		offset += fat->size;
		this->data->seekg(fat->size - 2, stream::cur); // -2 because we read them above
	}

}

JillTiles::~JillTiles()
{
}

int JillTiles::getCaps()
{
	return Tileset::ColourDepthVGA;
}

unsigned int JillTiles::getLayoutWidth()
{
	return 10;
}

ImagePtr JillTiles::createImageInstance(const EntryPtr& id,
	stream::inout_sptr content)
{
	FATEntryPtr pFAT = boost::dynamic_pointer_cast<FATEntry>(id);
	assert(pFAT);

	if (pFAT->size == 64 * 12 + 3) {
		// This could actually be palette data
		uint8_t width, height;
		content >> u8(width) >> u8(height);
		if ((width == 64) && (height == 12)) {
			// Yes, definitely a palette
			camoto::stream::sub_sptr sub(new stream::sub());
			sub->open(content, 3, 768, NULL); // NULL because this is a fixed size
			return ImagePtr(new VGAPalette(sub, 6));
		}
	}
	return ImagePtr(new JillImage(content, this->colourMap));
}

JillTiles::FATEntry *JillTiles::preInsertFile(
	const JillTiles::FATEntry *idBeforeThis, JillTiles::FATEntry *pNewEntry)
{
	if (this->items.size() >= 255) {
		throw stream::error("maximum number of tiles reached");
	}

	// Update the header
	this->data->seekp(0, stream::start);
	this->data << u8(this->items.size() + 1);

	return pNewEntry;
}

void JillTiles::postRemoveFile(const FATEntry *pid)
{
	// Update the header
	this->data->seekp(0, stream::start);
	this->data << u8(this->items.size());
	return;
}


//
// JillImage
//

JillImage::JillImage(stream::inout_sptr data, const StdImageDataPtr colourMap)
	:	VGAImage(data, 3), // 3 == skip width/height/flag fields
		colourMap(colourMap)
{
	this->data->seekg(0, stream::start);
	data >> u8(this->width) >> u8(this->height);
}

JillImage::~JillImage()
{
}

int JillImage::getCaps()
{
	return this->VGAImage::getCaps()
		| Image::CanSetDimensions;
}

void JillImage::getDimensions(unsigned int *width, unsigned int *height)
{
	*width = this->width;
	*height = this->height;
	return;
}

void JillImage::setDimensions(unsigned int width, unsigned int height)
{
	assert(this->getCaps() & Image::CanSetDimensions);
	if ((width == 64) && (height == 12)) {
		// We can't store images this size (in 8-bit/VGA mode at least) because
		// if the game encounters these dimensions the image data is immediately
		// loaded into the VGA palette registers instead!
		throw stream::error("Sorry, this tileset format cannot store images that "
			"are exactly 64x12 pixels in size.  You will have to use a different "
			"size.");
	}
	this->width = width;
	this->height = height;
	return;
}

StdImageDataPtr JillImage::toStandard()
{
	StdImageDataPtr img = this->VGAImage::toStandard();

	uint8_t *p = img.get();

	// Apply the colour map
	for (int i = 0; i < this->width * this->height; i++) {
		*p = this->colourMap[*p];
		p++;
	}

	return img;
}

void JillImage::fromStandard(StdImageDataPtr newContent,
	StdImageDataPtr newMask
)
{
	std::cerr << "ERROR: Cannot overwrite Jill images yet (no colourmap handling)" << std::endl;

	this->VGAImage::fromStandard(newContent, newMask);

	// Update dimensions
	this->data->seekp(0, stream::start);
	this->data << u8(this->width) << u8(this->height);

	uint8_t *img = new uint8_t[this->width * this->height];
	boost::shared_array<uint8_t> pimg(img);
	this->data->seekg(3, stream::start);
	this->data->read(img, this->width * this->height);

	// Apply the colour map
	uint8_t *p = img;
	for (int i = 0; i < this->width * this->height; i++) {
		*p = this->colourMap[*p];
		p++;
	}

	this->data->seekp(3, stream::start);
	this->data->write(img, this->width * this->height);

	return;
}


} // namespace gamegraphics
} // namespace camoto
