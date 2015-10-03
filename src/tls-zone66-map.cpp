/**
 * @file  tls-zone66-map.cpp
 * @brief Zone 66 map tileset
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Zone_66_Level_Format
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
#include "tileset-fat.hpp"
#include "tileset-fat-fixed_tile_size.hpp"
#include "img-vga-raw.hpp"
#include "pal-vga-raw.hpp"
#include "tls-zone66-map.hpp"

namespace camoto {
namespace gamegraphics {

/// Offset of first tileset in a tileset collection.
#define Z66_FIRST_TILE_OFFSET 0

/// Width of each tile, in pixels
#define Z66_TILE_WIDTH  32

/// Height of each tile, in pixels
#define Z66_TILE_HEIGHT 32

/// Size of each tile, in bytes
#define Z66_TILE_SIZE (Z66_TILE_WIDTH * Z66_TILE_HEIGHT)

/// Colour depth of VGA palette
#define Z66_PAL_DEPTH 6

#define FILETYPE_ZONE66_M  "tile/zone66-map"

/// Handler for Zone 66 map tilesets.
class Tileset_Zone66Map:
	virtual public Tileset_FAT,
	virtual public Tileset_FAT_FixedTileSize
{
	public:
		Tileset_Zone66Map(std::unique_ptr<stream::inout> content,
			std::shared_ptr<const Palette> pal);
		virtual ~Tileset_Zone66Map();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual Point dimensions() const;
		virtual unsigned int layoutWidth() const;

		// Tileset_FAT
		virtual std::unique_ptr<Image> openImage(const FileHandle& id);
		virtual const FileHandle insert(const FileHandle& idBeforeThis,
			File::Attribute attr);
		using Archive::insert;
};

//
// TilesetType_Zone66Map
//

TilesetType_Zone66Map::TilesetType_Zone66Map()
{
}

TilesetType_Zone66Map::~TilesetType_Zone66Map()
{
}

std::string TilesetType_Zone66Map::code() const
{
	return "tls-zone66-map";
}

std::string TilesetType_Zone66Map::friendlyName() const
{
	return "Zone 66 Map Tileset";
}

std::vector<std::string> TilesetType_Zone66Map::fileExtensions() const
{
	return {"z66"};
}

std::vector<std::string> TilesetType_Zone66Map::games() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Zone 66");
	return vcGames;
}

TilesetType::Certainty TilesetType_Zone66Map::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();

	// Standard tileset
	// TESTED BY: tls_zone66_map_c01
	if (len % Z66_TILE_SIZE != 0) return DefinitelyNo;

	// TESTED BY: tls_zone66_map_c00
	return PossiblyYes;
}

std::shared_ptr<Tileset> TilesetType_Zone66Map::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->truncate(0);
	return this->open(std::move(content), suppData);
}

std::shared_ptr<Tileset> TilesetType_Zone66Map::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	std::shared_ptr<const Palette> pal;
	// Only load the palette if one was given
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		auto palFile = std::make_shared<Palette_VGA>(
			std::move(suppData[SuppItem::Palette]), Z66_PAL_DEPTH
		);
		pal = palFile->palette();
	}
	return std::make_shared<Tileset_Zone66Map>(std::move(content), pal);
}

SuppFilenames TilesetType_Zone66Map::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	SuppFilenames supps;
	supps[SuppItem::Palette] = "mpal.z66";
	return supps;
}


//
// Tileset_Zone66Map
//

Tileset_Zone66Map::Tileset_Zone66Map(std::unique_ptr<stream::inout> content,
	std::shared_ptr<const Palette> pal)
	:	Tileset_FAT(std::move(content), Z66_FIRST_TILE_OFFSET, ARCH_NO_FILENAMES),
		Tileset_FAT_FixedTileSize(Z66_TILE_SIZE)
{
	this->pal = pal;
	stream::pos len = this->content->size();

	this->content->seekg(0, stream::start);
	int numImages = len / Z66_TILE_SIZE;

	this->vcFAT.reserve(numImages);
	for (int i = 0; i < numImages; i++) {
		auto fat = std::make_unique<FATEntry>();
		fat->bValid = true;
		fat->fAttr = File::Attribute::Default;
		fat->iIndex = i;
		fat->iOffset = i * Z66_TILE_SIZE;
		fat->storedSize = Z66_TILE_SIZE;
		fat->lenHeader = 0;
		fat->type = FILETYPE_ZONE66_M;
		this->vcFAT.push_back(std::move(fat));
	}
}

Tileset_Zone66Map::~Tileset_Zone66Map()
{
}

Tileset::Caps Tileset_Zone66Map::caps() const
{
	return Caps::HasDimensions |
		(this->pal ? Caps::HasPalette : Caps::Default);
}

ColourDepth Tileset_Zone66Map::colourDepth() const
{
	return ColourDepth::VGA;
}

Point Tileset_Zone66Map::dimensions() const
{
	return {Z66_TILE_WIDTH, Z66_TILE_HEIGHT};
}

unsigned int Tileset_Zone66Map::layoutWidth() const
{
	return 18;
}

std::unique_ptr<Image> Tileset_Zone66Map::openImage(const FileHandle& id)
{
	return std::make_unique<Image_VGARaw>(
		this->open(id, true), this->dimensions(), this->palette()
	);
}

const Tileset::FileHandle Tileset_Zone66Map::insert(const FileHandle& idBeforeThis,
	File::Attribute attr)
{
	return this->insert(idBeforeThis, "", this->lenTile, FILETYPE_ZONE66_M, attr);
}

} // namespace gamegraphics
} // namespace camoto
