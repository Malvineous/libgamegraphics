/**
 * @file   tls-catacomb.hpp
 * @brief  Catacomb/Catacomb II CGA and EGA tilesets
 *
 * Copyright (C) 2010-2013 Adam Nielsen <malvineous@shikadi.net>
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

#ifndef _CAMOTO_TLS_CATACOMB_HPP_
#define _CAMOTO_TLS_CATACOMB_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

/// Flag for tileset image type.
enum CatacombImageType {
	CAT_CGA = 0,
	CAT_EGA = 1
};

/// Common code for Catacomb tilesets.
class CatacombTilesetType: virtual public TilesetType
{
	public:
		CatacombTilesetType();

		virtual ~CatacombTilesetType();

		virtual std::vector<std::string> getFileExtensions() const;

		virtual std::vector<std::string> getGameList() const;

		virtual SuppFilenames getRequiredSupps(const std::string& filenameGraphics) const;
};

/// CGA-specific code for Catacomb tilesets.
class CatacombCGATilesetType: virtual public CatacombTilesetType
{
	public:
		CatacombCGATilesetType();

		virtual ~CatacombCGATilesetType();

		virtual std::string getCode() const;

		virtual std::string getFriendlyName() const;

		virtual Certainty isInstance(stream::input_sptr fsGraphics) const;

		virtual TilesetPtr create(stream::inout_sptr psGraphics,
			SuppData& suppData) const;

		virtual TilesetPtr open(stream::inout_sptr fsGraphics,
			SuppData& suppData) const;
};

/// EGA-specific code for Catacomb tilesets.
class CatacombEGATilesetType: virtual public CatacombTilesetType
{
	public:
		CatacombEGATilesetType();

		virtual ~CatacombEGATilesetType();

		virtual std::string getCode() const;

		virtual std::string getFriendlyName() const;

		virtual Certainty isInstance(stream::input_sptr fsGraphics) const;

		virtual TilesetPtr create(stream::inout_sptr psGraphics,
			SuppData& suppData) const;

		virtual TilesetPtr open(stream::inout_sptr fsGraphics,
			SuppData& suppData) const;
};

/// Tileset format handler.
class CatacombTileset: virtual public FATTileset
{
	public:
		CatacombTileset(stream::inout_sptr data, CatacombImageType imageType);

		virtual ~CatacombTileset();

		virtual int getCaps();

		void resize(EntryPtr& id, stream::len newSize);

		virtual void getTilesetDimensions(unsigned int *width, unsigned int *height);

		virtual unsigned int getLayoutWidth();

		// FATTileset

		virtual ImagePtr createImageInstance(const EntryPtr& id,
			stream::inout_sptr content);

	protected:
		CatacombImageType imageType;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TLS_CATACOMB_HPP_
