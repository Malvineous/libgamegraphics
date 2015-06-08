/**
 * @file  tls-harry-hsb.cpp
 * @brief Halloween Harry .HSB tileset handler.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/HSB_Format_(Halloween_Harry)
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
#include "tls-harry-hsb.hpp"
#include "pal-gmf-harry.hpp"
#include "img-vga-raw.hpp"
#include "tileset-fat.hpp"

/// Offset of first tile in an empty tileset
#define HSB_FIRST_TILE_OFFSET  0

/// Number of bytes in each image's header
#define HSB_HEADER_LEN 8

#define FILETYPE_HARRY_HSB   "tile/harry-hsb"

namespace camoto {
namespace gamegraphics {

/// Image inside Halloween Harry .HSB tileset
class Image_HarryHSB: virtual public Image_VGA
{
	public:
		/// Constructor
		/**
		 * Create an image from the supplied stream.
		 *
		 * @param content
		 *   VGA image data.
		 *
		 * @param pal
		 *   Image palette.
		 */
		Image_HarryHSB(std::unique_ptr<stream::inout> content,
			std::shared_ptr<const Palette> pal);
		virtual ~Image_HarryHSB();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual Point dimensions() const;
		virtual void dimensions(const Point& newDimensions);
		virtual void convert(const Pixels& newContent,
			const Pixels& newMask);

	protected:
		Point dims;
};


/// Halloween Harry .HSB tileset
class Tileset_HarryHSB: virtual public Tileset_FAT
{
	public:
		Tileset_HarryHSB(std::unique_ptr<stream::inout> content,
			std::shared_ptr<const Palette> pal);
		virtual ~Tileset_HarryHSB();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual unsigned int layoutWidth() const;

		// Tileset_FAT
		virtual void resize(FileHandle& id, stream::len newStoredSize,
			stream::len newRealSize);
		virtual std::unique_ptr<Image> openImage(FileHandle& id);
		virtual FileHandle insert(const FileHandle& idBeforeThis,
			File::Attribute attr);
		using Archive::insert;
};


TilesetType_HarryHSB::TilesetType_HarryHSB()
{
}

TilesetType_HarryHSB::~TilesetType_HarryHSB()
{
}

std::string TilesetType_HarryHSB::code() const
{
	return "tls-harry-hsb";
}

std::string TilesetType_HarryHSB::friendlyName() const
{
	return "Halloween Harry HSB tileset";
}

std::vector<std::string> TilesetType_HarryHSB::fileExtensions() const
{
	return {"hsb"};
}

std::vector<std::string> TilesetType_HarryHSB::games() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Alien Carnage");
	vcGames.push_back("Halloween Harry");
	return vcGames;
}

TilesetType_HarryHSB::Certainty TilesetType_HarryHSB::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();

	// Since there's no header, an empty file could mean an empty tileset.
	// TESTED BY: tls_harry_hsb_new_isinstance
	if (len == 0) return PossiblyYes;

	content.seekg(0, stream::start);
	while (len > HSB_HEADER_LEN) {
		uint16_t unk1, unk2;
		uint16_t width, height;
		content
			>> u16le(unk1)
			>> u16le(unk2)
			>> u16le(width)
			>> u16le(height)
		;
		len -= HSB_HEADER_LEN;
		unsigned int delta = width * height;

		// Width or height of zero don't really make sense
		// TESTED BY: fmt_harry_hsb_isinstance_c03
		if ((width == 0) || (height == 0)) return DefinitelyNo;

		// If this tile goes past EOF then it's not a valid file
		// TESTED BY: fmt_harry_hsb_isinstance_c01
		if (len < delta) return DefinitelyNo;
		len -= delta;

		content.seekg(delta, stream::cur);
	}

	// Trailing data
	// TESTED BY: fmt_harry_hsb_isinstance_c02
	if (len) return DefinitelyNo;

	// TESTED BY: fmt_harry_hsb_isinstance_c00
	return DefinitelyYes;
}

std::shared_ptr<Tileset> TilesetType_HarryHSB::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->truncate(0);
	return this->open(std::move(content), suppData);
}

std::shared_ptr<Tileset> TilesetType_HarryHSB::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	std::shared_ptr<const Palette> pal;
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		auto palFile = std::make_unique<Palette_HarryGMF>(
			std::move(suppData[SuppItem::Palette])
		);
		pal = palFile->palette();
	}
	return std::make_shared<Tileset_HarryHSB>(std::move(content), pal);
}

SuppFilenames TilesetType_HarryHSB::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	SuppFilenames supps;
	supps[SuppItem::Palette] = "m1z1.gmf"; // any map file
	return supps;
}


Image_HarryHSB::Image_HarryHSB(std::unique_ptr<stream::inout> content,
	std::shared_ptr<const Palette> pal)
	:	Image_VGA(std::move(content), HSB_HEADER_LEN)
{
	this->pal = pal;
	if (this->content->size() == 0) {
		// Newly inserted tile
		this->dims = {0, 0};
	} else {
		uint16_t unk1, unk2;
		this->content->seekg(0, stream::start);
		*this->content
			>> u16le(unk1)
			>> u16le(unk2)
			>> u16le(this->dims.x)
			>> u16le(this->dims.y)
		;
	}
}

