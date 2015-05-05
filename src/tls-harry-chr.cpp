/**
 * @file  tls-harry-chr.cpp
 * @brief Halloween Harry .CHR tileset handler.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/CHR_Format_(Halloween_Harry)
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
#include <camoto/util.hpp> // make_unique
#include "pal-gmf-harry.hpp"
#include "img-vga-raw.hpp"
#include "tls-harry-chr.hpp"

/// Offset of first tile in an empty tileset
#define CHR_FIRST_TILE_OFFSET  0

/// Number of tiles in the file
#define CHR_NUM_TILES   255

/// Width of each image in the tileset
#define CHR_WIDTH  16

/// Height of each image in the tileset
#define CHR_HEIGHT 16

#define FILETYPE_HARRY_CHR   "tile/harry-chr"

namespace camoto {
namespace gamegraphics {

TilesetType_HarryCHR::TilesetType_HarryCHR()
{
}

TilesetType_HarryCHR::~TilesetType_HarryCHR()
{
}

std::string TilesetType_HarryCHR::code() const
{
	return "tls-harry-chr";
}

std::string TilesetType_HarryCHR::friendlyName() const
{
	return "Halloween Harry CHR tileset";
}

std::vector<std::string> TilesetType_HarryCHR::fileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("chr");
	return vcExtensions;
}

std::vector<std::string> TilesetType_HarryCHR::games() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Alien Carnage");
	vcGames.push_back("Halloween Harry");
	return vcGames;
}

TilesetType_HarryCHR::Certainty TilesetType_HarryCHR::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();

	// Exact match with all tiles present
	// TESTED BY: tls_harry_chr_isinstance_c01
	if (len == CHR_WIDTH * CHR_HEIGHT * CHR_NUM_TILES) return DefinitelyYes;

	// Wrong size
	// TESTED BY: tls_harry_chr_isinstance_c02
	if (len % (CHR_WIDTH * CHR_HEIGHT) != 0)
		return DefinitelyNo;

	// Probable match with only some tiles present
	// TESTED BY: tls_harry_chr_isinstance_c00
	return PossiblyYes;
}

std::shared_ptr<Tileset> TilesetType_HarryCHR::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->truncate(0);
	return this->open(std::move(content), suppData);
}

std::shared_ptr<Tileset> TilesetType_HarryCHR::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	std::shared_ptr<const Palette> pal;
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		auto palFile = std::make_unique<Palette_HarryGMF>(
			std::move(suppData[SuppItem::Palette])
		);
		pal = palFile->palette();
	}
	return std::make_shared<Tileset_HarryCHR>(std::move(content), pal);
}

SuppFilenames TilesetType_HarryCHR::getRequiredSupps(
	const std::string& filenameTileset) const
{
	SuppFilenames supps;
	// Convert "missionX.chr" into "mXz1.gmf"
	std::string palName = "m";
	palName += filenameTileset[filenameTileset.length() - 5];
	palName += "z1.gmf";
	supps[SuppItem::Palette] = palName;
	return supps;
}


Tileset_HarryCHR::Tileset_HarryCHR(std::unique_ptr<stream::inout> content,
	std::shared_ptr<const Palette> pal)
	:	Tileset_FAT(std::move(content), CHR_FIRST_TILE_OFFSET, ARCH_NO_FILENAMES),
		Tileset_FAT_FixedTileSize(CHR_WIDTH * CHR_HEIGHT)
{
	this->pal = pal;

	auto len = this->content->size();
	for (unsigned int i = 0; i < len / (CHR_WIDTH * CHR_HEIGHT); i++) {
		auto fat = std::make_unique<FATEntry>();
		fat->bValid = true;
		fat->fAttr = File::Attribute::Default;
		fat->iIndex = i;
		fat->iOffset = i * CHR_WIDTH * CHR_HEIGHT;
		fat->lenHeader = 0;
		fat->realSize = fat->storedSize = CHR_WIDTH * CHR_HEIGHT;
		this->vcFAT.push_back(std::move(fat));
	}
}

Tileset_HarryCHR::~Tileset_HarryCHR()
{
}

Tileset::Caps Tileset_HarryCHR::caps() const
{
	return Caps::HasPalette;
}

ColourDepth Tileset_HarryCHR::colourDepth() const
{
	return ColourDepth::VGA;
}

Point Tileset_HarryCHR::dimensions() const
{
	return {CHR_WIDTH, CHR_HEIGHT};
}

unsigned int Tileset_HarryCHR::layoutWidth() const
{
	return 18;
}

std::unique_ptr<Image> Tileset_HarryCHR::openImage(FileHandle& id)
{
	return std::make_unique<Image_VGARaw>(
		this->open(id, true), this->dimensions(), this->palette()
	);
}

Tileset::FileHandle Tileset_HarryCHR::insert(const FileHandle& idBeforeThis,
	File::Attribute attr)
{
	return this->insert(idBeforeThis, "", this->lenTile, FILETYPE_HARRY_CHR,
		attr);
}

} // namespace gamegraphics
} // namespace camoto
