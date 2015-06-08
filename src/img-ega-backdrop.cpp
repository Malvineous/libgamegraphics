/**
 * @file  img-ega-backdrop.cpp
 * @brief Image implementation of EGA "backdrop" style tiled images.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Cosmo_Tileset_Format
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

#include <camoto/util.hpp> // make_unique
#include "img-ega-backdrop.hpp"
#include "image-from_tileset.hpp"
#include "tls-ega-apogee.hpp"

using namespace camoto;
using namespace camoto::gamegraphics;

ImageType_Backdrop::ImageType_Backdrop(Point dimsTile, Point dimsTileset,
	PlaneCount planeCount)
	:	dimsTile(dimsTile),
		dimsTileset(dimsTileset),
		planeCount(planeCount)
{
}

ImageType::Certainty ImageType_Backdrop::isInstance(stream::input& content)
	const
{
	// Files are a fixed size.
	// TESTED BY: TODO
	if (content.size() != this->getExpectedSize()) return DefinitelyNo;

	// TESTED BY: TODO
	return PossiblyYes;
}

std::unique_ptr<Image> ImageType_Backdrop::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->truncate(this->getExpectedSize());
	return this->open(std::move(content), suppData);
}

std::unique_ptr<Image> ImageType_Backdrop::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	return std::make_unique<Image_FromTileset>(
		std::make_unique<Tileset_EGAApogee>(
			std::move(content),
			this->dimsTile,
			PlaneCount::Solid, 1, nullptr
		),
		0, this->dimsTileset.x, this->dimsTileset
	);
}

SuppFilenames ImageType_Backdrop::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	return {};
}

stream::len ImageType_Backdrop::getExpectedSize() const
{
	auto tileSize = this->dimsTile.x / 8 * this->dimsTile.y *
		(unsigned int)this->planeCount;
	return this->dimsTileset.x * this->dimsTileset.y * tileSize;
}


ImageType_CosmoBackdrop::ImageType_CosmoBackdrop()
	:	ImageType_Backdrop(
			Point{ 8,  8}, // tile size
			Point{40, 18}, // image size (in tiles)
			PlaneCount::Solid
		)
{
}

std::string ImageType_CosmoBackdrop::code() const
{
	return "img-cosmo-backdrop";
}

std::string ImageType_CosmoBackdrop::friendlyName() const
{
	return "Cosmo's Cosmic Adventures level backdrop";
}

std::vector<std::string> ImageType_CosmoBackdrop::fileExtensions() const
{
	return {"mni"};
}

std::vector<std::string> ImageType_CosmoBackdrop::games() const
{
	return {"Cosmo's Cosmic Adventures"};
}


ImageType_Nukem2Backdrop::ImageType_Nukem2Backdrop()
	:	ImageType_Backdrop(
			Point{ 8,  8}, // tile size
			Point{40, 25}, // image size (in tiles)
			PlaneCount::Solid
		)
{
}

std::string ImageType_Nukem2Backdrop::code() const
{
	return "img-nukem2-backdrop";
}

std::string ImageType_Nukem2Backdrop::friendlyName() const
{
	return "Duke Nukem II level backdrop";
}

std::vector<std::string> ImageType_Nukem2Backdrop::fileExtensions() const
{
	return {"mni"};
}

std::vector<std::string> ImageType_Nukem2Backdrop::games() const
{
	return {"Duke Nukem II"};
}
