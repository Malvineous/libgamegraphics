/**
 * @file   tls-harry-ico.cpp
 * @brief  Halloween Harry .ICO tileset handler.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/ICO_Format_(Halloween_Harry)
 *
 * Copyright (C) 2010-2012 Adam Nielsen <malvineous@shikadi.net>
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
#include "pal-gmf-harry.hpp"
#include "img-ddave.hpp"

namespace camoto {
namespace gamegraphics {

/// Offset of first tile in an empty tileset
#define ICO_FIRST_TILE_OFFSET  0

/// Size of each image's header (2x UINT16LE)
#define ICO_HEADER_LEN  4

//
// HarryICOTilesetType
//

HarryICOTilesetType::HarryICOTilesetType()
{
}

HarryICOTilesetType::~HarryICOTilesetType()
{
}

std::string HarryICOTilesetType::getCode() const
{
	return "tls-harry-ico";
}

std::string HarryICOTilesetType::getFriendlyName() const
{
	return "Halloween Harry ICO Tileset";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> HarryICOTilesetType::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("ico");
	return vcExtensions;
}

std::vector<std::string> HarryICOTilesetType::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Alien Carnage");
	vcGames.push_back("Halloween Harry");
	return vcGames;
}

HarryICOTilesetType::Certainty HarryICOTilesetType::isInstance(stream::input_sptr psGraphics) const
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
		} catch (const stream::incomplete_read& e) {
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

TilesetPtr HarryICOTilesetType::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	psGraphics->truncate(0);
	psGraphics->seekp(0, stream::start);

	PaletteTablePtr pal;
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new GMFHarryPalette(suppData[SuppItem::Palette]));
		pal = palFile->getPalette();
	} else {
		std::cerr << "No palette supplied for .ICO, using default" << std::endl;
		pal = createPalette_DefaultVGA();
	}

	return TilesetPtr(new HarryICOTileset(psGraphics, pal));
}

TilesetPtr HarryICOTilesetType::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	PaletteTablePtr pal;
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new GMFHarryPalette(suppData[SuppItem::Palette]));
		pal = palFile->getPalette();
	} else {
		std::cerr << "No palette supplied for .ICO, using default" << std::endl;
		pal = createPalette_DefaultVGA();
	}

	return TilesetPtr(new HarryICOTileset(psGraphics, pal));
}

SuppFilenames HarryICOTilesetType::getRequiredSupps(const std::string& filenameGraphics) const
{
	SuppFilenames supps;
	supps[SuppItem::Palette] = "m1z1.gmf"; // any map file
	return supps;
}


//
// HarryICOTileset
//

HarryICOTileset::HarryICOTileset(stream::inout_sptr data, PaletteTablePtr pal)
	:	FATTileset(data, ICO_FIRST_TILE_OFFSET),
		pal(pal)
{
	assert(this->pal);
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
		fat->isValid = true;
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

HarryICOTileset::~HarryICOTileset()
{
}

int HarryICOTileset::getCaps()
{
	return HasPalette;
}

unsigned int HarryICOTileset::getLayoutWidth()
{
	return 16;
}

PaletteTablePtr HarryICOTileset::getPalette()
{
	return this->pal;
}

ImagePtr HarryICOTileset::createImageInstance(const EntryPtr& id,
	stream::inout_sptr content)
{
	return ImagePtr(new DDaveVGAImage(content, false, this->pal));
}

} // namespace gamegraphics
} // namespace camoto
