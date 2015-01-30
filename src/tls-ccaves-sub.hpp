/**
 * @file  tls-ccaves-sub.hpp
 * @brief Crystal Caves sub tileset handler (containing images)
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

#ifndef _CAMOTO_TLS_CCAVES_SUB_HPP_
#define _CAMOTO_TLS_CCAVES_SUB_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

class CCavesSubTilesetType: virtual public TilesetType {

	public:

		CCavesSubTilesetType();

		virtual ~CCavesSubTilesetType();

		virtual std::string getCode() const;

		virtual std::string getFriendlyName() const;

		virtual std::vector<std::string> getFileExtensions() const;

		virtual std::vector<std::string> getGameList() const;

		virtual Certainty isInstance(stream::input_sptr fsGraphics) const;

		virtual TilesetPtr create(stream::inout_sptr psGraphics,
			SuppData& suppData) const;

		virtual TilesetPtr open(stream::inout_sptr fsGraphics,
			SuppData& suppData) const;

		virtual SuppFilenames getRequiredSupps(const std::string& filenameGraphics) const;

};

class CCavesSubTileset: virtual public FATTileset {
	protected:
		uint8_t width, height, numPlanes;

	public:
		CCavesSubTileset(stream::inout_sptr data, uint8_t numPlanes);

		virtual ~CCavesSubTileset();

		virtual int getCaps();

		void resize(EntryPtr& id, stream::len newSize);

		virtual void getTilesetDimensions(unsigned int *width, unsigned int *height);

		virtual void setTilesetDimensions(unsigned int width, unsigned int height);

		virtual unsigned int getLayoutWidth();

		// FATTileset

		virtual ImagePtr createImageInstance(const EntryPtr& id,
			stream::inout_sptr content);

		virtual FATEntry *preInsertFile(const FATEntry *idBeforeThis,
			FATEntry *pNewEntry);

		virtual void postRemoveFile(const FATEntry *pid);

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TLS_CCAVES_SUB_HPP_
