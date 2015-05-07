/**
 * @file  tls-ccomic2.cpp
 * @brief Captain Comic II tileset
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Captain_Comic_II
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
#include <camoto/stream_filtered.hpp>
#include <camoto/util.hpp> // make_unique
#include "tileset-fat.hpp"
#include "tileset-fat-fixed_tile_size.hpp"
#include "img-ega-planar.hpp"
#include "tls-ccomic2.hpp"
#include "filter-ccomic2.hpp"

namespace camoto {
namespace gamegraphics {

#define CC2_TILE_WIDTH 16
#define CC2_TILE_HEIGHT 16

#define FILETYPE_CCOMIC2 "tile/ccomic2"

constexpr int CC2_firstTileOffset(PlaneCount numPlanes)
{
	return (numPlanes == PlaneCount::Solid) ? 6 : 0;
}

class Tileset_CComic2:
	virtual public Tileset_FAT,
	virtual public Tileset_FAT_FixedTileSize
{
	public:
		Tileset_CComic2(std::unique_ptr<stream::inout> content,
			PlaneCount numPlanes);
		virtual ~Tileset_CComic2();

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
		PlaneCount numPlanes;
};

//
// TilesetType_CComic2
//

TilesetType_CComic2::TilesetType_CComic2()
{
}

TilesetType_CComic2::~TilesetType_CComic2()
{
}

std::string TilesetType_CComic2::code() const
{
	return "tls-ccomic2";
}

std::string TilesetType_CComic2::friendlyName() const
{
	return "Captain Comic II Tileset";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> TilesetType_CComic2::fileExtensions() const
{
	return {"0"};
}

std::vector<std::string> TilesetType_CComic2::games() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Captain Comic 2");
	return vcGames;
}

TilesetType_CComic2::Certainty TilesetType_CComic2::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();
	if (len > 65535) return DefinitelyNo; // file too large (probably)
	content.seekg(0, stream::start);
	uint16_t num;
	for (int i = 0; i < 3; i++) {
		content >> u16le(num);
		if (num > 512) return DefinitelyNo; // too many tiles (probably)
	}
	return Unsure;
}

std::shared_ptr<Tileset> TilesetType_CComic2::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	constexpr auto numPlanes = PlaneCount::Solid;

	auto content_filtered = std::make_unique<stream::filtered>(
		std::move(content),
		std::make_shared<filter_ccomic2_unrle>(CC2_firstTileOffset(numPlanes)),
		std::make_shared<filter_ccomic2_rle>(CC2_firstTileOffset(numPlanes)),
		nullptr
	);

	content_filtered->seekp(0, stream::start);
	// Zero tiles, 0x0
	*content_filtered
		<< u16le(0)
		<< u16le(0)
		<< u16le(0)
	;
	return std::make_shared<Tileset_CComic2>(std::move(content_filtered),
		numPlanes);
}

std::shared_ptr<Tileset> TilesetType_CComic2::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	constexpr auto numPlanes = PlaneCount::Solid;

	auto content_filtered = std::make_unique<stream::filtered>(
		std::move(content),
		std::make_shared<filter_ccomic2_unrle>(CC2_firstTileOffset(numPlanes)),
		std::make_shared<filter_ccomic2_rle>(CC2_firstTileOffset(numPlanes)),
		nullptr
	);

	return std::make_shared<Tileset_CComic2>(std::move(content_filtered),
		numPlanes);
}

SuppFilenames TilesetType_CComic2::getRequiredSupps(
	const std::string& filenameGraphics) const
{
	return {};
}


//
// Tileset_CComic2
//

Tileset_CComic2::Tileset_CComic2(std::unique_ptr<stream::inout> content,
	PlaneCount numPlanes)
	:	Tileset_FAT(std::move(content), CC2_firstTileOffset(numPlanes), ARCH_NO_FILENAMES),
		Tileset_FAT_FixedTileSize(CC2_TILE_WIDTH / 8 * CC2_TILE_HEIGHT * (int)numPlanes),
		numPlanes(numPlanes)
{
	int lenHeader = CC2_firstTileOffset(numPlanes);

	stream::pos len = this->content->size() - lenHeader;
	int numImages = len / this->lenTile;

	this->vcFAT.reserve(numImages);
	for (int i = 0; i < numImages; i++) {
		auto f = std::make_unique<FATEntry>();

		f->iIndex = i;
		f->iOffset = lenHeader + i * this->lenTile;
		f->storedSize = this->lenTile;
		f->lenHeader = 0;
		f->type = FILETYPE_CCOMIC2;
		f->fAttr = File::Attribute::Default;
		f->bValid = true;
		f->realSize = f->storedSize;

		this->vcFAT.push_back(std::move(f));
	}

}

Tileset_CComic2::~Tileset_CComic2()
{
}

Tileset::Caps Tileset_CComic2::caps() const
{
	return Caps::HasDimensions;
}

ColourDepth Tileset_CComic2::colourDepth() const
{
	return ColourDepth::EGA;
}

Point Tileset_CComic2::dimensions() const
{
	return {CC2_TILE_WIDTH, CC2_TILE_HEIGHT};
}

unsigned int Tileset_CComic2::layoutWidth() const
{
	return 4;
}

std::unique_ptr<Image> Tileset_CComic2::openImage(FileHandle& id)
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

Tileset::FileHandle Tileset_CComic2::insert(const FileHandle& idBeforeThis,
	File::Attribute attr)
{
	auto newHandle = this->insert(idBeforeThis, "", this->lenTile,
		FILETYPE_CCOMIC2, attr);
	return newHandle;
}

} // namespace gamegraphics
} // namespace camoto
