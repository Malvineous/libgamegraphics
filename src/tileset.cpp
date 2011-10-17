/**
 * @file   tileset.cpp
 * @brief  Declaration of Tileset class, for accessing files storing multiple
 *         game images.
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

#include <cassert>
#include <camoto/gamegraphics/tileset.hpp>

namespace camoto {
namespace gamegraphics {

Tileset::Entry::Entry()
{
}

Tileset::Entry::~Entry()
{
}

Tileset::Tileset()
	throw ()
{
}

Tileset::~Tileset()
	throw ()
{
}

TilesetPtr Tileset::openTileset(const EntryPtr& id)
	throw (stream::error)
{
	// Caller didn't check EntryPtr->attr
	assert(false);
	throw stream::error("this tileset has no images"
		" (this is a bug - the caller should have checked the EntryPtr's"
		" attributes to detect this)");
}

ImagePtr Tileset::openImage(const EntryPtr& id)
	throw (stream::error)
{
	// Caller didn't check EntryPtr->attr
	assert(false);
	throw stream::error("this tileset has no images"
		" (this is a bug - the caller should have checked the EntryPtr's"
		" attributes to detect this)");
}

void Tileset::getTilesetDimensions(unsigned int *width, unsigned int *height)
	throw ()
{
	*width = 0;
	*height = 0;
	return;
}

void Tileset::setTilesetDimensions(unsigned int width, unsigned int height)
	throw (stream::error)
{
	// Caller didn't check getCaps()
	assert(false);
	throw stream::error("this tileset cannot have its tile size changed"
		" (this is a bug - the caller should have used getCaps() to detect this)");
}

unsigned int Tileset::getLayoutWidth()
	throw ()
{
	return 0;
}

PaletteTablePtr Tileset::getPalette()
	throw ()
{
	// Caller didn't check getCaps()
	assert(false);
	return PaletteTablePtr();
}

void Tileset::setPalette(PaletteTablePtr newPalette)
	throw (stream::error)
{
	// Caller didn't check getCaps()
	assert(false);
	throw stream::error("this format doesn't support palettes"
		" (this is a bug - the caller should have used getCaps() to detect this)");
	return;
}

} // namespace gamegraphics
} // namespace camoto
