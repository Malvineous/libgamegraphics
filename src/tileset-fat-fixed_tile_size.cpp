/**
 * @file  tileset-fat-fixed_tile_size.cpp
 * @brief Partial implementation of Tileset for those with fixed-size tiles.
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
#include <camoto/util.hpp> // createString
#include "tileset-fat-fixed_tile_size.hpp"

namespace camoto {
namespace gamegraphics {

Tileset_FAT_FixedTileSize::Tileset_FAT_FixedTileSize(stream::len lenTile)
	:	lenTile(lenTile)
{
}

Tileset_FAT_FixedTileSize::~Tileset_FAT_FixedTileSize()
{
}

void Tileset_FAT_FixedTileSize::resize(const FileHandle& id, stream::len newStoredSize,
	stream::len newRealSize)
{
	if (newStoredSize != this->lenTile) {
		throw stream::error(createString("Tiles in this tileset format are a "
			"fixed size of " << this->lenTile << " bytes, so this one cannot be "
			"resized to " << newStoredSize << " bytes."));
	}
	// No need to do the resize if we are here, because we should already be the
	// same size!
	auto pFAT = FATEntry::cast(id);
	assert(pFAT->storedSize == newStoredSize);
	return;
}

void Tileset_FAT_FixedTileSize::preInsertFile(const FATEntry *idBeforeThis,
	FATEntry *pNewEntry)
{
	if (pNewEntry->storedSize != this->lenTile) {
		throw stream::error(createString("Tiles in this tileset format are a "
			"fixed size of " << this->lenTile << " bytes, so this one cannot be "
			"added as it is " << pNewEntry->storedSize << " bytes."));
	}
	return;
}

void Tileset_FAT_FixedTileSize::changeTileSize(stream::len newLenTile)
{
	this->lenTile = newLenTile;
	return;
}

} // namespace gamearchive
} // namespace camoto
