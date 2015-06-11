/**
 * @file  tileset.cpp
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
#include <camoto/gamegraphics/tileset.hpp>

namespace camoto {
namespace gamegraphics {

Tileset::Tileset()
{
}

Tileset::~Tileset()
{
}

std::shared_ptr<Tileset> Tileset::openTileset(const FileHandle& id)
{
	// Caller didn't check FileHandle->fAttr
	assert(false);
	throw stream::error("This tileset has no images"
		" (this is a bug - the caller should have checked the FileHandle's"
		" attributes to detect this).");
}

std::unique_ptr<Image> Tileset::openImage(const FileHandle& id)
{
	// Caller didn't check FileHandle->fAttr
	assert(false);
	throw stream::error("This tileset has no images"
		" (this is a bug - the caller should have checked the FileHandle's"
		" attributes to detect this).");
}

Point Tileset::dimensions() const
{
	// Fail if this function is called when the caps say not to
	assert(this->caps() & Caps::HasDimensions);

	// If we get here the file format said the dimensions are present but
	// forgot to override this function, so fail.
	assert(false);
}

void Tileset::dimensions(const Point& newDimensions)
{
	// Fail if this function is called when the caps say not to
	assert(this->caps() & Caps::SetDimensions);

	// If we get here the file format said the dimensions can be changed but
	// forgot to override this function, so fail.
	assert(false);
}

unsigned int Tileset::layoutWidth() const
{
	return 0;
}

const Tileset::FileHandle Tileset::insert(const FileHandle& idBeforeThis,
	File::Attribute attr)
{
	throw stream::error("This tileset contains a fixed number of images and "
		"cannot have more inserted.");
}

std::shared_ptr<const Palette> Tileset::palette() const
{
	return this->pal;
}

void Tileset::palette(std::shared_ptr<const Palette> newPalette)
{
	assert(this->caps() & Caps::SetPalette);
	this->pal = newPalette;
	return;
}

} // namespace gamegraphics
} // namespace camoto
