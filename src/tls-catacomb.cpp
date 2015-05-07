/**
 * @file  tls-catacomb.cpp
 * @brief Catacomb/Catacomb II CGA and EGA tilesets
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Catacomb_Tileset_Format
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
#include "img-ega-linear.hpp"
#include "tls-catacomb.hpp"

namespace camoto {
namespace gamegraphics {

/// Offset of first tileset in a tileset collection.
#define CAT_FIRST_TILE_OFFSET 0

#define CAT_TILE_WIDTH 8
#define CAT_TILE_HEIGHT 8

/// Number of bytes per tile in the CGA tileset
#define CAT_CGA_TILE_SIZE 16

/// Number of bytes per tile in the EGA tileset
#define CAT_EGA_TILE_SIZE 32

/// Number of tiles in the Catacomb I tileset
#define CATI_NUMTILES 1462

/// Number of tiles in the Catacomb II tileset
#define CATII_NUMTILES 1618

#define FILETYPE_CAT  "tile/catacomb"

/// Tileset format handler.
class Tileset_Catacomb:
	virtual public Tileset_FAT,
	virtual public Tileset_FAT_FixedTileSize
{
	public:
		Tileset_Catacomb(std::unique_ptr<stream::inout> content, ColourDepth depth);
		virtual ~Tileset_Catacomb();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual Point dimensions() const;
		virtual unsigned int layoutWidth() const;

		// Tileset_FAT
		virtual std::unique_ptr<Image> openImage(FileHandle& id);
		virtual FileHandle insert(const FileHandle& idBeforeThis,
			File::Attribute attr);
		using Archive::insert;

	protected:
		ColourDepth depth;
};


TilesetType_Catacomb::TilesetType_Catacomb()
{
}

TilesetType_Catacomb::~TilesetType_Catacomb()
{
}

std::vector<std::string> TilesetType_Catacomb::fileExtensions() const
{
	return {"cat", "ca2"};
}

std::vector<std::string> TilesetType_Catacomb::games() const
{
	return {
		"Catacomb",
		"Catacomb II"
	};
}

SuppFilenames TilesetType_Catacomb::getRequiredSupps(
	const std::string& filenameGraphics) const
{
	return {};
}


//
// TilesetType_CatacombEGA
//

TilesetType_CatacombEGA::TilesetType_CatacombEGA()
{
}

TilesetType_CatacombEGA::~TilesetType_CatacombEGA()
{
}

std::string TilesetType_CatacombEGA::code() const
{
	return "tls-catacomb-ega";
}

std::string TilesetType_CatacombEGA::friendlyName() const
{
	return "Catacomb EGA Tileset";
}

TilesetType_CatacombEGA::Certainty TilesetType_CatacombEGA::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();

	// Catacomb I
	// TESTED BY: tls_catacomb_ega_c01
	if (len == CATI_NUMTILES * CAT_EGA_TILE_SIZE) return DefinitelyYes;

	// Catacomb II
	if (len == CATII_NUMTILES * CAT_EGA_TILE_SIZE) return DefinitelyYes;

	// TESTED BY: tls_catacomb_ega_c00
	if (len % CAT_EGA_TILE_SIZE == 0) return PossiblyYes;

	// TESTED BY: tls_catacomb_ega_c02
	return DefinitelyNo;
}

std::shared_ptr<Tileset> TilesetType_CatacombEGA::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->truncate(0);
	// Zero tiles, 0x0
	return this->open(std::move(content), suppData);
}

std::shared_ptr<Tileset> TilesetType_CatacombEGA::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	return std::make_shared<Tileset_Catacomb>(std::move(content),
		ColourDepth::EGA);
}


//
// TilesetType_CatacombCGA
//

TilesetType_CatacombCGA::TilesetType_CatacombCGA()
{
}

TilesetType_CatacombCGA::~TilesetType_CatacombCGA()
{
}

std::string TilesetType_CatacombCGA::code() const
{
	return "tls-catacomb-cga";
}

std::string TilesetType_CatacombCGA::friendlyName() const
{
	return "Catacomb CGA Tileset";
}

TilesetType_CatacombCGA::Certainty TilesetType_CatacombCGA::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();

	// Catacomb I
	// TESTED BY: tls_catacomb_ega_c01
	if (len == CATI_NUMTILES * CAT_CGA_TILE_SIZE) return DefinitelyYes;

	// Catacomb II
	if (len == CATII_NUMTILES * CAT_CGA_TILE_SIZE) return DefinitelyYes;

	// TESTED BY: tls_catacomb_ega_c00
	if (len % CAT_EGA_TILE_SIZE == 0) return PossiblyYes;

	// TESTED BY: tls_catacomb_ega_c02
	return DefinitelyNo;
}

std::shared_ptr<Tileset> TilesetType_CatacombCGA::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->truncate(0);
	// Zero tiles, 0x0
	return this->open(std::move(content), suppData);
}

std::shared_ptr<Tileset> TilesetType_CatacombCGA::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	return std::make_shared<Tileset_Catacomb>(std::move(content),
		ColourDepth::CGA);
}


//
// Tileset_Catacomb
//

Tileset_Catacomb::Tileset_Catacomb(std::unique_ptr<stream::inout> content,
	ColourDepth depth)
	:	Tileset_FAT(std::move(content), CAT_FIRST_TILE_OFFSET, ARCH_NO_FILENAMES),
		Tileset_FAT_FixedTileSize(
			(depth == ColourDepth::EGA) ? CAT_EGA_TILE_SIZE : CAT_CGA_TILE_SIZE
		),
		depth(depth)
{
	stream::pos len = this->content->size();

	this->content->seekg(0, stream::start);
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
		fat->type = FILETYPE_CAT;
		this->vcFAT.push_back(std::move(fat));
	}

}

Tileset_Catacomb::~Tileset_Catacomb()
{
}

Tileset::Caps Tileset_Catacomb::caps() const
{
	return Caps::Default;
}

ColourDepth Tileset_Catacomb::colourDepth() const
{
	return this->depth;
}

Point Tileset_Catacomb::dimensions() const
{
	return {CAT_TILE_WIDTH, CAT_TILE_HEIGHT};
}

unsigned int Tileset_Catacomb::layoutWidth() const
{
	return 2;
}

std::unique_ptr<Image> Tileset_Catacomb::openImage(FileHandle& id)
{
	switch (this->depth) {
		case ColourDepth::EGA:
			return std::make_unique<Image_EGA_Planar>(
				this->open(id, true), 0, this->dimensions(),
				EGAPlaneLayout{
					EGAPlanePurpose::Blue1,
					EGAPlanePurpose::Green1,
					EGAPlanePurpose::Red1,
					EGAPlanePurpose::Intensity1,
				},
				this->palette()
			);
		case ColourDepth::CGA:
			return std::make_unique<Image_EGA_Linear>(
				this->open(id, true), 0, this->dimensions(),
				EGAPlaneLayout{
					EGAPlanePurpose::Green1,
					EGAPlanePurpose::Blue1,
				},
				bitstream::endian::bigEndian,
				createPalette_CGA(CGAPaletteType::CyanMagentaBright)
			);
		default:
			throw stream::error("Invalid depth.");
	}
}

Tileset::FileHandle Tileset_Catacomb::insert(const FileHandle& idBeforeThis,
	File::Attribute attr)
{
	return this->insert(idBeforeThis, "", this->lenTile, FILETYPE_CAT, attr);
}

} // namespace gamegraphics
} // namespace camoto
