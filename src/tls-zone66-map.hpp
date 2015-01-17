/**
 * @file   tls-zone66-map.hpp
 * @brief  Zone 66 map tileset
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

#ifndef _CAMOTO_TLS_ZONE66_MAP_HPP_
#define _CAMOTO_TLS_ZONE66_MAP_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include <camoto/gamegraphics/palettetable.hpp>
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

/// File handler for Zone 66 map tileset files.
class Zone66MapTilesetType: virtual public TilesetType
{
	public:
		Zone66MapTilesetType();
		virtual ~Zone66MapTilesetType();

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

/// Handler for Zone 66 map tilesets.
class Zone66MapTileset: virtual public FATTileset
{
	public:
		Zone66MapTileset(stream::inout_sptr data, PaletteTablePtr pal);
		virtual ~Zone66MapTileset();

		virtual int getCaps();
		void resize(EntryPtr& id, stream::len newSize);
		virtual void getTilesetDimensions(unsigned int *width, unsigned int *height);
		virtual unsigned int getLayoutWidth();
		virtual PaletteTablePtr getPalette();

		// FATTileset
		virtual ImagePtr createImageInstance(const EntryPtr& id,
			stream::inout_sptr content);

	protected:
		unsigned int numPlanes;
		PaletteTablePtr pal;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TLS_ZONE66_MAP_HPP_
