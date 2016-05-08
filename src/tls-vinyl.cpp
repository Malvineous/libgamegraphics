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
#include <camoto/stream_filtered.hpp>
#include <camoto/util.hpp> // make_unique
#include "img-vga-raw.hpp"
#include "pal-vga-raw.hpp"
#include "filter-vinyl-tileset.hpp"
#include "tls-vinyl.hpp"

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

/// Image implementation for masked tiles within a VGFM tileset.
/**
 * The data read by this class is not the same as the data in the tileset.
 * Rather filter-vinyl-tileset replaces all the codes with pixel values
 * (effectively decoding/decompressing the whole tileset), then we just read
 * the decoded values here.  We get a mask byte after every four expanded
 * pixels, so this class takes care of breaking those out into a separate mask
 * image.  But don't pay too much attention to how this class reads/writes
 * data, because it doesn't really correspond to any Vinyl file formats!
 */
class Image_VGFM_MaskedTile: virtual public Image
{
	public:
		/// Constructor
		/**
		 * @param content
		 *   VGFM tile data, without length header.
		 */
		Image_VGFM_MaskedTile(std::unique_ptr<stream::inout> content,
			std::shared_ptr<const Palette> pal)
			:	content(std::move(content))
		{
			this->pal = pal;
		}

		virtual ~Image_VGFM_MaskedTile()
		{
		}

		virtual Caps caps() const
		{
			return this->pal ? Caps::HasPalette : Caps::Default;
		}

		virtual ColourDepth colourDepth() const
		{
			return ColourDepth::VGA;
		}

		virtual Point dimensions() const
		{
			return {VGFM_TILE_WIDTH, VGFM_TILE_HEIGHT};
		}

		virtual Pixels convert() const
		{
			auto dims = this->dimensions();
			unsigned long dataSize = dims.x * dims.y;

			// Safety check to ensure supplied stream is long enough
			auto streamSize = this->content->size();
			if (streamSize < dataSize) {
				throw stream::error(createString("An image of " << dims.x << "x" << dims.y
						<< " requires " << dataSize << " bytes, but the supplied stream is only "
						<< streamSize << " bytes long."));
			}

			Pixels pix;
			pix.resize(dataSize);

			this->content->seekg(0, stream::start);
			auto pixbuf = pix.data();
			for (unsigned int i = 0; i < dataSize; i += 4) {
				// Skip over mask byte
				this->content->seekg(1, stream::cur);
				this->content->read(pixbuf, 4);
				pixbuf += 4;
			}
			return pix;
		}

		virtual Pixels convert_mask() const
		{
			auto dims = this->dimensions();
			unsigned int dataSize = dims.x * dims.y;

			Pixels pix;
			pix.resize(dataSize, 0);

			this->content->seekg(0, stream::start);
			for (unsigned int i = 0; i < dataSize; i += 4) {
				uint8_t mask;
				*this->content >> u8(mask);
				for (int j = 0; j < 4; j++) {
					// This algorithm only works with this enum value
					static_assert((int)Image::Mask::Transparent == 1, "Algorithm must be updated");

					pix[i + j] = (mask & 1) ^ 1;
					mask >>= 1;
				}
				// Skip over pixels
				this->content->seekg(4, stream::cur);
			}
			return pix;
		}

		virtual void convert(const Pixels& newContent, const Pixels& newMask)
		{
			auto dims = this->dimensions();
			unsigned long dataSize = dims.x * dims.y;

			// Safety check to ensure supplied stream is long enough
			auto streamSize = this->content->size();
			if (streamSize < dataSize) {
				throw stream::error(createString("An image of " << dims.x << "x" << dims.y
						<< " requires " << dataSize << " bytes, but the supplied stream is only "
						<< streamSize << " bytes long."));
			}

			this->content->seekp(0, stream::start);
			auto pixbuf = newContent.data();
			auto maskbuf = newMask.data();
			for (unsigned int i = 0; i < dataSize; i += 4) {
				uint8_t maskbyte = 0;
				for (int j = 0; j < 4; j++) {
					// This algorithm only works with this enum value
					static_assert((int)Image::Mask::Transparent == 1, "Algorithm must be updated");

					maskbyte |= ((*maskbuf++ & 1) ^ 1) << 4;
					maskbyte >>= 1;
				}
				this->content->write(&maskbyte, 1);
				this->content->write(pixbuf, 4);
				pixbuf += 4;
			}
			this->content->flush();
			return;
		}

