/**
 * @file  basetileset.cpp
 * @brief Base class for all Tileset subclasses.
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

#include <cassert>
#include "basetileset.hpp"

namespace camoto {
namespace gamegraphics {

std::string Tileset_Base::Tileset_BaseEntry::getName() const
{
	return this->name;
}

bool Tileset_Base::Tileset_BaseEntry::isValid() const
{
	return this->valid;
}

int Tileset_Base::Tileset_BaseEntry::getAttr() const
{
	return this->attr;
}

Tileset_Base::Tileset_Base()
{
}

Tileset_Base::~Tileset_Base()
{
}

TilesetPtr Tileset_Base::openTileset(const EntryPtr& id)
{
	// Caller didn't check EntryPtr->attr
	assert(false);
	throw stream::error("this tileset has no images"
		" (this is a bug - the caller should have checked the EntryPtr's"
		" attributes to detect this)");
}

ImagePtr Tileset_Base::openImage(const EntryPtr& id)
{
	// Caller didn't check EntryPtr->attr
	assert(false);
	throw stream::error("this tileset has no images"
		" (this is a bug - the caller should have checked the EntryPtr's"
		" attributes to detect this)");
}

void Tileset_Base::getTilesetDimensions(unsigned int *width, unsigned int *height)
{
	*width = 0;
	*height = 0;
	return;
}

void Tileset_Base::setTilesetDimensions(unsigned int width, unsigned int height)
{
	// Caller didn't check getCaps()
	assert(false);
	throw stream::error("this tileset cannot have its tile size changed"
		" (this is a bug - the caller should have used getCaps() to detect this)");
}

unsigned int Tileset_Base::getLayoutWidth()
{
	return 0;
}

PaletteTablePtr Tileset_Base::getPalette()
{
	// Caller didn't check getCaps()
	assert(false);
	return PaletteTablePtr();
}

void Tileset_Base::setPalette(PaletteTablePtr newPalette)
{
	// Caller didn't check getCaps()
	assert(false);
	throw stream::error("this format can't have its palette changed"
		" (this is a bug - the caller should have used getCaps() to detect this)");
	return;
}

} // namespace gamegraphics
} // namespace camoto
