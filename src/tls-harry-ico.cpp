/**
 * @file  tls-harry-ico.cpp
 * @brief Halloween Harry .ICO tileset handler.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/ICO_Format_(Halloween_Harry)
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
#include "tls-harry-ico.hpp"
#include "img-pcx.hpp"
#include "img-ddave.hpp"

namespace camoto {
namespace gamegraphics {

/// Offset of first tile in an empty tileset
#define ICO_FIRST_TILE_OFFSET  0

/// Size of each image's header (2x UINT16LE)
#define ICO_HEADER_LEN  4

#define FILETYPE_HARRY_ICO   "tile/harry-ico"

class Tileset_HarryICO: virtual public Tileset_FAT
{
	public:
		Tileset_HarryICO(std::unique_ptr<stream::inout> content,
			std::shared_ptr<const Palette> pal);
		virtual ~Tileset_HarryICO();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual unsigned int layoutWidth() const;

		// Tileset_FAT
		virtual void resize(const FileHandle& id, stream::len newStoredSize,
			stream::len newRealSize);
		virtual std::unique_ptr<Image> openImage(const FileHandle& id);
		virtual const FileHandle insert(const FileHandle& idBeforeThis,
			File::Attribute attr);
		using Archive::insert;
};

//
// TilesetType_HarryICO
//

TilesetType_HarryICO::TilesetType_HarryICO()
{
}

TilesetType_HarryICO::~TilesetType_HarryICO()
{
}

std::string TilesetType_HarryICO::code() const
{
	return "tls-harry-ico";
}

std::string TilesetType_HarryICO::friendlyName() const
{
	return "Halloween Harry ICO Tileset";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> TilesetType_HarryICO::fileExtensions() const
{
	return {"ico"};
}

std::vector<std::string> TilesetType_HarryICO::games() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Alien Carnage");
	vcGames.push_back("Halloween Harry");
	return vcGames;
}

TilesetType_HarryICO::Certainty TilesetType_HarryICO::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();

	// Since there's no header, an empty file could mean an empty tileset.
	// TESTED BY: tls_harry_ico_new_isinstance
	if (len == 0) return PossiblyYes;

	content.seekg(0, stream::start);
	while (len > ICO_HEADER_LEN) {
		uint16_t width, height;
		content
			>> u16le(width)
			>> u16le(height)
		;
		len -= ICO_HEADER_LEN;
		unsigned int delta = width * height;

		// Width or height of zero don't really make sense
		// TESTED BY: fmt_harry_ico_isinstance_c03
		if ((width == 0) || (height == 0)) return DefinitelyNo;

		// If this tile goes past EOF then it's not a valid file
		// TESTED BY: fmt_harry_ico_isinstance_c01
		if (len < delta) return DefinitelyNo;
		len -= delta;

		content.seekg(delta, stream::cur);
	}

	// Trailing data
	// TESTED BY: fmt_harry_ico_isinstance_c02
	if (len) return DefinitelyNo;

	// TESTED BY: fmt_harry_ico_isinstance_c00
	return DefinitelyYes;
}

std::shared_ptr<Tileset> TilesetType_HarryICO::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->truncate(0);
	return this->open(std::move(content), suppData);
}

std::shared_ptr<Tileset> TilesetType_HarryICO::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	std::shared_ptr<const Palette> pal;
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		auto palFile = std::make_unique<Image_PCX>(
			std::move(suppData[SuppItem::Palette]), 8, 1, true
		);
		pal = palFile->palette();
	}
	return std::make_shared<Tileset_HarryICO>(std::move(content), pal);
}

SuppFilenames TilesetType_HarryICO::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	SuppFilenames supps;
	supps[SuppItem::Palette] = "pre2.pcx"; // any UI image file
	return supps;
}


//
// Tileset_HarryICO
//

Tileset_HarryICO::Tileset_HarryICO(std::unique_ptr<stream::inout> content,
	std::shared_ptr<const Palette> pal)
	:	Tileset_FAT(std::move(content), ICO_FIRST_TILE_OFFSET, ARCH_NO_FILENAMES)
{
	this->pal = pal;

	stream::pos len = this->content->size();
	this->content->seekg(0, stream::start);

	unsigned int i = 0;
	stream::pos pos = 0;
	while (len > ICO_HEADER_LEN) {
		uint16_t width, height;
		*this->content
			>> u16le(width)
			>> u16le(height)
		;
		auto fat = std::make_unique<FATEntry>();
		fat->storedSize = ICO_HEADER_LEN + width * height;

		// If this tile goes past EOF then ignore it
		if (len < fat->storedSize) break;
		len -= fat->storedSize;

		fat->bValid = true;
		fat->fAttr = File::Attribute::Default;
		fat->iIndex = i;
		fat->iOffset = pos;
		fat->lenHeader = 0;

		this->content->seekg(fat->storedSize - ICO_HEADER_LEN, stream::cur);
		pos += fat->storedSize;

		this->vcFAT.push_back(std::move(fat));

		i++;
	}
}

Tileset_HarryICO::~Tileset_HarryICO()
{
}

Tileset::Caps Tileset_HarryICO::caps() const
{
	return Caps::HasPalette;
}

ColourDepth Tileset_HarryICO::colourDepth() const
{
	return ColourDepth::VGA;
}

unsigned int Tileset_HarryICO::layoutWidth() const
{
	return 16;
}

void Tileset_HarryICO::resize(const FileHandle& id, stream::len newStoredSize,
	stream::len newRealSize)
{
	auto fat = FATEntry::cast(id);
	assert(fat);

	auto targetSize = newStoredSize - ICO_HEADER_LEN;

	// Try to preserve the old width if possible
	uint16_t oldWidth = 0, oldHeight = 0;
	if (fat->iOffset + ICO_HEADER_LEN < this->content->size()) {
		// This tile already exists, read its height so we can try to match one
		// dimension
		this->content->seekg(fat->iOffset, stream::start);
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
		<< u16le(newWidth)
		<< u16le(newHeight)
	;
	return;
}

std::unique_ptr<Image> Tileset_HarryICO::openImage(const FileHandle& id)
{
	return std::make_unique<Image_DDaveVGA>(
		this->open(id, true), false, this->palette()
	);
}

const Tileset::FileHandle Tileset_HarryICO::insert(const FileHandle& idBeforeThis,
	File::Attribute attr)
{
	return this->insert(idBeforeThis, "", 0, FILETYPE_HARRY_ICO, attr);
}

} // namespace gamegraphics
} // namespace camoto
