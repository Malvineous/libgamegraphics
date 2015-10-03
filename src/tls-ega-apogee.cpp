/**
 * @file  tls-ega-apogee.cpp
 * @brief Common EGA tileset routines used by Apogee games
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Cosmo_Tileset_Format
 *   http://www.shikadi.net/moddingwiki/Major_Stryker_Tileset_Format
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
#include "img-ega-byteplanar.hpp"
#include "tls-ega-apogee.hpp"

#define FILETYPE_APOGEE "tile/apogee-ega"

namespace camoto {
namespace gamegraphics {

/// Offset of first tileset in a tileset collection.
#define EGA_APOGEE_FIRST_TILE_OFFSET 0

Tileset_EGAApogee::Tileset_EGAApogee(std::unique_ptr<stream::inout> content,
	Point tileDimensions, PlaneCount numPlanes, unsigned int idealWidth,
	std::shared_ptr<const Palette> pal)
	:	Tileset_FAT(std::move(content), EGA_APOGEE_FIRST_TILE_OFFSET, ARCH_NO_FILENAMES),
		Tileset_FAT_FixedTileSize(tileDimensions.x / 8 * tileDimensions.y * (unsigned int)numPlanes),
		tileDimensions(tileDimensions),
		numPlanes(numPlanes),
		idealWidth(idealWidth)
{
	this->pal = pal;

	int tileSize = this->tileDimensions.x / 8 * this->tileDimensions.y * (unsigned int)this->numPlanes;

	stream::pos len = this->content->size();

	this->content->seekg(0, stream::start);
	int numImages = len / tileSize;

	this->vcFAT.reserve(numImages);
	for (int i = 0; i < numImages; i++) {
		auto fat = std::make_unique<FATEntry>();
		fat->bValid = true;
		fat->fAttr = File::Attribute::Default;
		fat->iIndex = i;
		fat->iOffset = i * tileSize;
		fat->storedSize = fat->realSize = tileSize;
		fat->lenHeader = 0;
		fat->type = FILETYPE_APOGEE;
		this->vcFAT.push_back(std::move(fat));
	}

}

Tileset_EGAApogee::~Tileset_EGAApogee()
{
}

Tileset::Caps Tileset_EGAApogee::caps() const
{
	return Caps::HasDimensions
		| (this->pal ? Caps::HasPalette : Caps::Default);
}

ColourDepth Tileset_EGAApogee::colourDepth() const
{
	return ColourDepth::EGA;
}

Point Tileset_EGAApogee::dimensions() const
{
	return this->tileDimensions;
}

unsigned int Tileset_EGAApogee::layoutWidth() const
{
	return this->idealWidth;
}

std::unique_ptr<Image> Tileset_EGAApogee::openImage(const FileHandle& id)
{
	EGAPlaneLayout planes;
	switch (this->numPlanes) {
		case PlaneCount::Solid:
			planes = {
				EGAPlanePurpose::Blue1,
				EGAPlanePurpose::Green1,
				EGAPlanePurpose::Red1,
				EGAPlanePurpose::Intensity1,
			};
			break;
		case PlaneCount::Masked:
			planes = {
				EGAPlanePurpose::Opaque0,
				EGAPlanePurpose::Blue1,
				EGAPlanePurpose::Green1,
				EGAPlanePurpose::Red1,
				EGAPlanePurpose::Intensity1,
			};
			break;
	}
	return std::make_unique<Image_EGA_BytePlanar>(
		this->open(id, true), 0, this->dimensions(), planes, this->palette()
	);
}

const Tileset::FileHandle Tileset_EGAApogee::insert(const FileHandle& idBeforeThis,
	File::Attribute attr)
{
	auto newHandle = this->insert(idBeforeThis, "", this->lenTile,
		FILETYPE_APOGEE, attr);
	return newHandle;
}

} // namespace gamegraphics
} // namespace camoto
