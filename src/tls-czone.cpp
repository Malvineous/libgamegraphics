/**
 * @file  tls-czone.cpp
 * @brief Duke Nukem II CZone tileset
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Duke_Nukem_II_CZone_Format
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

class Tileset_CZone: virtual public Tileset
{
	public:
		Tileset_CZone(stream::inout_sptr data, PaletteTablePtr pal);
		virtual ~Tileset_CZone();

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
// TilesetType_CZone
//

TilesetType_CZone::TilesetType_CZone()
{
}

TilesetType_CZone::~TilesetType_CZone()
{
}

std::string TilesetType_CZone::getCode() const
{
	return "tls-nukem2-czone";
}

std::string TilesetType_CZone::getFriendlyName() const
{
	return "Duke Nukem II CZone Tileset";
}

std::vector<std::string> TilesetType_CZone::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("mni");
	return vcExtensions;
}

std::vector<std::string> TilesetType_CZone::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Duke Nukem II");
	return vcGames;
}

TilesetType_CZone::Certainty TilesetType_CZone::isInstance(stream::input_sptr psGraphics) const
{
	stream::pos len = psGraphics->size();

	// Standard tileset
	if (len == 42000) return PossiblyYes;

	return DefinitelyNo;
}

TilesetPtr TilesetType_CZone::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	psGraphics->seekp(0, stream::start);
	ImagePtr palFile(new Palette_VGA(suppData[SuppItem::Palette], 6));
	PaletteTablePtr pal = palFile->getPalette();
	// Zero tiles, 0x0
	return TilesetPtr(new Tileset_CZone(psGraphics, pal));
}

TilesetPtr TilesetType_CZone::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	ImagePtr palFile(new Palette_VGA(suppData[SuppItem::Palette], 6));
	PaletteTablePtr pal = palFile->getPalette();
	return TilesetPtr(new Tileset_CZone(psGraphics, pal));
}

SuppFilenames TilesetType_CZone::getRequiredSupps(const std::string& filenameGraphics) const
{
	SuppFilenames supps;
	supps[SuppItem::Palette] = "gamepal.pal";
	return supps;
}


//
// Tileset_CZone
//

Tileset_CZone::Tileset_CZone(stream::inout_sptr data, PaletteTablePtr pal)
	:	data(data),
		pal(pal)
{
	{
		Tileset_FAT::FATEntry *fat = new Tileset_FAT::FATEntry();
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
		Tileset_FAT::FATEntry *fat = new Tileset_FAT::FATEntry();
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

Tileset_CZone::~Tileset_CZone()
{
}

int Tileset_CZone::getCaps()
{
	return Tileset::HasPalette | Tileset::ColourDepthEGA;
}

const Tileset_CZone::VC_ENTRYPTR& Tileset_CZone::getItems() const
{
	return this->items;
}

TilesetPtr Tileset_CZone::openTileset(const EntryPtr& id)
{
	Tileset_FAT::FATEntryPtr pFAT = boost::dynamic_pointer_cast<Tileset_FAT::FATEntry>(id);
	assert(pFAT);
	stream::fn_truncate fnTruncate = boost::bind(&Tileset_CZone::resize, this, id, _1);
	stream::sub_sptr sub(new stream::sub);
	sub->open(this->data, pFAT->offset + pFAT->lenHeader, pFAT->size, fnTruncate);
	TilesetPtr tileset;
	switch (pFAT->index) {
		case 0: // solid
			tileset.reset(new Tileset_EGAApogee(sub, CZ_TILE_WIDTH, CZ_TILE_HEIGHT,
				EGA_NUMPLANES_SOLID, CZ_IDEAL_WIDTH, this->pal));
			break;
		case 1: // masked
			tileset.reset(new Tileset_EGAApogee(sub, CZ_TILE_WIDTH, CZ_TILE_HEIGHT,
				EGA_NUMPLANES_MASKED, CZ_IDEAL_WIDTH, this->pal));
			break;
	}
	return tileset;
}

ImagePtr Tileset_CZone::openImage(const EntryPtr& id)
{
	assert(false);
	return ImagePtr();
}

Tileset_CZone::EntryPtr Tileset_CZone::insert(const EntryPtr& idBeforeThis, int attr)
{
	throw stream::error("tilesets are fixed in this format");
}

void Tileset_CZone::remove(EntryPtr& id)
{
	throw stream::error("tilesets are fixed in this format");
}

void Tileset_CZone::resize(EntryPtr& id, stream::len newSize)
{
	Tileset_FAT::FATEntryPtr pFAT = boost::dynamic_pointer_cast<Tileset_FAT::FATEntry>(id);
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

void Tileset_CZone::flush()
{
	return;
}

void Tileset_CZone::getTilesetDimensions(unsigned int *width, unsigned int *height)
{
	*width = 0;
	*height = 0;
	return;
}

void Tileset_CZone::setTilesetDimensions(unsigned int width, unsigned int height)
{
	assert(false);
	return;
}

unsigned int Tileset_CZone::getLayoutWidth()
{
	return 40;
}

PaletteTablePtr Tileset_CZone::getPalette()
{
	return this->pal;
}

void Tileset_CZone::setPalette(PaletteTablePtr newPalette)
{
	assert(false);
	return;
}

} // namespace gamegraphics
} // namespace camoto
