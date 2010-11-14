/**
 * @file   tls-ccaves.hpp
 * @brief  Crystal Caves tileset handler.
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

#ifndef _CAMOTO_TLS_CCAVES_HPP_
#define _CAMOTO_TLS_CCAVES_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

class CCavesTilesetType: virtual public TilesetType {

	public:

		CCavesTilesetType()
			throw ();

		virtual ~CCavesTilesetType()
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
			MP_SUPPDATA& suppData) const
			throw (std::ios::failure);

		virtual TilesetPtr open(iostream_sptr fsGraphics, FN_TRUNCATE fnTruncate,
			MP_SUPPDATA& suppData) const
			throw (std::ios::failure);

		virtual MP_SUPPLIST getRequiredSupps(const std::string& filenameGraphics) const
			throw ();

};

class CCavesTileset: virtual public FATTileset {
	protected:
		unsigned int numPlanes;

	public:
		CCavesTileset(iostream_sptr data, FN_TRUNCATE fnTruncate,
			unsigned int numPlanes)
			throw (std::ios::failure);

		virtual ~CCavesTileset()
			throw ();

		virtual int getCaps()
			throw ();

		// FATTileset

		virtual TilesetPtr createTilesetInstance(const EntryPtr& id,
			iostream_sptr content, FN_TRUNCATE fnTruncate)
			throw (std::ios::failure);

};

class CCavesTiles: virtual public FATTileset {
	protected:
		uint8_t width, height, numPlanes;

	public:
		CCavesTiles(iostream_sptr data, FN_TRUNCATE fnTruncate, uint8_t numPlanes)
			throw (std::ios::failure);

		virtual ~CCavesTiles()
			throw ();

		virtual int getCaps()
			throw ();

		void resize(EntryPtr& id, size_t newSize)
			throw (std::ios::failure);

		virtual void getTilesetDimensions(unsigned int *width, unsigned int *height)
			throw ();

		virtual void setTilesetDimensions(unsigned int width, unsigned int height)
			throw (std::ios::failure);

		virtual unsigned int getLayoutWidth()
			throw ();

		// FATTileset

		virtual ImagePtr createImageInstance(const EntryPtr& id,
			iostream_sptr content, FN_TRUNCATE fnTruncate)
			throw (std::ios::failure);

		virtual FATEntry *preInsertFile(const FATEntry *idBeforeThis,
			FATEntry *pNewEntry)
			throw (std::ios::failure);

		virtual void postRemoveFile(const FATEntry *pid)
			throw (std::ios::failure);

};

} // namespace gamearchive
} // namespace camoto

#endif // _CAMOTO_TLS_CCAVES_HPP_
