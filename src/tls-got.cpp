/**
 * @file  tls-got.cpp
 * @brief God of Thunder tileset handler.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/God_of_Thunder
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
#include "tls-got.hpp"
#include "pal-vga-raw.hpp"
#include "tileset-fat.hpp"

/// Offset of first tile in an empty tileset
#define GOT_FIRST_TILE_OFFSET  0

/// Number of bytes in the header
#define GOT_HEADER_LEN 6

#define FILETYPE_GOT "tile/got"

namespace camoto {
namespace gamegraphics {

class Tileset_GOT: virtual public Tileset_FAT
{
	public:
		Tileset_GOT(std::unique_ptr<stream::inout> content,
			std::shared_ptr<const Palette> pal);
		virtual ~Tileset_GOT();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual unsigned int layoutWidth() const;

		// Archive
		virtual void resize(FileHandle& id, stream::len newStoredSize,
			stream::len newRealSize);

		// Tileset_FAT
		virtual std::unique_ptr<Image> openImage(FileHandle& id);
		virtual FileHandle insert(const FileHandle& idBeforeThis,
			File::Attribute attr);
		using Archive::insert;
};

Image_GOT::Image_GOT(std::unique_ptr<stream::inout> content,
	std::shared_ptr<const Palette> pal)
	:	Image_VGA_Planar(std::move(content), GOT_HEADER_LEN)
{
	this->pal = pal;
}

Image_GOT::~Image_GOT()
{
}

Image::Caps Image_GOT::caps() const
{
	return Caps::HasPalette | Caps::SetDimensions;
}

ColourDepth Image_GOT::colourDepth() const
{
	return ColourDepth::VGA;
}

Point Image_GOT::dimensions() const
{
	this->content->seekg(0, stream::start);
	uint16_t width, height;
	*this->content
		>> u16le(width)
		>> u16le(height)
	;
	return {width * 4, height};
}

void Image_GOT::dimensions(const Point& newDimensions)
{
	if (newDimensions.x % 4 != 0) {
		throw stream::error("God of Thunder tiles can only have a width that is a "
			"multiple of 4.");
	}
	this->content->seekp(0, stream::start);
	unsigned int width = newDimensions.x / 4;
	*this->content
		<< u16le(width)
		<< u16le(newDimensions.y)
	;
	return;
}


TilesetType_GOT::TilesetType_GOT()
{
}

TilesetType_GOT::~TilesetType_GOT()
{
}

std::string TilesetType_GOT::code() const
{
	return "tls-got";
}

std::string TilesetType_GOT::friendlyName() const
{
	return "God of Thunder tileset";
}

std::vector<std::string> TilesetType_GOT::fileExtensions() const
{
	return {};
}

std::vector<std::string> TilesetType_GOT::games() const
{
	return {
		"God of Thunder"
	};
}

TilesetType_GOT::Certainty TilesetType_GOT::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();

	// Empty tileset
	// TESTED BY: tls_got_isinstance_c04
	if (len == 0) return PossiblyYes;

	content.seekg(0, stream::start);
	while (len) {
		int width, height;
		content
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
		content.seekg(lenTile + 2, stream::cur);
	}

	// TESTED BY: tls_got_isinstance_c00
	return DefinitelyYes;
}

std::shared_ptr<Tileset> TilesetType_GOT::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->truncate(0);
	content->seekp(0, stream::start);

	return this->open(std::move(content), suppData);
}

std::shared_ptr<Tileset> TilesetType_GOT::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	auto pal = std::make_shared<Palette>();
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		auto palFile = std::make_unique<Palette_VGA>(
			std::move(suppData[SuppItem::Palette]), 8
		);
		*pal = *palFile->palette();

		// Make colours 0 and 15 transparent
		(*pal)[0].alpha = 0;
		(*pal)[15].alpha = 0;
	}
	return std::make_shared<Tileset_GOT>(std::move(content), pal);
}

SuppFilenames TilesetType_GOT::getRequiredSupps(
	const std::string& filenameTileset) const
{
	SuppFilenames supps;
	supps[SuppItem::Palette] = "palette";
	return supps;
}


Tileset_GOT::Tileset_GOT(std::unique_ptr<stream::inout> content,
	std::shared_ptr<const Palette> pal)
	:	Tileset_FAT(std::move(content), GOT_FIRST_TILE_OFFSET, ARCH_NO_FILENAMES)
{
	this->pal = pal;

	stream::len len = this->content->size();
	stream::pos off = 0;
	this->content->seekg(0, stream::start);
	unsigned int i = 0;
	while (len > 6) {
		unsigned int width, height;
		FATEntry *fat = new FATEntry();
		FileHandle ep(fat);
		fat->bValid = true;
		fat->fAttr = File::Attribute::Default;
		fat->iIndex = i++;
		fat->iOffset = off;
		fat->lenHeader = 0;
		*this->content
			>> u16le(width)
			>> u16le(height)
		;
		width *= 4;
		fat->storedSize = width * height + GOT_HEADER_LEN;
		off += fat->storedSize;
		len -= fat->storedSize;
		this->content->seekg(fat->storedSize - 4, stream::cur);
		this->vcFAT.push_back(ep);
	}
}

Tileset_GOT::~Tileset_GOT()
{
}

Tileset::Caps Tileset_GOT::caps() const
{
	return Caps::HasPalette;
}

ColourDepth Tileset_GOT::colourDepth() const
{
	return ColourDepth::VGA;
}

unsigned int Tileset_GOT::layoutWidth() const
{
	return 8;
}

void Tileset_GOT::resize(FileHandle& id, stream::len newStoredSize,
	stream::len newRealSize)
{
	auto fat = FATEntry::cast(id);
	assert(fat);

	auto targetSize = newStoredSize - GOT_HEADER_LEN;

	// Find a width and height that can represent this amount
	unsigned int newWidth = 1, newHeight;
	for (unsigned int x = 4 * 8; x > 0; x -= 4) {
		if (targetSize % x == 0) {
			newWidth = x;
			break;
		}
	}
	if (newWidth < 4) {
		throw stream::error(createString(
			"GOT tiles cannot store exactly " << newStoredSize << " bytes."
		));
	}
	newHeight = targetSize / newWidth;
	newWidth /= 4;

	this->Tileset_FAT::resize(id, newStoredSize, newRealSize);

	// Write the new tile count, so the tileset won't be corrupted in case nothing
	// gets written to this new file.
	this->content->seekp(fat->iOffset, stream::start);
	*this->content
		<< u16le(newWidth)
		<< u16le(newHeight)
	;
	return;
}

std::unique_ptr<Image> Tileset_GOT::openImage(FileHandle& id)
{
	return std::make_unique<Image_GOT>(this->open(id, true), this->pal);
}

Tileset::FileHandle Tileset_GOT::insert(const FileHandle& idBeforeThis,
	File::Attribute attr)
{
	return this->insert(idBeforeThis, "", 6, FILETYPE_GOT, attr);
}

} // namespace gamegraphics
} // namespace camoto
