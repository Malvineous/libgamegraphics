/**
 * @file   img-palette_only.cpp
 * @brief  Graphics implementation of a Palette file.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Crystal_Caves_Tileset_Format
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

#include "img-palette.hpp"

namespace camoto {
namespace gamegraphics {

PaletteGraphics::PaletteGraphics()
	throw ()
{
}

PaletteGraphics::~PaletteGraphics()
	throw ()
{
}

int PaletteGraphics::getCaps()
	throw ()
{
	return HasPalette;
}

int PaletteGraphics::getTilesetCount()
	throw ()
{
	return 0;
}

GraphicsPtr PaletteGraphics::getTileset(int index)
	throw ()
{
	// This function should never be called
	assert(false);
}

void PaletteGraphics::getTileSize(unsigned int *width, unsigned int *height)
	throw ()
{
	*width = 0;
	*height = 0;
	return;
}

unsigned int PaletteGraphics::getLayoutWidth()
	throw ()
{
	return 0;
}

void PaletteGraphics::setTileSize(unsigned int x, unsigned int y)
	throw (std::ios::failure)
{
	// This function should never be called (as per getCaps() response.)
	assert(false);
}

int PaletteGraphics::getImageCount()
	throw ()
{
	return 0;
}

void PaletteGraphics::setImageCount(int count)
	throw (std::ios::failure)
{
	// This function should never be called (as per getCaps() response.)
	assert(false);
}

ImageConverterPtr PaletteGraphics::openImage(int index)
	throw ()
{
	// This function should never be called (as getImageCount() returns 0.)
	assert(false);
	return ImageConverterPtr();
}

} // namespace gamegraphics
} // namespace camoto
