/**
 * @file   graphics.cpp
 * @brief  Declaration of top-level Graphics class, for accessing files
 *         storing game images.
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

#include <cassert>
#include <camoto/gamegraphics/graphics.hpp>

namespace camoto {
namespace gamegraphics {

Graphics::Graphics()
	throw ()
{
}

Graphics::~Graphics()
	throw ()
{
}

int Graphics::getTilesetCount()
	throw ()
{
	return 0;
}

GraphicsPtr Graphics::getTileset(int index)
	throw ()
{
	// Tileset out of range
	assert(false);
	return GraphicsPtr();
}

/*
GraphicsPtr Graphics::insertTileset(int insertBefore)
	throw ()
{
	// Caller didn't check getCaps()
	assert(false);
	return GraphicsPtr();
}
*/

VC_METADATA_ITEMS Graphics::getMetadataList() const
	throw ()
{
	return VC_METADATA_ITEMS();
}

std::string Graphics::getMetadata(E_METADATA item) const
	throw (std::ios::failure)
{
	// This should never be called because getMetadataList() returned an empty
	// list.
	assert(false);
	throw std::ios::failure("unsupported metadata item");
}

void Graphics::setMetadata(E_METADATA item, const std::string& value)
	throw (std::ios::failure)
{
	// This should never be called because getMetadataList() returned an empty
	// list.
	assert(false);
	throw std::ios::failure("unsupported metadata item");
}

} // namespace gamegraphics
} // namespace camoto
