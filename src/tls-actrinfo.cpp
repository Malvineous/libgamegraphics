/**
 * @file  tls-actrinfo.cpp
 * @brief Cosmo/Duke II actor tileset
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
class Tileset_Actrinfo: virtual public Tileset_FAT
{
	public:
		Tileset_Actrinfo(stream::inout_sptr dataInfo, stream::inout_sptr dataTiles,
			PaletteTablePtr pal);
		virtual ~Tileset_Actrinfo();

		virtual int getCaps();
		void resize(EntryPtr& id, stream::len newSize);
		virtual unsigned int getLayoutWidth();
		virtual PaletteTablePtr getPalette();

		// Tileset_FAT
		virtual TilesetPtr createTilesetInstance(const EntryPtr& id,
			stream::inout_sptr content);

	protected:
		stream::inout_sptr dataTiles;  ///< ACTORS.MNI or equivalent
		PaletteTablePtr pal;
};

/// Tileset containing each frame for a single actor.
class Tileset_SingleActor: virtual public Tileset_FAT
{
	public:
		Tileset_SingleActor(stream::inout_sptr fat, stream::inout_sptr data,
			PaletteTablePtr pal);
		virtual ~Tileset_SingleActor();

		virtual int getCaps();
		void resize(EntryPtr& id, stream::len newSize);
		virtual unsigned int getLayoutWidth();
		virtual PaletteTablePtr getPalette();

		// Tileset_FAT
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
// TilesetType_Actrinfo
//

TilesetType_Actrinfo::TilesetType_Actrinfo()
{
}

TilesetType_Actrinfo::~TilesetType_Actrinfo()
{
}

std::string TilesetType_Actrinfo::getCode() const
{
	return "tls-actrinfo";
}

std::string TilesetType_Actrinfo::getFriendlyName() const
{
	return "Cosmo Actor Tileset";
}

std::vector<std::string> TilesetType_Actrinfo::getFileExtensions() const
{
	return {"mni"};
}

std::vector<std::string> TilesetType_Actrinfo::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Cosmo's Cosmic Adventures");
	return vcGames;
}

TilesetType_Actrinfo::Certainty TilesetType_Actrinfo::isInstance(stream::input_sptr psGraphics) const
{
	return Unsure;
}

TilesetPtr TilesetType_Actrinfo::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	psGraphics->seekp(0, stream::start);
	// Zero tiles, 0x0
	if (suppData.find(SuppItem::FAT) == suppData.end()) {
		throw stream::error("no actor info file specified (missing supplementary item)");
	}
	return TilesetPtr(
		new Tileset_Actrinfo(suppData[SuppItem::FAT], psGraphics, PaletteTablePtr())
	);
}

TilesetPtr TilesetType_Actrinfo::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	if (suppData.find(SuppItem::FAT) == suppData.end()) {
		throw stream::error("no actor info file specified (missing supplementary item)");
	}
	return TilesetPtr(
		new Tileset_Actrinfo(suppData[SuppItem::FAT], psGraphics, PaletteTablePtr())
	);
}

SuppFilenames TilesetType_Actrinfo::getRequiredSupps(const std::string& filenameGraphics) const
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
// Tileset_Actrinfo
//

Tileset_Actrinfo::Tileset_Actrinfo(stream::inout_sptr dataInfo,
	stream::inout_sptr dataTiles, PaletteTablePtr pal)
	:	Tileset_FAT(dataInfo, ACTR_FIRST_TILE_OFFSET),
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

Tileset_Actrinfo::~Tileset_Actrinfo()
{
}

int Tileset_Actrinfo::getCaps()
{
	return Tileset::ColourDepthEGA
		| (this->pal ? Tileset::HasPalette : 0);
}

void Tileset_Actrinfo::resize(EntryPtr& id, stream::len newSize)
{
	FATEntry *fat = dynamic_cast<FATEntry *>(id.get());
	assert(fat);

	if (newSize != fat->size) {
		throw stream::error("tiles in this tileset are a fixed size");
	}
	return;
}

unsigned int Tileset_Actrinfo::getLayoutWidth()
{
	return 1;
}

PaletteTablePtr Tileset_Actrinfo::getPalette()
{
	return this->pal;
}

TilesetPtr Tileset_Actrinfo::createTilesetInstance(const EntryPtr& id,
	stream::inout_sptr content)
{
	FATEntry *fat = dynamic_cast<FATEntry *>(id.get());
	assert(fat);

	return TilesetPtr(
		new Tileset_SingleActor(content, this->dataTiles, this->pal)
	);
}


//
// Tileset_SingleActor
//

Tileset_SingleActor::Tileset_SingleActor(stream::inout_sptr dataInfo,
	stream::inout_sptr data, PaletteTablePtr pal)
	:	Tileset_FAT(data, ACTR_SINGLE_FIRST_TILE_OFFSET),
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

Tileset_SingleActor::~Tileset_SingleActor()
{
}

int Tileset_SingleActor::getCaps()
{
	return Tileset::ColourDepthEGA
		| (this->pal ? Tileset::HasPalette : 0);
}

void Tileset_SingleActor::resize(EntryPtr& id, stream::len newSize)
{
	FATEntry *fat = dynamic_cast<FATEntry *>(id.get());
	assert(fat);

	if (newSize != fat->size) {
		throw stream::error("tiles in this tileset are a fixed size");
	}
	return;
}

unsigned int Tileset_SingleActor::getLayoutWidth()
{
	return 40;
}

PaletteTablePtr Tileset_SingleActor::getPalette()
{
	return this->pal;
}

ImagePtr Tileset_SingleActor::createImageInstance(const EntryPtr& id,
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

	Image_EGABytePlanarTiled *ega = new Image_EGABytePlanarTiled();
	ImagePtr conv(ega);
	ega->setParams(
		content, 0, fat->width * ACTR_TILE_WIDTH, fat->height * ACTR_TILE_HEIGHT,
		planes, this->pal
	);

	return conv;
}

} // namespace gamegraphics
} // namespace camoto
