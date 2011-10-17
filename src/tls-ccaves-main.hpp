/**
 * @file   tls-ccaves-main.hpp
 * @brief  Crystal Caves top-level tileset handler (containing sub tilesets)
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

#ifndef _CAMOTO_TLS_CCAVES_MAIN_HPP_
#define _CAMOTO_TLS_CCAVES_MAIN_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

class CCavesMainTilesetType: virtual public TilesetType {

	public:

		CCavesMainTilesetType()
			throw ();

		virtual ~CCavesMainTilesetType()
			throw ();

		virtual std::string getCode() const
			throw ();

		virtual std::string getFriendlyName() const
			throw ();

		virtual std::vector<std::string> getFileExtensions() const
			throw ();

		virtual std::vector<std::string> getGameList() const
			throw ();

		virtual Certainty isInstance(stream::inout_sptr fsGraphics) const
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

class CCavesMainTileset: virtual public FATTileset {
	protected:
		unsigned int numPlanes;

	public:
		CCavesMainTileset(stream::inout_sptr data,
			unsigned int numPlanes)
			throw (stream::error);

		virtual ~CCavesMainTileset()
			throw ();

		virtual int getCaps()
			throw ();

		// FATTileset

		virtual TilesetPtr createTilesetInstance(const EntryPtr& id,
			stream::inout_sptr content)
			throw (stream::error);

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TLS_CCAVES_MAIN_HPP_
