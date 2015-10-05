/**
 * @file  tls-bash.cpp
 * @brief Monster Bash tileset handler
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Monster_Bash_Tileset_Format
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
#include "img-ega-planar.hpp"
#include "tls-bash.hpp"

namespace camoto {
namespace gamegraphics {

/// Maximum number of tilesets to load from a tileset collection.
#define MB_SAFETY_MAX_TILESETCOUNT  1024

/// Offset of first tileset in a tileset collection.
//#define MB_FIRST_TILESET_OFFSET 0

/// Offset of the first tile within a single tileset.
#define MB_FIRST_TILE_OFFSET    0

/// Width of each image in pixels
#define MB_TILE_WIDTH 16

/// Height of each image in pixels
#define MB_TILE_HEIGHT 16

/// Number of tiles per BG file
#define MB_NUM_TILES_BG 511

/// Number of tiles per FG file
#define MB_NUM_TILES_FG 128

/// Length of each tile in the background file, in bytes
#define MB_TILE_LEN_BG  (32 * (unsigned int)PlaneCount::Solid)

/// Length of each tile in the foreground file, in bytes
#define MB_TILE_LEN_FG  (32 * (unsigned int)PlaneCount::Masked)

#define FILETYPE_BASH_TILE_SOLID   "tile/bash-solid"
#define FILETYPE_BASH_TILE_MASKED  "tile/bash-masked"

class Tileset_MonsterBash:
	virtual public Tileset_FAT,
	virtual public Tileset_FAT_FixedTileSize
{
	public:
		Tileset_MonsterBash(std::unique_ptr<stream::inout> content,
			PlaneCount numPlanes);
		virtual ~Tileset_MonsterBash();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual Point dimensions() const;
		virtual unsigned int layoutWidth() const;

		// Tileset_FAT
		virtual std::unique_ptr<Image> openImage(const FileHandle& id);
		virtual const FileHandle insert(const FileHandle& idBeforeThis,
			File::Attribute attr);
		using Archive::insert;

	protected:
		PlaneCount numPlanes; ///< Number of colour planes in each image
};

//
// TilesetType_MonsterBashBackground
//

TilesetType_MonsterBashBackground::TilesetType_MonsterBashBackground()
{
}

TilesetType_MonsterBashBackground::~TilesetType_MonsterBashBackground()
{
}

std::string TilesetType_MonsterBashBackground::code() const
{
	return "tls-bash-bg";
}

std::string TilesetType_MonsterBashBackground::friendlyName() const
{
	return "Monster Bash Background Tileset";
}

std::vector<std::string> TilesetType_MonsterBashBackground::fileExtensions() const
{
	return {"tbg"};
}

std::vector<std::string> TilesetType_MonsterBashBackground::games() const
{
	return {
		"Monster Bash",
	};
}

TilesetType::Certainty TilesetType_MonsterBashBackground::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();
	if (len == 0) return PossiblyYes;
	if (len == MB_NUM_TILES_BG * MB_TILE_LEN_BG) return PossiblyYes;
	// Some tiles are one byte larger for some reason
	if (len == MB_NUM_TILES_BG * MB_TILE_LEN_BG + 1) return PossiblyYes;
	return DefinitelyNo;
}

std::shared_ptr<Tileset> TilesetType_MonsterBashBackground::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->truncate(0);
	return this->open(std::move(content), suppData);
}

std::shared_ptr<Tileset> TilesetType_MonsterBashBackground::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	return std::make_shared<Tileset_MonsterBash>(
		std::move(content), PlaneCount::Solid
	);
}

SuppFilenames TilesetType_MonsterBashBackground::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	return {};
}


//
// TilesetType_MonsterBashForeground
//

TilesetType_MonsterBashForeground::TilesetType_MonsterBashForeground()
{
}

TilesetType_MonsterBashForeground::~TilesetType_MonsterBashForeground()
{
}

std::string TilesetType_MonsterBashForeground::code() const
{
	return "tls-bash-fg";
}

std::string TilesetType_MonsterBashForeground::friendlyName() const
{
	return "Monster Bash Foreground Tileset";
}

std::vector<std::string> TilesetType_MonsterBashForeground::fileExtensions() const
{
	return {"tfg", "tbn"};
}

std::vector<std::string> TilesetType_MonsterBashForeground::games() const
{
	return {
		"Monster Bash",
	};
}

TilesetType_MonsterBashForeground::Certainty TilesetType_MonsterBashForeground::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();
	if (len == 0) return PossiblyYes;
	if (len == MB_NUM_TILES_FG * MB_TILE_LEN_FG) return PossiblyYes;
	// Some tiles are one byte larger for some reason
	if (len == MB_NUM_TILES_FG * MB_TILE_LEN_FG + 1) return PossiblyYes;
	return DefinitelyNo;
}

std::shared_ptr<Tileset> TilesetType_MonsterBashForeground::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->truncate(0);
	return this->open(std::move(content), suppData);
}

std::shared_ptr<Tileset> TilesetType_MonsterBashForeground::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	return std::make_shared<Tileset_MonsterBash>(
		std::move(content), PlaneCount::Masked
	);
}

SuppFilenames TilesetType_MonsterBashForeground::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	return {};
}


//
// Tileset_MonsterBash
//

Tileset_MonsterBash::Tileset_MonsterBash(std::unique_ptr<stream::inout> content,
	PlaneCount numPlanes)
	:	Tileset_FAT(std::move(content), MB_FIRST_TILE_OFFSET, ARCH_NO_FILENAMES),
		Tileset_FAT_FixedTileSize((MB_TILE_WIDTH * MB_TILE_HEIGHT / 8) * (unsigned int)numPlanes),
		numPlanes(numPlanes)
{
	stream::pos len = this->content->size();

	int numImages = len / this->lenTile;
	this->vcFAT.reserve(numImages);
	for (int i = 0; i < numImages; i++) {
		auto fat = std::make_unique<FATEntry>();
		fat->bValid = true;
		fat->fAttr = File::Attribute::Default;
		fat->iIndex = i;
		fat->iOffset = i * this->lenTile;
		fat->storedSize = this->lenTile;
		fat->lenHeader = 0;
		switch (this->numPlanes) {
			case PlaneCount::Solid: fat->type = FILETYPE_BASH_TILE_SOLID;
			case PlaneCount::Masked: fat->type = FILETYPE_BASH_TILE_MASKED;
		}
		this->vcFAT.push_back(std::move(fat));
	}
}

Tileset_MonsterBash::~Tileset_MonsterBash()
{
}

Tileset::Caps Tileset_MonsterBash::caps() const
{
	return Caps::HasDimensions;
}

ColourDepth Tileset_MonsterBash::colourDepth() const
{
	return ColourDepth::EGA;
}

Point Tileset_MonsterBash::dimensions() const
{
	return {MB_TILE_WIDTH, MB_TILE_HEIGHT};
}

unsigned int Tileset_MonsterBash::layoutWidth() const
{
	return 20;
}

std::unique_ptr<Image> Tileset_MonsterBash::openImage(const FileHandle& id)
{
	EGAPlaneLayout planes = {
		(this->numPlanes == PlaneCount::Masked)
			? EGAPlanePurpose::Opaque1 : EGAPlanePurpose::Unused,
		EGAPlanePurpose::Intensity1,
		EGAPlanePurpose::Red1,
		EGAPlanePurpose::Green1,
		EGAPlanePurpose::Blue1,
	};

	return std::make_unique<Image_EGA_Planar>(
		this->open(id, true), 0, this->dimensions(), planes, this->palette()
	);
}

const Tileset::FileHandle Tileset_MonsterBash::insert(const FileHandle& idBeforeThis,
	File::Attribute attr)
{
	return this->insert(idBeforeThis, "", this->lenTile,
		this->numPlanes == PlaneCount::Solid ? FILETYPE_BASH_TILE_SOLID : FILETYPE_BASH_TILE_MASKED,
		attr);
}

} // namespace gamegraphics
} // namespace camoto
