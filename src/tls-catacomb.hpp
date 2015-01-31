/**
 * @file  tls-catacomb.hpp
 * @brief Catacomb/Catacomb II CGA and EGA tilesets
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

#ifndef _CAMOTO_TLS_CATACOMB_HPP_
#define _CAMOTO_TLS_CATACOMB_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

/// Flag for tileset image type.
enum ImageType_Catacomb {
	CAT_CGA = 0,
	CAT_EGA = 1
};

/// Common code for Catacomb tilesets.
class TilesetType_Catacomb: virtual public TilesetType
{
	public:
		TilesetType_Catacomb();

		virtual ~TilesetType_Catacomb();

		virtual std::vector<std::string> getFileExtensions() const;

		virtual std::vector<std::string> getGameList() const;

		virtual SuppFilenames getRequiredSupps(const std::string& filenameGraphics) const;
};

/// CGA-specific code for Catacomb tilesets.
class TilesetType_CatacombCGA: virtual public TilesetType_Catacomb
{
	public:
		TilesetType_CatacombCGA();

		virtual ~TilesetType_CatacombCGA();

		virtual std::string getCode() const;

		virtual std::string getFriendlyName() const;

		virtual Certainty isInstance(stream::input_sptr fsGraphics) const;

		virtual TilesetPtr create(stream::inout_sptr psGraphics,
			SuppData& suppData) const;

		virtual TilesetPtr open(stream::inout_sptr fsGraphics,
			SuppData& suppData) const;
};

/// EGA-specific code for Catacomb tilesets.
class TilesetType_CatacombEGA: virtual public TilesetType_Catacomb
{
	public:
		TilesetType_CatacombEGA();

		virtual ~TilesetType_CatacombEGA();

		virtual std::string getCode() const;

		virtual std::string getFriendlyName() const;

		virtual Certainty isInstance(stream::input_sptr fsGraphics) const;

		virtual TilesetPtr create(stream::inout_sptr psGraphics,
			SuppData& suppData) const;

		virtual TilesetPtr open(stream::inout_sptr fsGraphics,
			SuppData& suppData) const;
};

/// Tileset format handler.
class Tileset_Catacomb: virtual public Tileset_FAT
{
	public:
		Tileset_Catacomb(stream::inout_sptr data, ImageType_Catacomb imageType);

		virtual ~Tileset_Catacomb();

		virtual int getCaps();

		void resize(EntryPtr& id, stream::len newSize);

		virtual void getTilesetDimensions(unsigned int *width, unsigned int *height);

		virtual unsigned int getLayoutWidth();

		// Tileset_FAT

		virtual ImagePtr createImageInstance(const EntryPtr& id,
			stream::inout_sptr content);

	protected:
		ImageType_Catacomb imageType;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TLS_CATACOMB_HPP_
