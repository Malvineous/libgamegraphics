/**
 * @file   tls-wordresc-extra.cpp
 * @brief  Word Rescue virtual tileset (from combining bits of other images)
 *
 * Copyright (C) 2010-2011 Adam Nielsen <malvineous@shikadi.net>
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

#include <camoto/gamegraphics/tilesetFromList.hpp>
#include "tls-wordresc-extra.hpp"
#include "img-pcx.hpp"

namespace camoto {
namespace gamegraphics {

WordrescExtraTilesetType::WordrescExtraTilesetType()
	throw ()
{
}

WordrescExtraTilesetType::~WordrescExtraTilesetType()
	throw ()
{
}

std::string WordrescExtraTilesetType::getCode() const
	throw ()
{
	return "tls-wordresc-extra";
}

std::string WordrescExtraTilesetType::getFriendlyName() const
	throw ()
{
	return "Word Rescue Virtual Tileset";
}

std::vector<std::string> WordrescExtraTilesetType::getFileExtensions() const
	throw ()
{
	std::vector<std::string> vcExtensions;
	return vcExtensions;
}

std::vector<std::string> WordrescExtraTilesetType::getGameList() const
	throw ()
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Word Rescue");
	return vcGames;
}

WordrescExtraTilesetType::Certainty WordrescExtraTilesetType::isInstance(iostream_sptr psGraphics) const
	throw (std::ios::failure)
{
	return Unsure;
}

TilesetPtr WordrescExtraTilesetType::create(iostream_sptr psGraphics,
	FN_TRUNCATE fnTruncate, SuppData& suppData) const
	throw (std::ios::failure)
{
	// TODO
	throw std::ios::failure("Not yet implemented");
}

TilesetPtr WordrescExtraTilesetType::open(iostream_sptr psGraphics,
	FN_TRUNCATE fnTruncate, SuppData& suppData) const
	throw (std::ios::failure)
{
	if (suppData.find(SuppItem::Extra1) == suppData.end()) {
		throw std::ios::failure("Missing supp item type Extra1");
	}

	TilesetFromList::TileList tileList;

	ImagePtr img1(new PCXImage(psGraphics, fnTruncate));
	TilesetFromList::Tile t;

	t.img = img1; t.xOffset = 209; t.yOffset =  40; t.width =  23; t.height =  23; tileList.push_back(t);
	t.img = img1; t.xOffset = 232; t.yOffset =  40; t.width =  24; t.height =  32; tileList.push_back(t);
	t.img = img1; t.xOffset = 256; t.yOffset =  40; t.width =  24; t.height =  32; tileList.push_back(t);

	t.img = img1; t.xOffset =  16; t.yOffset =  33; t.width =  32; t.height =  40; tileList.push_back(t);
	t.img = img1; t.xOffset =  48; t.yOffset =  33; t.width =  32; t.height =  40; tileList.push_back(t);
	t.img = img1; t.xOffset =  80; t.yOffset =  33; t.width =  32; t.height =  40; tileList.push_back(t);
	t.img = img1; t.xOffset = 112; t.yOffset =  33; t.width =  32; t.height =  40; tileList.push_back(t);
	t.img = img1; t.xOffset = 144; t.yOffset =  33; t.width =  32; t.height =  40; tileList.push_back(t);
	t.img = img1; t.xOffset = 176; t.yOffset =  33; t.width =  32; t.height =  40; tileList.push_back(t);

	t.img = img1; t.xOffset =  16; t.yOffset =  73; t.width =  32; t.height =  40; tileList.push_back(t);
	t.img = img1; t.xOffset =  48; t.yOffset =  73; t.width =  32; t.height =  40; tileList.push_back(t);
	t.img = img1; t.xOffset =  80; t.yOffset =  73; t.width =  32; t.height =  40; tileList.push_back(t);
	t.img = img1; t.xOffset = 112; t.yOffset =  73; t.width =  32; t.height =  40; tileList.push_back(t);
	t.img = img1; t.xOffset = 144; t.yOffset =  73; t.width =  32; t.height =  40; tileList.push_back(t);
	t.img = img1; t.xOffset = 176; t.yOffset =  73; t.width =  32; t.height =  40; tileList.push_back(t);

	t.img = img1; t.xOffset =  16; t.yOffset = 120; t.width =  24; t.height =  24; tileList.push_back(t);
	t.img = img1; t.xOffset =  40; t.yOffset = 120; t.width =  24; t.height =  24; tileList.push_back(t);
	t.img = img1; t.xOffset =  64; t.yOffset = 120; t.width =  24; t.height =  24; tileList.push_back(t);
	t.img = img1; t.xOffset =  88; t.yOffset = 120; t.width =  24; t.height =  24; tileList.push_back(t);
	t.img = img1; t.xOffset = 112; t.yOffset = 120; t.width =  24; t.height =  24; tileList.push_back(t);
	t.img = img1; t.xOffset = 136; t.yOffset = 120; t.width =  24; t.height =  24; tileList.push_back(t);
	t.img = img1; t.xOffset = 160; t.yOffset = 120; t.width =  24; t.height =  24; tileList.push_back(t);
	t.img = img1; t.xOffset = 184; t.yOffset = 120; t.width =  24; t.height =  24; tileList.push_back(t);

	t.img = img1; t.xOffset =  16; t.yOffset = 144; t.width =  24; t.height =  24; tileList.push_back(t);
	t.img = img1; t.xOffset =  40; t.yOffset = 144; t.width =  24; t.height =  24; tileList.push_back(t);
	t.img = img1; t.xOffset =  64; t.yOffset = 144; t.width =  24; t.height =  24; tileList.push_back(t);
	t.img = img1; t.xOffset =  88; t.yOffset = 144; t.width =  24; t.height =  24; tileList.push_back(t);
	t.img = img1; t.xOffset = 112; t.yOffset = 144; t.width =  24; t.height =  24; tileList.push_back(t);
	t.img = img1; t.xOffset = 136; t.yOffset = 144; t.width =  24; t.height =  24; tileList.push_back(t);
	t.img = img1; t.xOffset = 160; t.yOffset = 144; t.width =  24; t.height =  24; tileList.push_back(t);
	t.img = img1; t.xOffset = 184; t.yOffset = 144; t.width =  24; t.height =  24; tileList.push_back(t);

	t.img = img1; t.xOffset = 208; t.yOffset =  72; t.width =  71; t.height =  72; tileList.push_back(t);

	SuppItem *next = &suppData[SuppItem::Extra1];
	ImagePtr img2(new PCXImage(next->stream, next->fnTruncate));

	t.img = img2; t.xOffset =   0; t.yOffset =   0; t.width =  16; t.height =  16; tileList.push_back(t);
	t.img = img2; t.xOffset =  16; t.yOffset =   0; t.width =  16; t.height =  16; tileList.push_back(t);
	t.img = img2; t.xOffset =  32; t.yOffset =   0; t.width =  16; t.height =  16; tileList.push_back(t);
	t.img = img2; t.xOffset =  48; t.yOffset =   0; t.width =  16; t.height =  16; tileList.push_back(t);
	t.img = img2; t.xOffset =  64; t.yOffset =   0; t.width =  16; t.height =  16; tileList.push_back(t);
	t.img = img2; t.xOffset =  80; t.yOffset =   0; t.width =  16; t.height =  16; tileList.push_back(t);
	t.img = img2; t.xOffset =  96; t.yOffset =   0; t.width =  16; t.height =  16; tileList.push_back(t);
	t.img = img2; t.xOffset = 112; t.yOffset =   0; t.width =  16; t.height =  16; tileList.push_back(t);
	t.img = img2; t.xOffset = 128; t.yOffset =   0; t.width =  16; t.height =  16; tileList.push_back(t);

	t.img = img2; t.xOffset =   0; t.yOffset =  16; t.width =  16; t.height =  16; tileList.push_back(t);
	t.img = img2; t.xOffset =  16; t.yOffset =  16; t.width =  16; t.height =  16; tileList.push_back(t);
	t.img = img2; t.xOffset =  32; t.yOffset =  16; t.width =  16; t.height =  16; tileList.push_back(t);
	t.img = img2; t.xOffset =  48; t.yOffset =  16; t.width =  16; t.height =  16; tileList.push_back(t);
	t.img = img2; t.xOffset =  64; t.yOffset =  16; t.width =  16; t.height =  16; tileList.push_back(t);
	t.img = img2; t.xOffset =  80; t.yOffset =  16; t.width =  16; t.height =  16; tileList.push_back(t);
	t.img = img2; t.xOffset =  96; t.yOffset =  16; t.width =  16; t.height =  16; tileList.push_back(t);
	t.img = img2; t.xOffset = 112; t.yOffset =  16; t.width =  16; t.height =  16; tileList.push_back(t);
	t.img = img2; t.xOffset = 128; t.yOffset =  16; t.width =  16; t.height =  16; tileList.push_back(t);

	t.img = img2; t.xOffset =   0; t.yOffset =  32; t.width =  16; t.height =  16; tileList.push_back(t);
	t.img = img2; t.xOffset =  16; t.yOffset =  32; t.width =  16; t.height =  16; tileList.push_back(t);
	t.img = img2; t.xOffset =  32; t.yOffset =  32; t.width =  16; t.height =  16; tileList.push_back(t);
	t.img = img2; t.xOffset =  48; t.yOffset =  32; t.width =  16; t.height =  16; tileList.push_back(t);
	t.img = img2; t.xOffset =  64; t.yOffset =  32; t.width =  16; t.height =  16; tileList.push_back(t);
	t.img = img2; t.xOffset =  80; t.yOffset =  32; t.width =  16; t.height =  16; tileList.push_back(t);
	t.img = img2; t.xOffset =  96; t.yOffset =  32; t.width =  16; t.height =  16; tileList.push_back(t);
	t.img = img2; t.xOffset = 112; t.yOffset =  32; t.width =  16; t.height =  16; tileList.push_back(t);
	t.img = img2; t.xOffset = 128; t.yOffset =  32; t.width =  16; t.height =  16; tileList.push_back(t);

	return TilesetPtr(new TilesetFromList(tileList));
}

SuppFilenames WordrescExtraTilesetType::getRequiredSupps(const std::string& filenameGraphics) const
	throw ()
{
	// No supplemental types/empty list
	SuppFilenames supp;
	supp[SuppItem::Extra1] = "font.wr";
	return supp;
}

} // namespace gamegraphics
} // namespace camoto
