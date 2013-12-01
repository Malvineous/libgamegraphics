/**
 * @file   tls-czone.cpp
 * @brief  Duke Nukem II CZone tileset
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Duke_Nukem_II_CZone_Format
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

#include <boost/bind.hpp>
#include <camoto/iostream_helpers.hpp>
#include "tls-czone.hpp"
#include "tls-ega-apogee.hpp"
#include "tileset-fat.hpp" // for FATEntry
#include "pal-vga-raw.hpp"

/// Width of each tile, in pixels
#define CZ_TILE_WIDTH 8

/// Height of each tile, in pixels
#define CZ_TILE_HEIGHT 8

/// Ideal width of tileset, in number of tiles
#define CZ_IDEAL_WIDTH 40

namespace camoto {
namespace gamegraphics {

class CZoneTileset: virtual public Tileset
{
	public:
		CZoneTileset(stream::inout_sptr data, PaletteTablePtr pal);
		virtual ~CZoneTileset();

		virtual int getCaps();
		virtual const VC_ENTRYPTR& getItems() const;
		virtual TilesetPtr openTileset(const EntryPtr& id);
		virtual ImagePtr openImage(const EntryPtr& id);
		virtual EntryPtr insert(const EntryPtr& idBeforeThis, int attr);
		virtual void remove(EntryPtr& id);
		virtual void resize(EntryPtr& id, stream::len newSize);
		virtual void flush();
		virtual void getTilesetDimensions(unsigned int *width, unsigned int *height);
		virtual void setTilesetDimensions(unsigned int width, unsigned int height);
		virtual unsigned int getLayoutWidth();
		virtual PaletteTablePtr getPalette();
		virtual void setPalette(PaletteTablePtr newPalette);

	protected:
		stream::inout_sptr data;
		PaletteTablePtr pal;
		VC_ENTRYPTR items;
};

//
// CZoneTilesetType
//

CZoneTilesetType::CZoneTilesetType()
{
}

CZoneTilesetType::~CZoneTilesetType()
{
}

std::string CZoneTilesetType::getCode() const
{
	return "tls-nukem2-czone";
}

std::string CZoneTilesetType::getFriendlyName() const
{
	return "Duke Nukem II CZone Tileset";
}

std::vector<std::string> CZoneTilesetType::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("mni");
	return vcExtensions;
}

std::vector<std::string> CZoneTilesetType::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Duke Nukem II");
	return vcGames;
}

CZoneTilesetType::Certainty CZoneTilesetType::isInstance(stream::input_sptr psGraphics) const
{
	stream::pos len = psGraphics->size();

	// Standard tileset
	if (len == 42000) return PossiblyYes;

	return DefinitelyNo;
}

TilesetPtr CZoneTilesetType::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	psGraphics->seekp(0, stream::start);
	ImagePtr palFile(new VGAPalette(suppData[SuppItem::Palette], 6));
	PaletteTablePtr pal = palFile->getPalette();
	// Zero tiles, 0x0
	return TilesetPtr(new CZoneTileset(psGraphics, pal));
}

TilesetPtr CZoneTilesetType::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	ImagePtr palFile(new VGAPalette(suppData[SuppItem::Palette], 6));
	PaletteTablePtr pal = palFile->getPalette();
	return TilesetPtr(new CZoneTileset(psGraphics, pal));
}

SuppFilenames CZoneTilesetType::getRequiredSupps(const std::string& filenameGraphics) const
{
	SuppFilenames supps;
	supps[SuppItem::Palette] = "gamepal.pal";
	return supps;
}


//
// CZoneTileset
//

CZoneTileset::CZoneTileset(stream::inout_sptr data, PaletteTablePtr pal)
	:	data(data),
		pal(pal)
{
	{
		FATTileset::FATEntry *fat = new FATTileset::FATEntry();
		EntryPtr ep(fat);
		fat->valid = true;
		fat->attr = Tileset::SubTileset;
		fat->index = 0;
		fat->offset = 3600;
		fat->size = 32000;
		fat->lenHeader = 0;
		this->items.push_back(ep);
	}
	{
		FATTileset::FATEntry *fat = new FATTileset::FATEntry();
		EntryPtr ep(fat);
		fat->valid = true;
		fat->attr = Tileset::SubTileset;
		fat->index = 1;
		fat->offset = 3600+32000;
		fat->size = 6400;
		fat->lenHeader = 0;
		this->items.push_back(ep);
	}
}

CZoneTileset::~CZoneTileset()
{
}

int CZoneTileset::getCaps()
{
	return Tileset::HasPalette | Tileset::ColourDepthEGA;
}

const CZoneTileset::VC_ENTRYPTR& CZoneTileset::getItems() const
{
	return this->items;
}

TilesetPtr CZoneTileset::openTileset(const EntryPtr& id)
{
	FATTileset::FATEntryPtr pFAT = boost::dynamic_pointer_cast<FATTileset::FATEntry>(id);
	assert(pFAT);
	stream::fn_truncate fnTruncate = boost::bind<void>(&CZoneTileset::resize, this, id, _1);
	stream::sub_sptr sub(new stream::sub);
	sub->open(this->data, pFAT->offset + pFAT->lenHeader, pFAT->size, fnTruncate);
	TilesetPtr tileset;
	switch (pFAT->index) {
		case 0: // solid
			tileset.reset(new EGAApogeeTileset(sub, CZ_TILE_WIDTH, CZ_TILE_HEIGHT,
				EGA_NUMPLANES_SOLID, CZ_IDEAL_WIDTH, this->pal));
			break;
		case 1: // masked
			tileset.reset(new EGAApogeeTileset(sub, CZ_TILE_WIDTH, CZ_TILE_HEIGHT,
				EGA_NUMPLANES_MASKED, CZ_IDEAL_WIDTH, this->pal));
			break;
	}
	return tileset;
}

ImagePtr CZoneTileset::openImage(const EntryPtr& id)
{
	assert(false);
	return ImagePtr();
}

CZoneTileset::EntryPtr CZoneTileset::insert(const EntryPtr& idBeforeThis, int attr)
{
	throw stream::error("tilesets are fixed in this format");
}

void CZoneTileset::remove(EntryPtr& id)
{
	throw stream::error("tilesets are fixed in this format");
}

void CZoneTileset::resize(EntryPtr& id, stream::len newSize)
{
	FATTileset::FATEntryPtr pFAT = boost::dynamic_pointer_cast<FATTileset::FATEntry>(id);
	assert(pFAT);
	stream::len correctSize;
	if (pFAT->index == 0) correctSize = 32000;
	else if (pFAT->index == 1) correctSize = 6400;
	else correctSize = 0;
	if (newSize != correctSize) {
		throw stream::error("tiles in this tileset are a fixed size");
	}
	return;
}

void CZoneTileset::flush()
{
	return;
}

void CZoneTileset::getTilesetDimensions(unsigned int *width, unsigned int *height)
{
	*width = 0;
	*height = 0;
	return;
}

void CZoneTileset::setTilesetDimensions(unsigned int width, unsigned int height)
{
	assert(false);
	return;
}

unsigned int CZoneTileset::getLayoutWidth()
{
	return 40;
}

PaletteTablePtr CZoneTileset::getPalette()
{
	return this->pal;
}

void CZoneTileset::setPalette(PaletteTablePtr newPalette)
{
	assert(false);
	return;
}

} // namespace gamegraphics
} // namespace camoto
