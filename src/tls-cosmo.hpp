/**
 * @file   tls-cosmo.hpp
 * @brief  Cosmo's Cosmic Adventures tileset
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

#ifndef _CAMOTO_TLS_COSMO_HPP_
#define _CAMOTO_TLS_COSMO_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

class CosmoTilesetType: virtual public TilesetType {

	public:
		CosmoTilesetType()
			throw ();

		virtual ~CosmoTilesetType()
			throw ();

		virtual std::string getCode() const
			throw ();

		virtual std::string getFriendlyName() const
			throw ();

		virtual std::vector<std::string> getFileExtensions() const
			throw ();

		virtual std::vector<std::string> getGameList() const
			throw ();

		virtual Certainty isInstance(stream::input_sptr fsGraphics) const
			throw (stream::error);

		virtual TilesetPtr create(stream::inout_sptr psGraphics,
			SuppData& suppData) const
			throw (stream::error);

		virtual TilesetPtr open(stream::inout_sptr fsGraphics,
			SuppData& suppData) const
			throw (stream::error);

		virtual SuppFilenames getRequiredSupps(const std::string& filenameGraphics) const
			throw ();

};

class CosmoMaskedTilesetType: virtual public CosmoTilesetType {

	public:
		virtual std::string getCode() const
			throw ();

		virtual std::string getFriendlyName() const
			throw ();

		virtual Certainty isInstance(stream::input_sptr fsGraphics) const
			throw (stream::error);

		virtual TilesetPtr create(stream::inout_sptr psGraphics,
			SuppData& suppData) const
			throw (stream::error);

		virtual TilesetPtr open(stream::inout_sptr fsGraphics,
			SuppData& suppData) const
			throw (stream::error);

};

class CosmoTileset: virtual public FATTileset {

	public:
		CosmoTileset(stream::inout_sptr data, uint8_t numPlanes)
			throw (stream::error);

		virtual ~CosmoTileset()
			throw ();

		virtual int getCaps()
			throw ();

		void resize(EntryPtr& id, size_t newSize)
			throw (stream::error);

		virtual void getTilesetDimensions(unsigned int *width, unsigned int *height)
			throw ();

		virtual unsigned int getLayoutWidth()
			throw ();

		// FATTileset

		virtual ImagePtr createImageInstance(const EntryPtr& id,
			stream::inout_sptr content)
			throw (stream::error);

	protected:
		unsigned int numPlanes;

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TLS_COSMO_HPP_
