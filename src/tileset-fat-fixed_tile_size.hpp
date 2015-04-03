/**
 * @file  tileset-fat-fixed_tile_size.hpp
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

#ifndef _CAMOTO_TILESET_FAT_FIXED_TILESIZE_HPP_
#define _CAMOTO_TILESET_FAT_FIXED_TILESIZE_HPP_

#include <camoto/gamearchive/archive-fat.hpp>
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

/// Partial Tileset implementation where all tiles are the same size.
class Tileset_FAT_FixedTileSize: virtual public Tileset_FAT
{
	public:
		Tileset_FAT_FixedTileSize(stream::len lenTile);
		virtual ~Tileset_FAT_FixedTileSize();

		virtual void resize(FileHandle& id, stream::len newStoredSize,
			stream::len newRealSize);

	protected:
		virtual void preInsertFile(const FATEntry *idBeforeThis,
			FATEntry *pNewEntry);

		/// Change the lenTile value from the constructor.
		/**
		 * This can be used when the dimensions of the whole tileset are changing.
		 * In this case the tiles are all still the same size, but the size of all
		 * of them is being changed.
		 */
		void changeTileSize(stream::len newLenTile);

		stream::len lenTile;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TILESET_FAT_FIXED_TILESIZE_HPP_
