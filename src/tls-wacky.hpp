/**
 * @file   tls-wacky.hpp
 * @brief  Wacky Wheels tileset
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

#ifndef _CAMOTO_TLS_WACKY_HPP_
#define _CAMOTO_TLS_WACKY_HPP_

#include <boost/enable_shared_from_this.hpp>
#include <Magick++.h>
#include <camoto/gamegraphics/tilesettype.hpp>

namespace camoto {
namespace gamegraphics {

class WackyTilesetType: virtual public TilesetType {

	public:
		WackyTilesetType()
			throw ();

		virtual ~WackyTilesetType()
			throw ();

		virtual std::string getCode() const
			throw ();

		virtual std::string getFriendlyName() const
			throw ();

		virtual std::vector<std::string> getFileExtensions() const
			throw ();

		virtual std::vector<std::string> getGameList() const
			throw ();

		virtual Certainty isInstance(iostream_sptr fsGraphics) const
			throw (std::ios::failure);

		virtual TilesetPtr create(iostream_sptr psGraphics, FN_TRUNCATE fnTruncate,
			SuppData& suppData) const
			throw (std::ios::failure);

		virtual TilesetPtr open(iostream_sptr fsGraphics, FN_TRUNCATE fnTruncate,
			SuppData& suppData) const
			throw (std::ios::failure);

		virtual SuppFilenames getRequiredSupps(const std::string& filenameGraphics) const
			throw ();

};

class WackyTileset: virtual public Tileset,
	public boost::enable_shared_from_this<WackyTileset>
{

	public:
		struct WackyEntry: public Entry {
			int index; ///< Zero-based index of tile
		};

		WackyTileset(iostream_sptr data, FN_TRUNCATE fnTruncate)
			throw (std::ios::failure);

		virtual ~WackyTileset()
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

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TLS_WACKY_HPP_
