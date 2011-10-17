/**
 * @file   tls-bash.hpp
 * @brief  Monster Bash tileset handler
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

#ifndef _CAMOTO_TLS_MONSTERBASH_HPP_
#define _CAMOTO_TLS_MONSTERBASH_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

class MonsterBashBackgroundTilesetType: virtual public TilesetType {

	public:

		MonsterBashBackgroundTilesetType()
			throw ();

		virtual ~MonsterBashBackgroundTilesetType()
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

class MonsterBashForegroundTilesetType: virtual public TilesetType {

	public:

		MonsterBashForegroundTilesetType()
			throw ();

		virtual ~MonsterBashForegroundTilesetType()
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

class MonsterBashTileset: virtual public FATTileset {

	public:
		MonsterBashTileset(stream::inout_sptr data, uint8_t numPlanes)
			throw (stream::error);

		virtual ~MonsterBashTileset()
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

		virtual FATEntry *preInsertFile(const FATEntry *idBeforeThis,
			FATEntry *pNewEntry)
			throw (stream::error);

	protected:
		int numPlanes; ///< Number of colour planes in each image
		int lenTile;   ///< Length of each image, in bytes

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TLS_MONSTERBASH_HPP_
