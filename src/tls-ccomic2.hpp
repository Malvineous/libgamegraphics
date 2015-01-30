/**
 * @file  tls-ccomic2.hpp
 * @brief Captain Comic II tileset
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

#ifndef _CAMOTO_TLS_CCOMIC2_HPP_
#define _CAMOTO_TLS_CCOMIC2_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

class CComic2TilesetType: virtual public TilesetType
{
	public:
		CComic2TilesetType();
		virtual ~CComic2TilesetType();

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

class CComic2SpriteType: virtual public CComic2TilesetType
{
	public:
		virtual std::string getCode() const;
		virtual std::string getFriendlyName() const;
		virtual Certainty isInstance(stream::input_sptr fsGraphics) const;
		virtual TilesetPtr create(stream::inout_sptr psGraphics,
			SuppData& suppData) const;
		virtual TilesetPtr open(stream::inout_sptr fsGraphics,
			SuppData& suppData) const;
};

class CComic2Tileset: virtual public FATTileset
{
	public:
		CComic2Tileset(stream::inout_sptr data, uint8_t numPlanes);
		virtual ~CComic2Tileset();

		virtual int getCaps();
		void resize(EntryPtr& id, stream::len newSize);
		virtual void getTilesetDimensions(unsigned int *width, unsigned int *height);
		virtual unsigned int getLayoutWidth();

		// FATTileset
		virtual ImagePtr createImageInstance(const EntryPtr& id,
			stream::inout_sptr content);

	protected:
		unsigned int numPlanes;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TLS_CCOMIC2_HPP_
