/**
 * @file   tls-ccaves-sub.hpp
 * @brief  Crystal Caves sub tileset handler (containing images)
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

#ifndef _CAMOTO_TLS_CCAVES_SUB_HPP_
#define _CAMOTO_TLS_CCAVES_SUB_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

class CCavesSubTilesetType: virtual public TilesetType {

	public:

		CCavesSubTilesetType()
			throw ();

		virtual ~CCavesSubTilesetType()
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

class CCavesSubTileset: virtual public FATTileset {
	protected:
		uint8_t width, height, numPlanes;

	public:
		CCavesSubTileset(stream::inout_sptr data, uint8_t numPlanes)
			throw (stream::error);

		virtual ~CCavesSubTileset()
			throw ();

		virtual int getCaps()
			throw ();

		void resize(EntryPtr& id, size_t newSize)
			throw (stream::error);

		virtual void getTilesetDimensions(unsigned int *width, unsigned int *height)
			throw ();

		virtual void setTilesetDimensions(unsigned int width, unsigned int height)
			throw (stream::error);

		virtual unsigned int getLayoutWidth()
			throw ();

		// FATTileset

		virtual ImagePtr createImageInstance(const EntryPtr& id,
			stream::inout_sptr content)
			throw (stream::error);

		virtual FATEntry *preInsertFile(const FATEntry *idBeforeThis,
			FATEntry *pNewEntry)
			throw (stream::error);

		virtual void postRemoveFile(const FATEntry *pid)
			throw (stream::error);

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TLS_CCAVES_SUB_HPP_
