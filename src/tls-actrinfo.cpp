/**
 * @file   tls-actrinfo.cpp
 * @brief  Cosmo/Duke II actor tileset
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

#include <camoto/iostream_helpers.hpp>
#include <camoto/gamegraphics/palettetable.hpp>
#include "tileset-fat.hpp"
#include "img-ega-byteplanar-tiled.hpp"
#include "tls-actrinfo.hpp"

/// Number of planes in each image
#define ACTR_NUMPLANES  5

/// Offset of first tileset in a tileset collection.
#define ACTR_FIRST_TILE_OFFSET 0

/// Offset of first tile/frame in a single actor tileset.
#define ACTR_SINGLE_FIRST_TILE_OFFSET 0

/// Size of each tile within a single actor frame, in pixels.
#define ACTR_TILE_WIDTH 8
#define ACTR_TILE_HEIGHT 8

/// Size of each tile within a single actor frame, in bytes.
#define ACTR_TILE_SIZE (ACTR_TILE_WIDTH / 8 * ACTR_TILE_HEIGHT * ACTR_NUMPLANES)

#include <camoto/gamegraphics/tilesettype.hpp>
#include <camoto/gamegraphics/palettetable.hpp>
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

/// Tileset for the full list of actors, one sub-tileset for each actor.
class ActrinfoTileset: virtual public FATTileset
{
	public:
		ActrinfoTileset(stream::inout_sptr dataInfo, stream::inout_sptr dataTiles,
			PaletteTablePtr pal);
		virtual ~ActrinfoTileset();

		virtual int getCaps();
		void resize(EntryPtr& id, stream::len newSize);
		virtual unsigned int getLayoutWidth();
		virtual PaletteTablePtr getPalette();

		// FATTileset
		virtual TilesetPtr createTilesetInstance(const EntryPtr& id,
			stream::inout_sptr content);

	protected:
		stream::inout_sptr dataTiles;  ///< ACTORS.MNI or equivalent
		PaletteTablePtr pal;
};

/// Tileset containing each frame for a single actor.
class SingleActorTileset: virtual public FATTileset
{
	public:
		SingleActorTileset(stream::inout_sptr fat, stream::inout_sptr data,
			PaletteTablePtr pal);
		virtual ~SingleActorTileset();

		virtual int getCaps();
		void resize(EntryPtr& id, stream::len newSize);
		virtual unsigned int getLayoutWidth();
		virtual PaletteTablePtr getPalette();

		// FATTileset
		virtual ImagePtr createImageInstance(const EntryPtr& id,
			stream::inout_sptr content);

		class ActorEntry: virtual public FATEntry {
			public:
				unsigned int width;  ///< Image width, in tiles
				unsigned int height; ///< Image height, in tiles
		};

	protected:
		stream::inout_sptr dataInfo;  ///< ACTRINFO.MNI or equivalent
		PaletteTablePtr pal;
};

//
// ActrinfoTilesetType
//

ActrinfoTilesetType::ActrinfoTilesetType()
{
}

ActrinfoTilesetType::~ActrinfoTilesetType()
{
}

std::string ActrinfoTilesetType::getCode() const
{
	return "tls-actrinfo";
}

std::string ActrinfoTilesetType::getFriendlyName() const
{
	return "Cosmo Actor Tileset";
}

std::vector<std::string> ActrinfoTilesetType::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("mni");
	return vcExtensions;
}

std::vector<std::string> ActrinfoTilesetType::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Cosmo's Cosmic Adventures");
	return vcGames;
}

ActrinfoTilesetType::Certainty ActrinfoTilesetType::isInstance(stream::input_sptr psGraphics) const
{
	return Unsure;
}

TilesetPtr ActrinfoTilesetType::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	psGraphics->seekp(0, stream::start);
	// Zero tiles, 0x0
	if (suppData.find(SuppItem::FAT) == suppData.end()) {
		throw stream::error("no actor info file specified (missing supplementary item)");
	}
	return TilesetPtr(
		new ActrinfoTileset(suppData[SuppItem::FAT], psGraphics, PaletteTablePtr())
	);
}

TilesetPtr ActrinfoTilesetType::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	if (suppData.find(SuppItem::FAT) == suppData.end()) {
		throw stream::error("no actor info file specified (missing supplementary item)");
	}
	return TilesetPtr(
		new ActrinfoTileset(suppData[SuppItem::FAT], psGraphics, PaletteTablePtr())
	);
}

SuppFilenames ActrinfoTilesetType::getRequiredSupps(const std::string& filenameGraphics) const
{
	SuppFilenames supps;
	if (filenameGraphics.compare("players.mni") == 0) {
		supps[SuppItem::FAT] = "plyrinfo.mni";
	} else if (filenameGraphics.compare("cartoon.mni") == 0) {
		supps[SuppItem::FAT] = "cartinfo.mni";
	} else {
		supps[SuppItem::FAT] = "actrinfo.mni";
	}
	return supps;
}


//
// ActrinfoTileset
//

ActrinfoTileset::ActrinfoTileset(stream::inout_sptr dataInfo,
	stream::inout_sptr dataTiles, PaletteTablePtr pal)
	:	FATTileset(dataInfo, ACTR_FIRST_TILE_OFFSET),
		dataTiles(dataTiles),
		pal(pal)
{
	this->data->seekg(0, stream::start);
	std::vector<unsigned int> offsets;
	unsigned int nextOffset;
	unsigned int numImages = 1;
	for (unsigned int i = 0; i < numImages; i++) {
		this->data >> u16le(nextOffset);
		nextOffset -= nextOffset / 65536;
		if (i == 0) numImages = nextOffset;
		nextOffset *= 2;
		offsets.push_back(nextOffset);
	}
	stream::len lenInfo = dataInfo->size();
	offsets.push_back(lenInfo);

	this->items.reserve(numImages);
	std::vector<unsigned int>::const_iterator oi = offsets.begin();
	for (unsigned int i = 0; i < numImages; i++) {
		FATEntry *fat = new FATEntry();
		EntryPtr ep(fat);
		fat->valid = true;
		fat->attr = Tileset::SubTileset;
		fat->index = i;
		fat->lenHeader = 0;
		fat->offset = *oi++;
		fat->size = *oi - fat->offset;
		if (fat->size == 0) {
			fat->attr = Tileset::EmptySlot;
		}
		this->items.push_back(ep);
	}
}

ActrinfoTileset::~ActrinfoTileset()
{
}

int ActrinfoTileset::getCaps()
{
	return Tileset::ColourDepthEGA
		| (this->pal ? Tileset::HasPalette : 0);
}

void ActrinfoTileset::resize(EntryPtr& id, stream::len newSize)
{
	FATEntry *fat = dynamic_cast<FATEntry *>(id.get());
	assert(fat);

	if (newSize != fat->size) {
		throw stream::error("tiles in this tileset are a fixed size");
	}
	return;
}

unsigned int ActrinfoTileset::getLayoutWidth()
{
	return 1;
}

PaletteTablePtr ActrinfoTileset::getPalette()
{
	return this->pal;
}

TilesetPtr ActrinfoTileset::createTilesetInstance(const EntryPtr& id,
	stream::inout_sptr content)
{
	FATEntry *fat = dynamic_cast<FATEntry *>(id.get());
	assert(fat);

	return TilesetPtr(
		new SingleActorTileset(content, this->dataTiles, this->pal)
	);
}


//
// SingleActorTileset
//

SingleActorTileset::SingleActorTileset(stream::inout_sptr dataInfo,
	stream::inout_sptr data, PaletteTablePtr pal)
	:	FATTileset(data, ACTR_SINGLE_FIRST_TILE_OFFSET),
		pal(pal)
{
	stream::pos lenFAT = dataInfo->size();
	this->data->seekg(0, stream::start);

	stream::len lenRemaining = lenFAT;
	ActorEntry *fatLast = NULL;
	unsigned int i = 0;
	while (lenRemaining) {
		ActorEntry *fat = new ActorEntry();
		EntryPtr ep(fat);
		fat->valid = true;
		fat->attr = Tileset::Default;
		fat->index = i++;
		fat->lenHeader = 0;
		dataInfo
			>> u16le(fat->height)
			>> u16le(fat->width)
			>> u32le(fat->offset)
		;
		// Adjust for memory alignment
		fat->offset -= fat->offset / 65536;
		if (fatLast) {
			fatLast->size = fat->offset - fatLast->offset;
			if (fatLast->size == 0) {
				fatLast->attr = Tileset::EmptySlot;
			}
		}

		this->items.push_back(ep);
		lenRemaining -= 8;
		fatLast = fat;
	}
	if (fatLast) {
		stream::pos lenData = this->data->size();
		fatLast->size = lenData - fatLast->offset;
	}
}

SingleActorTileset::~SingleActorTileset()
{
}

int SingleActorTileset::getCaps()
{
	return Tileset::ColourDepthEGA
		| (this->pal ? Tileset::HasPalette : 0);
}

void SingleActorTileset::resize(EntryPtr& id, stream::len newSize)
{
	FATEntry *fat = dynamic_cast<FATEntry *>(id.get());
	assert(fat);

	if (newSize != fat->size) {
		throw stream::error("tiles in this tileset are a fixed size");
	}
	return;
}

unsigned int SingleActorTileset::getLayoutWidth()
{
	return 40;
}

PaletteTablePtr SingleActorTileset::getPalette()
{
	return this->pal;
}

ImagePtr SingleActorTileset::createImageInstance(const EntryPtr& id,
	stream::inout_sptr content)
{
	ActorEntry *fat = dynamic_cast<ActorEntry *>(id.get());
	assert(fat);

	PLANE_LAYOUT planes;
	planes[PLANE_BLUE] = 2;
	planes[PLANE_GREEN] = 3;
	planes[PLANE_RED] = 4;
	planes[PLANE_INTENSITY] = 5;
	planes[PLANE_HITMAP] = 0;
	planes[PLANE_OPACITY] = 1;

	EGABytePlanarTiledImage *ega = new EGABytePlanarTiledImage();
	ImagePtr conv(ega);
	ega->setParams(
		content, 0, fat->width * ACTR_TILE_WIDTH, fat->height * ACTR_TILE_HEIGHT,
		planes, this->pal
	);

	return conv;
}

} // namespace gamegraphics
} // namespace camoto
