/**
 * @file  tls-wacky.cpp
 * @brief Wacky Wheels tileset
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Wacky_Wheels
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
#include <camoto/gamegraphics/tileset-from_image_list.hpp>
#include "tls-wacky.hpp"
#include "img-pcx.hpp"

namespace camoto {
namespace gamegraphics {

#define WW_TILE_WIDTH  32 ///< Width of each tile
#define WW_TILE_HEIGHT 32 ///< Height of each tile

TilesetType_Wacky::TilesetType_Wacky()
{
}

TilesetType_Wacky::~TilesetType_Wacky()
{
}

std::string TilesetType_Wacky::code() const
{
	return "tls-wacky";
}

std::string TilesetType_Wacky::friendlyName() const
{
	return "Wacky Wheels Tileset";
}

std::vector<std::string> TilesetType_Wacky::fileExtensions() const
{
	return {"pcx"};
}

std::vector<std::string> TilesetType_Wacky::games() const
{
	return {"Wacky Wheels"};
}

TilesetType_Wacky::Certainty TilesetType_Wacky::isInstance(
	stream::input& content) const
{
	ImageType_PCX_LinearVGA pcx;
	if (!pcx.isInstance(content)) return DefinitelyNo;

/// @todo Implement this when TilesetType::open() can take a read-only stream
/*
	SuppData sd;
	std::shared_ptr<Image> img = pcx.open(content, sd);

	unsigned int width, height;
	img->getDimensions(&width, &height);
	if ((width != 320) || (height != 200)) return DefinitelyNo;
*/
	return PossiblyYes; // best we can hope for
}

std::shared_ptr<Tileset> TilesetType_Wacky::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	// TODO
	throw stream::error("Not yet implemented");
}

std::shared_ptr<Tileset> TilesetType_Wacky::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	auto img = std::make_unique<Image_PCX>(std::move(content), 8, 1, true);
	auto imgDims = img->dimensions();
	return make_Tileset_FromImageList(
		{
			{
				std::move(img),
				Tileset_FromImageList::Item::AttachmentType::Append,
				Tileset_FromImageList::Item::SplitType::UniformTiles,
				{WW_TILE_WIDTH, WW_TILE_HEIGHT},
				{0, 0, imgDims.x, imgDims.y},
				{}
			}
		},
		320 / WW_TILE_WIDTH // pref width (tiles per row)
	);
}

SuppFilenames TilesetType_Wacky::getRequiredSupps(
	const std::string& filenameGraphics) const
{
	return {};
}

} // namespace gamegraphics
} // namespace camoto
