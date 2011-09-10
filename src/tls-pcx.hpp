/**
 * @file   tls-pcx.hpp
 * @brief  Tiles stored in a PCX image
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

#ifndef _CAMOTO_TLS_PCX_HPP_
#define _CAMOTO_TLS_PCX_HPP_

#include <boost/enable_shared_from_this.hpp>
#include <Magick++.h>
#include <camoto/gamegraphics/tileset.hpp>

namespace camoto {
namespace gamegraphics {

class PCXTileset: virtual public Tileset,
	public boost::enable_shared_from_this<PCXTileset>
{

	public:
		struct PCXEntry: public Entry {
			int index; ///< Zero-based index of tile
		};

		PCXTileset(iostream_sptr data, FN_TRUNCATE fnTruncate,
			int tileWidth, int tileHeight, int tilesWide, int tilesHigh)
			throw (std::ios::failure);

		virtual ~PCXTileset()
			throw ();

		virtual int getCaps()
			throw ();

		virtual const VC_ENTRYPTR& getItems() const
			throw ();

		virtual ImagePtr openImage(const EntryPtr& id)
			throw (std::ios::failure);

		virtual EntryPtr insert(const EntryPtr& idBeforeThis, int attr)
			throw (std::ios::failure);

		virtual void remove(EntryPtr& id)
			throw (std::ios::failure);

		void resize(EntryPtr& id, size_t newSize)
			throw (std::ios::failure);

		virtual void flush()
			throw (std::ios::failure);

		virtual void getTilesetDimensions(unsigned int *width, unsigned int *height)
			throw ();

		virtual unsigned int getLayoutWidth()
			throw ();

		virtual PaletteTablePtr getPalette()
			throw ();

		void setPalette(PaletteTablePtr newPalette)
			throw (std::ios::failure);

	protected:
		iostream_sptr data;  ///< Underlying .PCX file
		Magick::Image img;   ///< ImageMagick instance to .PCX
		VC_ENTRYPTR items;   ///< List of tiles
		int tileWidth;       ///< Width of each tile in pixels
		int tileHeight;      ///< Height of each tile in pixels
		int tilesWide;       ///< Number of tiles horizontally in each .PCX
		int tilesHigh;       ///< Number of tiles vertically in each .PCX

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TLS_PCX_HPP_
