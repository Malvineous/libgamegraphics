/**
 * @file  tls-ccomic.cpp
 * @brief Captain Comic tileset
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Captain_Comic
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
#include "tls-ccomic.hpp"

namespace camoto {
namespace gamegraphics {

#define CCA_TILE_WIDTH 16
#define CCA_TILE_HEIGHT 16

#define FILETYPE_CCOMIC "tile/ccomic"

class Tileset_CComic:
	virtual public Tileset_FAT,
	virtual public Tileset_FAT_FixedTileSize
{
	public:
		Tileset_CComic(std::unique_ptr<stream::inout> content,
			PlaneCount numPlanes);
		virtual ~Tileset_CComic();

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
		PlaneCount numPlanes;
};


//
// TilesetType_CComic
//

TilesetType_CComic::TilesetType_CComic()
{
}

TilesetType_CComic::~TilesetType_CComic()
{
}

std::string TilesetType_CComic::code() const
{
	return "tls-ccomic";
}

std::string TilesetType_CComic::friendlyName() const
{
	return "Captain Comic Tileset";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> TilesetType_CComic::fileExtensions() const
{
	return {"tt2"};
}

std::vector<std::string> TilesetType_CComic::games() const
{
	return {
		"Captain Comic",
	};
}

TilesetType_CComic::Certainty TilesetType_CComic::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();
	if (len % 128 == 4) return PossiblyYes;
	return DefinitelyNo;
}

std::shared_ptr<Tileset> TilesetType_CComic::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->seekp(0, stream::start);
	// Zero tiles
	content->write("\x00\x00\x00\x00", 4);
	return std::make_shared<Tileset_CComic>(std::move(content), PlaneCount::Solid);
}

std::shared_ptr<Tileset> TilesetType_CComic::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	return std::make_shared<Tileset_CComic>(std::move(content), PlaneCount::Solid);
}

SuppFilenames TilesetType_CComic::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	// No supplemental types/empty list
	return {};
}


//
// TilesetType_CComic_Sprite
//

std::string TilesetType_CComic_Sprite::code() const
{
	return "tls-ccomic-sprite";
}

std::string TilesetType_CComic_Sprite::friendlyName() const
{
	return "Captain Comic Sprite";
}

TilesetType_CComic_Sprite::Certainty TilesetType_CComic_Sprite::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();
	if (len % 160 == 0) return PossiblyYes;
	return DefinitelyNo;
}

std::shared_ptr<Tileset> TilesetType_CComic_Sprite::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->seekp(0, stream::start);
	// Zero tiles
	return std::make_shared<Tileset_CComic>(std::move(content), PlaneCount::Masked);
}

std::shared_ptr<Tileset> TilesetType_CComic_Sprite::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	return std::make_shared<Tileset_CComic>(std::move(content), PlaneCount::Masked);
}


//
// Tileset_CComic
//

constexpr int firstTileOffset(PlaneCount numPlanes)
{
	return (numPlanes == PlaneCount::Solid) ? 4 : 0;
}

Tileset_CComic::Tileset_CComic(std::unique_ptr<stream::inout> content,
	PlaneCount numPlanes)
	:	Tileset_FAT(std::move(content), firstTileOffset(numPlanes), ARCH_NO_FILENAMES),
		Tileset_FAT_FixedTileSize(CCA_TILE_WIDTH / 8 * CCA_TILE_HEIGHT * (int)numPlanes),
		numPlanes(numPlanes)
{
	int tileSize = (int)this->numPlanes << 5; // multiply by 32 (bytes per plane)
	int lenHeader = firstTileOffset(numPlanes);

	stream::pos len = this->content->size() - lenHeader;
	int numImages = len / tileSize;

	this->vcFAT.reserve(numImages);
	for (int i = 0; i < numImages; i++) {
		auto f = std::make_unique<FATEntry>();

		f->iIndex = i;
		f->iOffset = lenHeader + i * tileSize;
		f->storedSize = tileSize;
		f->lenHeader = 0;
		f->type = FILETYPE_CCOMIC;
		f->fAttr = File::Attribute::Default;
		f->bValid = true;
		f->realSize = f->storedSize;

		this->vcFAT.push_back(std::move(f));
	}

}

Tileset_CComic::~Tileset_CComic()
{
}

Tileset::Caps Tileset_CComic::caps() const
{
	return Caps::HasDimensions;
}

ColourDepth Tileset_CComic::colourDepth() const
{
	return ColourDepth::EGA;
}

Point Tileset_CComic::dimensions() const
{
	return {CCA_TILE_WIDTH, CCA_TILE_HEIGHT};
}

unsigned int Tileset_CComic::layoutWidth() const
{
	return 4;
}

std::unique_ptr<Image> Tileset_CComic::openImage(const FileHandle& id)
{
	EGAPlaneLayout planes = {
		EGAPlanePurpose::Blue1,
		EGAPlanePurpose::Green1,
		EGAPlanePurpose::Red1,
		EGAPlanePurpose::Intensity1,
		(this->numPlanes == PlaneCount::Masked)
			? EGAPlanePurpose::Opaque1 : EGAPlanePurpose::Unused,
		EGAPlanePurpose::Unused,
	};

	return std::make_unique<Image_EGA_Planar>(
		this->open(id, true), 0, this->dimensions(), planes, this->palette()
	);
}

const Tileset::FileHandle Tileset_CComic::insert(const FileHandle& idBeforeThis,
	File::Attribute attr)
{
	return this->insert(idBeforeThis, "", this->lenTile, FILETYPE_CCOMIC, attr);
}

} // namespace gamegraphics
} // namespace camoto
