/**
 * @file   tls-ccomic.hpp
 * @brief  Captain Comic tileset
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

#ifndef _CAMOTO_TLS_CCOMIC_HPP_
#define _CAMOTO_TLS_CCOMIC_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

class CComicTilesetType: virtual public TilesetType {

	public:
		CComicTilesetType()
			throw ();

		virtual ~CComicTilesetType()
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

class CComicSpriteType: virtual public CComicTilesetType {

	public:
		virtual std::string getCode() const
			throw ();

		virtual std::string getFriendlyName() const
			throw ();

		virtual Certainty isInstance(iostream_sptr fsGraphics) const
			throw (std::ios::failure);

		virtual TilesetPtr create(iostream_sptr psGraphics, FN_TRUNCATE fnTruncate,
			SuppData& suppData) const
			throw (std::ios::failure);

		virtual TilesetPtr open(iostream_sptr fsGraphics, FN_TRUNCATE fnTruncate,
			SuppData& suppData) const
			throw (std::ios::failure);

};

class CComicTileset: virtual public FATTileset {

	public:
		CComicTileset(iostream_sptr data, FN_TRUNCATE fnTruncate, uint8_t numPlanes)
			throw (std::ios::failure);

		virtual ~CComicTileset()
			throw ();

		virtual int getCaps()
			throw ();

		void resize(EntryPtr& id, size_t newSize)
			throw (std::ios::failure);

		virtual void getTilesetDimensions(unsigned int *width, unsigned int *height)
			throw ();

		virtual unsigned int getLayoutWidth()
			throw ();

		// FATTileset

		virtual ImagePtr createImageInstance(const EntryPtr& id,
			iostream_sptr content, FN_TRUNCATE fnTruncate)
			throw (std::ios::failure);

	protected:
		int numPlanes;

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TLS_CCOMIC_HPP_
