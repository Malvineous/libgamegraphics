/**
 * @file   tls-harry-ico.hpp
 * @brief  Halloween Harry .ICO tileset handler.
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

#ifndef _CAMOTO_TLS_HARRY_ICO_HPP_
#define _CAMOTO_TLS_HARRY_ICO_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

class HarryICOTilesetType: virtual public TilesetType
{
	public:
		HarryICOTilesetType();

		virtual ~HarryICOTilesetType();

		virtual std::string getCode() const;

		virtual std::string getFriendlyName() const;

		virtual std::vector<std::string> getFileExtensions() const;

		virtual std::vector<std::string> getGameList() const;

		virtual Certainty isInstance(stream::input_sptr fsTileset) const;

		virtual TilesetPtr create(stream::inout_sptr psTileset,
			SuppData& suppData) const;

		virtual TilesetPtr open(stream::inout_sptr fsTileset,
			SuppData& suppData) const;

		virtual SuppFilenames getRequiredSupps(const std::string& filenameTileset) const;
};

class HarryICOTileset: virtual public FATTileset
{
	public:
		HarryICOTileset(stream::inout_sptr data, PaletteTablePtr pal);

		virtual ~HarryICOTileset();

		virtual int getCaps();

		virtual unsigned int getLayoutWidth();

		PaletteTablePtr getPalette();

		virtual ImagePtr createImageInstance(const EntryPtr& id,
			stream::inout_sptr content);

	protected:
		PaletteTablePtr pal;
};

} // namespace gamearchive
} // namespace camoto

#endif // _CAMOTO_TLS_HARRY_ICO_HPP_
