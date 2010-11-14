/**
 * @file   main.cpp
 * @brief  Entry point for libgamegraphics.
 *
 * Copyright (C) 2010 Adam Nielsen <malvineous@shikadi.net>
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

#include <string>
#include <camoto/debug.hpp>
#include <camoto/gamegraphics.hpp>

// Include all the file formats for the Manager to load
#include "tls-ccaves.hpp"
#include "tls-zone66.hpp"
#include "img-zone66_tile.hpp"
#include "img-vga-raw.hpp"
#include "pal-vga-raw.hpp"

namespace camoto {
namespace gamegraphics {

ManagerPtr getManager()
	throw ()
{
	return ManagerPtr(new Manager());
}

Manager::Manager()
	throw ()
{
	this->vcTilesetTypes.push_back(TilesetTypePtr(new CCavesTilesetType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new Zone66TilesetType()));

	this->vcImageTypes.push_back(ImageTypePtr(new Zone66TileImageType()));
	this->vcImageTypes.push_back(ImageTypePtr(new VGARawImageType()));
	this->vcImageTypes.push_back(ImageTypePtr(new VGAPaletteImageType()));
}

Manager::~Manager()
	throw ()
{
}

TilesetTypePtr Manager::getTilesetType(int iIndex)
	throw ()
{
	if (iIndex >= this->vcTilesetTypes.size()) return TilesetTypePtr();
	return this->vcTilesetTypes[iIndex];
}

TilesetTypePtr Manager::getTilesetTypeByCode(const std::string& strCode)
	throw ()
{
	for (VC_TILESETTYPE::const_iterator i = this->vcTilesetTypes.begin();
		i != this->vcTilesetTypes.end();
		i++
	) {
		if ((*i)->getCode().compare(strCode) == 0) return *i;
	}
	return TilesetTypePtr();
}

ImageTypePtr Manager::getImageType(int iIndex)
	throw ()
{
	if (iIndex >= this->vcImageTypes.size()) return ImageTypePtr();
	return this->vcImageTypes[iIndex];
}

ImageTypePtr Manager::getImageTypeByCode(const std::string& strCode)
	throw ()
{
	for (VC_IMAGETYPE::const_iterator i = this->vcImageTypes.begin();
		i != this->vcImageTypes.end();
		i++
	) {
		if ((*i)->getCode().compare(strCode) == 0) return *i;
	}
	return ImageTypePtr();
}

} // namespace gamegraphics
} // namespace camoto
