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

#include <iostream>
#include <camoto/iostream_helpers.hpp>
#include "tls-harry-ico.hpp"
#include "img-pcx.hpp"
#include "img-ddave.hpp"

namespace camoto {
namespace gamegraphics {

/// Offset of first tile in an empty tileset
#define ICO_FIRST_TILE_OFFSET  0

/// Size of each image's header (2x UINT16LE)
#define ICO_HEADER_LEN  4

//
// TilesetType_HarryICO
//

TilesetType_HarryICO::TilesetType_HarryICO()
{
}

TilesetType_HarryICO::~TilesetType_HarryICO()
{
}

std::string TilesetType_HarryICO::getCode() const
{
	return "tls-harry-ico";
}

std::string TilesetType_HarryICO::getFriendlyName() const
{
	return "Halloween Harry ICO Tileset";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> TilesetType_HarryICO::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("ico");
	return vcExtensions;
}

std::vector<std::string> TilesetType_HarryICO::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Alien Carnage");
	vcGames.push_back("Halloween Harry");
	return vcGames;
}

TilesetType_HarryICO::Certainty TilesetType_HarryICO::isInstance(stream::input_sptr psGraphics) const
{
	stream::pos len = psGraphics->size();

	// Since there's no header, an empty file could mean an empty tileset.
	// TESTED BY: tls_harry_ico_new_isinstance
	if (len == 0) return PossiblyYes;

	// Make sure the file is large enough
	// TESTED BY: fmt_harry_ico_isinstance_c01
	if (len < ICO_HEADER_LEN) return DefinitelyNo;

	psGraphics->seekg(0, stream::start);
	stream::pos pos = 0;
	while (pos < len) {
		uint16_t width, height;
		try {
			psGraphics
				>> u16le(width)
				>> u16le(height)
			;
		} catch (const stream::incomplete_read&) {
			// If EOF is encountered here it's not a valid file
			// TESTED BY: fmt_harry_ico_isinstance_c02
			return DefinitelyNo;
		}
		int delta = width * height;
		// If this pushes us past EOF it's not a valid file
		// TESTED BY: fmt_harry_ico_isinstance_c03
		pos += delta + ICO_HEADER_LEN;
		if (pos > len) return DefinitelyNo;

		psGraphics->seekg(delta, stream::cur);
	}

	// TESTED BY: fmt_harry_ico_isinstance_c00
	return DefinitelyYes;
}

TilesetPtr TilesetType_HarryICO::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	psGraphics->truncate(0);
	psGraphics->seekp(0, stream::start);

	PaletteTablePtr pal;
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new Image_PCX(suppData[SuppItem::Palette], 8, 1));
		pal = palFile->getPalette();
	}

	return TilesetPtr(new Tileset_HarryICO(psGraphics, pal));
}

TilesetPtr TilesetType_HarryICO::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	PaletteTablePtr pal;
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new Image_PCX(suppData[SuppItem::Palette], 8, 1));
		pal = palFile->getPalette();
	}

	return TilesetPtr(new Tileset_HarryICO(psGraphics, pal));
}

SuppFilenames TilesetType_HarryICO::getRequiredSupps(const std::string& filenameGraphics) const
{
	SuppFilenames supps;
	supps[SuppItem::Palette] = "pre2.pcx"; // any UI image file
	return supps;
}


//
// Tileset_HarryICO
//

Tileset_HarryICO::Tileset_HarryICO(stream::inout_sptr data, PaletteTablePtr pal)
	:	Tileset_FAT(data, ICO_FIRST_TILE_OFFSET),
		pal(pal)
{
	stream::pos len = this->data->size();

	this->data->seekg(0, stream::start);

	stream::pos pos = 0;
	unsigned int i = 0;
	while (pos < len) {

		uint16_t width, height;
		this->data
			>> u16le(width)
			>> u16le(height)
		;
		unsigned int delta = width * height;

		FATEntry *fat = new FATEntry();
		EntryPtr ep(fat);
		fat->valid = true;
		fat->attr = 0;
		fat->index = i;
		fat->offset = pos;
		fat->size = delta + ICO_HEADER_LEN;
		fat->lenHeader = 0;
		this->items.push_back(ep);

		// If this pushes us past EOF it's not a valid file
		// TESTED BY: fmt_harry_ico_isinstance_c02
		pos += fat->size;

		this->data->seekg(delta, stream::cur);
		i++;
	}
}

Tileset_HarryICO::~Tileset_HarryICO()
{
}

int Tileset_HarryICO::getCaps()
{
	return Tileset::HasPalette | Tileset::ColourDepthVGA;
}

unsigned int Tileset_HarryICO::getLayoutWidth()
{
	return 16;
}

PaletteTablePtr Tileset_HarryICO::getPalette()
{
	return this->pal;
}

ImagePtr Tileset_HarryICO::createImageInstance(const EntryPtr& id,
	stream::inout_sptr content)
{
	return ImagePtr(new Image_DDaveVGA(content, false, this->pal));
}

} // namespace gamegraphics
} // namespace camoto