	protected:
		std::unique_ptr<stream::inout> content; ///< Image content
};

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
		//virtual void flush();
		virtual void preInsertFile(const FATEntry *idBeforeThis,
			FATEntry *pNewEntry);
		virtual void postInsertFile(FATEntry *pNewEntry);
		virtual void postRemoveFile(const FATEntry *pid);

	private:
		/// Update the number of tiles in the tileset
		void updateFileCount(unsigned int newCount);
};

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
	if (len < VGFM_FIRST_TILE_OFFSET + 2) return Certainty::DefinitelyNo; // too short

	content.seekg(0, stream::start);
	uint16_t numTiles;
	content >> u16le(numTiles);
	// TESTED BY: tls_vinyl_isinstance_c02
	if (numTiles > VGFM_SAFETY_MAX_TILES) return Certainty::DefinitelyNo;

	stream::pos nextOffset = 2;
	if (numTiles > 0) {
		unsigned int size;
		for (unsigned int i = 0; i < numTiles; i++) {
			content
				>> u16le(size)
			;
			nextOffset += VGFM_FAT_ENTRY_LEN + size;

			// Truncated tile (or missing lookup table if last tile)
			// TESTED BY: tls_vinyl_isinstance_c03
			if (nextOffset + 2 >= len) return Certainty::DefinitelyNo;

			// Don't seek +VGFM_FAT_ENTRY_LEN here because we just read those bytes
			content.seekg(size, stream::cur);
		}
	}

	// Should not be possible to be here if there aren't another two bytes
	uint16_t lenLookup;
	content >> u16le(lenLookup);
	nextOffset += 2;
	if (lenLookup) {
		// Lookup table truncated
		// TESTED BY: tls_vinyl_isinstance_c04
		if (nextOffset + lenLookup > len) return Certainty::DefinitelyNo;
		nextOffset += lenLookup;
	}

	// If we're here, the file seems valid, so do some extra checks to try to
	// minimise false positives.
	// TESTED BY: tls_vinyl_isinstance_c05
	if (nextOffset != len) return Certainty::Unsure;

	// Empty file - we can't be quite so sure
	// TESTED BY: tls_vinyl_isinstance_c06
	if ((numTiles == 0) && (lenLookup == 0)) return Certainty::PossiblyYes;

	// TESTED BY: tls_vinyl_isinstance_c00
	return Certainty::DefinitelyYes;
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

	// Create a filter to handle the compression/expansion of the tile codes
	auto content_filtered = std::make_unique<stream::filtered>(
		std::move(content),
		std::make_shared<filter_vinyl_tileset_expand>(),
		std::make_shared<filter_vinyl_tileset_compress>(),
		nullptr
	);

	return std::make_shared<Tileset_Vinyl>(
		std::move(content_filtered),
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
	:	Tileset_FAT(std::move(content), VGFM_FIRST_TILE_OFFSET, ARCH_NO_FILENAMES)
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
}

Tileset_Vinyl::~Tileset_Vinyl()
{
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

std::unique_ptr<Image> Tileset_Vinyl::openImage(const FileHandle& id)
{
	auto fat = FATEntry::cast(id);
	switch (fat->storedSize) {
		case VGFM_USOLID_LEN:
			return std::make_unique<Image_VGARaw>(
				this->open(id, true), this->dimensions(), this->palette()
			);
		case VGFM_UMASKED_LEN:
			return std::make_unique<Image_VGFM_MaskedTile>(
				this->open(id, true), this->palette()
			);
	}
	throw stream::error("Tried to open tile of unknown size (not masked or solid)");
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
	return;
}

void Tileset_Vinyl::postRemoveFile(const FATEntry *pid)
{
	this->updateFileCount(this->vcFAT.size());
	return;
}

void Tileset_Vinyl::updateFileCount(unsigned int newCount)
{
	this->content->seekp(VGFM_TILECOUNT_OFFSET, stream::start);
	*this->content << u16le(newCount);
	return;
}

} // namespace gamegraphics
} // namespace camoto
