/**
 * @file   tls-got.cpp
 * @brief  God of Thunder tileset handler.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/God_of_Thunder
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
#include "tls-got.hpp"
#include "pal-vga-raw.hpp"
#include "img-vga-planar.hpp"

/// Offset of first tile in an empty tileset
#define GOT_FIRST_TILE_OFFSET  0

/// Number of bytes in the header
#define GOT_HEADER_LEN 6

namespace camoto {
namespace gamegraphics {


/// Raw VGA Image implementation.
class GOTImage: virtual public VGAPlanarImage
{
	public:
		/// Constructor
		/**
		 * No truncate function is required as the image dimensions are fixed, so
		 * the file size will always remain constant.
		 *
		 * @param data
		 *   VGA data
		 *
		 * @param width
		 *   Image width
		 *
		 * @param height
		 *   Image height
		 *
		 * @param pal
		 *   Image palette
		 */
		GOTImage(stream::inout_sptr data, PaletteTablePtr pal);
		virtual ~GOTImage();

		virtual int getCaps();
		virtual void getDimensions(unsigned int *width, unsigned int *height);
		virtual void setDimensions(unsigned int width, unsigned int height);
		virtual PaletteTablePtr getPalette();

	protected:
		PaletteTablePtr pal;
};

GOTImage::GOTImage(stream::inout_sptr data, PaletteTablePtr pal)
	:	VGAPlanarImage(data, GOT_HEADER_LEN),
		pal(pal)
{
}

GOTImage::~GOTImage()
{
}

int GOTImage::getCaps()
{
	return ColourDepthVGA | HasPalette;
}

void GOTImage::getDimensions(unsigned int *width, unsigned int *height)
{
	this->data->seekg(0, stream::start);
	this->data
		>> u16le(*width)
		>> u16le(*height)
	;
	*width *= 4;
	return;
}

void GOTImage::setDimensions(unsigned int width, unsigned int height)
{
	if (width % 4 != 0) {
		throw stream::error("God of Thunder tiles can only have a width that is a multiple of 4.");
	}
	this->data->seekp(0, stream::start);
	width /= 4;
	this->data
		<< u16le(width)
		<< u16le(height)
	;
	return;
}

PaletteTablePtr GOTImage::getPalette()
{
	return this->pal;
}


GOTTilesetType::GOTTilesetType()
{
}

GOTTilesetType::~GOTTilesetType()
{
}

std::string GOTTilesetType::getCode() const
{
	return "tls-got";
}

std::string GOTTilesetType::getFriendlyName() const
{
	return "God of Thunder tileset";
}

std::vector<std::string> GOTTilesetType::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	return vcExtensions;
}

std::vector<std::string> GOTTilesetType::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("God of Thunder");
	return vcGames;
}

GOTTilesetType::Certainty GOTTilesetType::isInstance(
	stream::input_sptr psTileset) const
{
	stream::pos len = psTileset->size();

	psTileset->seekg(0, stream::start);
	while (len) {
		int width, height;
		psTileset
			>> u16le(width)
			>> u16le(height)
		;
		len -= 6; // including extra field not read above
		width *= 4;
		unsigned int lenTile = width * height;

		// Width out of range
		// TESTED BY: tls_got_isinstance_c01
		if (width > 320) return DefinitelyNo;

		// Height out of range
		// TESTED BY: tls_got_isinstance_c02
		if (width > 200) return DefinitelyNo;

		// Data truncated
		// TESTED BY: tls_got_isinstance_c03
		if (len < lenTile) return DefinitelyNo;

		len -= lenTile;
		psTileset->seekg(lenTile + 2, stream::cur);
	}

	// TESTED BY: tls_got_isinstance_c00
	return DefinitelyYes;
}

TilesetPtr GOTTilesetType::create(stream::inout_sptr psTileset,
	SuppData& suppData) const
{
	psTileset->truncate(0);
	psTileset->seekp(0, stream::start);

	PaletteTablePtr pal;
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new VGAPalette(suppData[SuppItem::Palette], 6));
		pal = palFile->getPalette();
	} else {
		throw stream::error("no palette specified (missing supplementary item)");
	}
	(*pal)[0].alpha = 0;
	(*pal)[15].alpha = 0;
	return TilesetPtr(new GOTTileset(psTileset, pal));
}

TilesetPtr GOTTilesetType::open(stream::inout_sptr psTileset,
	SuppData& suppData) const
{
	PaletteTablePtr pal;
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new VGAPalette(suppData[SuppItem::Palette], 8));
		pal = palFile->getPalette();
	} else {
		throw stream::error("no palette specified (missing supplementary item)");
	}
	(*pal)[0].alpha = 0;
	(*pal)[15].alpha = 0;
	return TilesetPtr(new GOTTileset(psTileset, pal));
}

SuppFilenames GOTTilesetType::getRequiredSupps(
	const std::string& filenameTileset) const
{
	SuppFilenames supps;
	supps[SuppItem::Palette] = "palette";
	return supps;
}


GOTTileset::GOTTileset(stream::inout_sptr data,
	PaletteTablePtr pal)
	:	FATTileset(data, GOT_FIRST_TILE_OFFSET),
		pal(pal)
{
	assert(this->pal);

	stream::len len = this->data->size();
	stream::pos off = 0;
	this->data->seekg(0, stream::start);
	unsigned int i = 0;
	while (len > 6) {
		unsigned int width, height;
		FATEntry *fat = new FATEntry();
		EntryPtr ep(fat);
		fat->valid = true;
		fat->attr = 0;
		fat->index = i++;
		fat->offset = off;
		fat->lenHeader = 0;
		this->data
			>> u16le(width)
			>> u16le(height)
		;
		width *= 4;
		fat->size = width * height + GOT_HEADER_LEN;
		off += fat->size;
		len -= fat->size;
		this->data->seekg(fat->size - 4, stream::cur);
		this->items.push_back(ep);
	}
}

GOTTileset::~GOTTileset()
{
}

int GOTTileset::getCaps()
{
	return Tileset::HasPalette | Tileset::ColourDepthVGA;
}

unsigned int GOTTileset::getLayoutWidth()
{
	return 8;
}

PaletteTablePtr GOTTileset::getPalette()
{
	return this->pal;
}

ImagePtr GOTTileset::createImageInstance(const EntryPtr& id,
	stream::inout_sptr content)
{
	ImagePtr img(new GOTImage(content, this->pal));
	return img;
}

} // namespace gamegraphics
} // namespace camoto
