/**
 * @file  tls-actrinfo.cpp
 * @brief Cosmo actor tileset
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Cosmo_Tileinfo_Format
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
#include <camoto/gamegraphics/palette.hpp>
#include "tls-actrinfo.hpp"
#include "tileset-fat.hpp"
#include "image-from_tileset.hpp"
#include "tls-ega-apogee.hpp"

/// Number of planes in each image
#define ACTR_NUMPLANES  5

/// Offset of first tileset in a tileset collection.
#define ACTR_FIRST_TILE_OFFSET 0

/// Offset of first tile/frame in a single actor tileset.
#define ACTR_SINGLE_FIRST_TILE_OFFSET 0

/// Size of each tile within a single actor frame, in pixels.
#define ACTR_TILE_WIDTH 8
#define ACTR_TILE_HEIGHT 8

namespace camoto {
namespace gamegraphics {

/// Tileset for the full list of actors, one sub-tileset for each actor.
class Tileset_Actrinfo: virtual public Tileset_FAT
{
	public:
		Tileset_Actrinfo(std::unique_ptr<stream::inout> content,
			std::unique_ptr<stream::inout> contentTileset, const Point& tileSize,
			std::shared_ptr<const Palette> pal);
		virtual ~Tileset_Actrinfo();

		// Archive
		virtual std::shared_ptr<Archive> openFolder(const FileHandle& id);

		// Tileset
		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual unsigned int layoutWidth() const;

		// Tileset_FAT
		virtual std::shared_ptr<Tileset> openTileset(const FileHandle& id);

	protected:
		/// Actual 8x8 tiles comprising the actors (e.g. ACTORS.MNI)
		std::shared_ptr<Tileset> actorTiles;

		/// Tile dimensions, in pixels (e.g. 8x8), for calculating index from offset
		Point tileSize;
};

/// Tileset containing all the frames for a single actor.
class Tileset_SingleActor: virtual public Tileset_FAT
{
	public:
		Tileset_SingleActor(std::unique_ptr<stream::inout> content,
			std::shared_ptr<Tileset> actorTiles, const Point& tileSize);
		virtual ~Tileset_SingleActor();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual unsigned int layoutWidth() const;
		virtual std::shared_ptr<const Palette> palette() const;

		void resize(const FileHandle& id, stream::len newSize);

		// Tileset_FAT
		virtual std::unique_ptr<Image> openImage(const FileHandle& id);

		class SingleActorEntry: virtual public FATEntry {
			public:
				Point dims; // Image size, in tiles
		};

	protected:
		/// Tile arrangement data (e.g. ACTRINFO.MNI)
		std::shared_ptr<Tileset_Actrinfo> base;

		/// Actual 8x8 tiles comprising the actors (e.g. ACTORS.MNI)
		std::shared_ptr<Tileset> actorTiles;

		/// Tile dimensions, in pixels (e.g. 8x8), for calculating index from offset
		Point tileSize;
};

//
// TilesetType_Actrinfo
//

TilesetType_Actrinfo::TilesetType_Actrinfo()
{
}

TilesetType_Actrinfo::~TilesetType_Actrinfo()
{
}

std::string TilesetType_Actrinfo::code() const
{
	return "tls-actrinfo";
}

std::string TilesetType_Actrinfo::friendlyName() const
{
	return "Cosmo Actor Tileset";
}

std::vector<std::string> TilesetType_Actrinfo::fileExtensions() const
{
	return {"mni"};
}

std::vector<std::string> TilesetType_Actrinfo::games() const
{
	return {
		"Cosmo's Cosmic Adventures",
	};
}

TilesetType_Actrinfo::Certainty TilesetType_Actrinfo::isInstance(
	stream::input& content) const
{
	return Unsure;
}

std::shared_ptr<Tileset> TilesetType_Actrinfo::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->seekp(0, stream::start);
	// Zero tiles, 0x0
	return this->open(std::move(content), suppData);
}

std::shared_ptr<Tileset> TilesetType_Actrinfo::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	if (suppData.find(SuppItem::FAT) == suppData.end()) {
		throw stream::error("no actor info file specified (missing supplementary item)");
	}
	return std::make_shared<Tileset_Actrinfo>(
		std::move(suppData[SuppItem::FAT]),
		std::move(content),
		Point{ACTR_TILE_WIDTH, ACTR_TILE_HEIGHT},
		nullptr
	);
}

SuppFilenames TilesetType_Actrinfo::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	SuppFilenames supps;
	if (filename.compare("players.mni") == 0) {
		supps[SuppItem::FAT] = "plyrinfo.mni";
	} else if (filename.compare("cartoon.mni") == 0) {
		supps[SuppItem::FAT] = "cartinfo.mni";
	} else {
		supps[SuppItem::FAT] = "actrinfo.mni";
	}
	return supps;
}


//
// Tileset_Actrinfo
//

Tileset_Actrinfo::Tileset_Actrinfo(std::unique_ptr<stream::inout> content,
	std::unique_ptr<stream::inout> contentTileset, const Point& tileSize,
	std::shared_ptr<const Palette> pal)
	:	Tileset_FAT(std::move(content), ACTR_FIRST_TILE_OFFSET, ARCH_NO_FILENAMES),
		actorTiles(
			std::make_shared<Tileset_EGAApogee>(
				std::move(contentTileset),
				tileSize,
				PlaneCount::Masked, 40, pal
			)
		),
		tileSize(tileSize)
{
	this->pal = pal;

	auto lenContent = this->content->size();
	if (lenContent > 2) {
		this->content->seekg(0, stream::start);
		stream::pos nextOffset;
		*this->content >> u16le(nextOffset);

		unsigned int numImages = nextOffset;

		// The file data is loaded in lots of 65535 bytes, into memory blocks of
		// 65536 bytes.  This means after every 65535 bytes, a padding byte should be
		// inserted in order for the offsets to add up correctly.  Likewise when
		// saving data, every 65536th byte should be dropped.
		nextOffset *= 2;
		nextOffset -= nextOffset / 65536;

		if (lenContent < numImages * 2) {
			throw stream::error("Actor info FAT truncated");
		}
		this->vcFAT.reserve(numImages);
		for (unsigned int i = 0; i < numImages; i++) {
			auto fat = std::make_unique<FATEntry>();
			fat->bValid = true;
			fat->fAttr = File::Attribute::Folder;
			fat->iIndex = i;
			fat->lenHeader = 0;
			fat->iOffset = nextOffset;
			if (i == numImages - 1) {
				nextOffset = lenContent;
			} else {
				*this->content >> u16le(nextOffset);
				nextOffset *= 2;
				nextOffset -= nextOffset / 65536;
			}
			if (nextOffset < fat->iOffset) {
				throw stream::error(createString("Invalid end offset for actor "
					<< i << " (actor size would be negative - offset is "
					<< fat->iOffset << ", end offset is " << nextOffset << ")"));
			}
			fat->realSize = fat->storedSize = nextOffset - fat->iOffset;
			if (fat->storedSize == 0) {
				fat->fAttr = File::Attribute::Vacant;
			}
			this->vcFAT.push_back(std::move(fat));
		}
	}

}

Tileset_Actrinfo::~Tileset_Actrinfo()
{
}

std::shared_ptr<gamearchive::Archive> Tileset_Actrinfo::openFolder(const FileHandle& id)
{
	return this->openTileset(id);
}

Tileset::Caps Tileset_Actrinfo::caps() const
{
	return (this->pal ? Caps::HasPalette : Caps::Default);
}

ColourDepth Tileset_Actrinfo::colourDepth() const
{
	return this->actorTiles->colourDepth();
}

unsigned int Tileset_Actrinfo::layoutWidth() const
{
	return 1;
}

std::shared_ptr<Tileset> Tileset_Actrinfo::openTileset(const FileHandle& id)
{
	return std::make_shared<Tileset_SingleActor>(
		this->open(id, true),
		this->actorTiles,
		this->tileSize
	);
}


//
// Tileset_SingleActor
//

Tileset_SingleActor::Tileset_SingleActor(std::unique_ptr<stream::inout> content,
	std::shared_ptr<Tileset> actorTiles, const Point& tileSize)
	:	Tileset_FAT(std::move(content), ACTR_SINGLE_FIRST_TILE_OFFSET, ARCH_NO_FILENAMES),
		actorTiles(actorTiles),
		tileSize(tileSize)
{
	stream::pos lenFAT = this->content->size();
	this->content->seekg(0, stream::start);

	stream::len lenRemaining = lenFAT;
	SingleActorEntry *fatLast = nullptr;
	unsigned int i = 0;
	while (lenRemaining >= 8) {
		SingleActorEntry *fat = new SingleActorEntry();
		FileHandle ep(fat);
		fat->bValid = true;
		fat->fAttr = File::Attribute::Default;
		fat->iIndex = i++;
		fat->lenHeader = 0;
		*this->content
			>> u16le(fat->dims.y)
			>> u16le(fat->dims.x)
			>> u32le(fat->iOffset)
		;
		// Adjust for memory alignment
		fat->iOffset -= fat->iOffset / 65536;
		if (fatLast) {
			fatLast->storedSize = fat->iOffset - fatLast->iOffset;
			if (fatLast->storedSize == 0) {
				fatLast->fAttr = File::Attribute::Vacant;
			}
		}

		this->vcFAT.push_back(ep);
		lenRemaining -= 8;
		fatLast = fat;
	}
	if (fatLast) {
		stream::pos lenData = this->content->size();
		fatLast->storedSize = lenData - fatLast->iOffset;
	}
}

Tileset_SingleActor::~Tileset_SingleActor()
{
}

Tileset::Caps Tileset_SingleActor::caps() const
{
	return Caps::Default
		| ((this->actorTiles->caps() & Caps::HasPalette) ? Caps::HasPalette : Caps::Default);
}

ColourDepth Tileset_SingleActor::colourDepth() const
{
	return this->actorTiles->colourDepth();
}

void Tileset_SingleActor::resize(const FileHandle& id, stream::len newSize)
{
	auto fat = FATEntry::cast(id);
	assert(fat);

	if (newSize != fat->storedSize) {
		throw stream::error("tiles in this tileset are a fixed size");
	}
	return;
}

unsigned int Tileset_SingleActor::layoutWidth() const
{
	return 1;
}

std::shared_ptr<const Palette> Tileset_SingleActor::palette() const
{
	return this->actorTiles->palette();
}

std::unique_ptr<Image> Tileset_SingleActor::openImage(const FileHandle& id)
{
	auto fat = dynamic_cast<const SingleActorEntry *>(id.get());
	assert(fat);

	unsigned int firstTile = fat->iOffset /
		(this->tileSize.x * this->tileSize.y * ACTR_NUMPLANES / 8);

	return std::make_unique<Image_FromTileset>(
		this->actorTiles, firstTile, fat->dims.x, fat->dims
	);
}

} // namespace gamegraphics
} // namespace camoto