Image_HarryHSB::~Image_HarryHSB()
{
}

Image::Caps Image_HarryHSB::caps() const
{
	return this->Image_VGA::caps()
		| Caps::HasPalette
		| Caps::SetDimensions;
}

ColourDepth Image_HarryHSB::colourDepth() const
{
	return ColourDepth::VGA;
}

Point Image_HarryHSB::dimensions() const
{
	return this->dims;
}

void Image_HarryHSB::dimensions(const Point& newDimensions)
{
	assert(this->caps() & Caps::SetDimensions);
	this->dims = newDimensions;
	return;
}

void Image_HarryHSB::convert(const Pixels& newContent,
	const Pixels& newMask)
{
	this->Image_VGA::convert(newContent, newMask);

	// Update dimensions
	this->content->seekp(0, stream::start);
	*this->content
		<< u16le(0) // TODO
		<< u16le(0) // TODO
		<< u16le(this->dims.x)
		<< u16le(this->dims.y)
	;
	return;
}


Tileset_HarryHSB::Tileset_HarryHSB(std::unique_ptr<stream::inout> content,
	std::shared_ptr<const Palette> pal)
	:	Tileset_FAT(std::move(content), HSB_FIRST_TILE_OFFSET, ARCH_NO_FILENAMES)
{
	this->pal = pal;

	stream::pos len = this->content->size();
	this->content->seekg(0, stream::start);

	unsigned int i = 0;
	stream::pos pos = 0;
	while (len > HSB_HEADER_LEN) {
		uint16_t unk1, unk2;
		uint16_t width, height;
		*this->content
			>> u16le(unk1)
			>> u16le(unk2)
			>> u16le(width)
			>> u16le(height)
		;
		auto fat = std::make_unique<FATEntry>();
		fat->storedSize = HSB_HEADER_LEN + width * height;

		// If this tile goes past EOF then ignore it
		if (len < fat->storedSize) break;
		len -= fat->storedSize;

		fat->bValid = true;
		fat->fAttr = File::Attribute::Default;
		fat->iIndex = i;
		fat->iOffset = pos;
		fat->lenHeader = 0;

		this->content->seekg(fat->storedSize - HSB_HEADER_LEN, stream::cur);
		pos += fat->storedSize;

		this->vcFAT.push_back(std::move(fat));

		i++;
	}
}

Tileset_HarryHSB::~Tileset_HarryHSB()
{
}

Tileset::Caps Tileset_HarryHSB::caps() const
{
	return Caps::HasPalette;
}

ColourDepth Tileset_HarryHSB::colourDepth() const
{
	return ColourDepth::VGA;
}

unsigned int Tileset_HarryHSB::layoutWidth() const
{
	return 1;
}

void Tileset_HarryHSB::resize(FileHandle& id, stream::len newStoredSize,
	stream::len newRealSize)
{
	auto fat = FATEntry::cast(id);
	assert(fat);

	auto targetSize = newStoredSize - HSB_HEADER_LEN;

	// Try to preserve the old width if possible
	uint16_t oldWidth = 0, oldHeight = 0;
	if (fat->iOffset + HSB_HEADER_LEN < this->content->size()) {
		// This tile already exists, read its height so we can try to match one
		// dimension
		this->content->seekg(fat->iOffset + 4, stream::start);
		*this->content
			>> u16le(oldWidth)
			>> u16le(oldHeight)
		;
	}

	// Avoid divide by zero
	if (oldWidth == 0) oldWidth++;
	if (oldHeight == 0) oldHeight++;

	// Find a width and height that can represent this amount
	unsigned int newWidth = 1, newHeight;
	if (targetSize % oldWidth == 0) {
		// Preserve width, extend height
		newWidth = oldWidth;
		newHeight = targetSize / newWidth;
	} else if (targetSize % oldHeight == 0) {
		// Preserve height, extend width
		newHeight = oldHeight;
		newWidth = targetSize / newHeight;
	} else {
		// Cannot preserve either height or width, just pick values that fit
		for (unsigned int x = 64; x > 1; x--) {
			if (targetSize % x == 0) {
				newWidth = x;
				break;
			}
		}
		// This will always work because worst-case newWidth == 1
		newHeight = targetSize / newWidth;
	}

	this->Tileset_FAT::resize(id, newStoredSize, newRealSize);

	// Write the new tile count, so the tileset won't be corrupted in case nothing
	// gets written to this new file.
	this->content->seekp(fat->iOffset, stream::start);
	*this->content
		<< u16le(0) // TODO
		<< u16le(0) // TODO
		<< u16le(newWidth)
		<< u16le(newHeight)
	;
	return;
}

std::unique_ptr<Image> Tileset_HarryHSB::openImage(FileHandle& id)
{
	return std::make_unique<Image_HarryHSB>(
		this->open(id, true), this->palette()
	);
}

Tileset::FileHandle Tileset_HarryHSB::insert(const FileHandle& idBeforeThis,
	File::Attribute attr)
{
	return this->insert(idBeforeThis, "", 0, FILETYPE_HARRY_HSB, attr);
}

} // namespace gamegraphics
} // namespace camoto
