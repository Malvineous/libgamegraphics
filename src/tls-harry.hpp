/**
 * @file   tls-harry.hpp
 * @brief  Halloween Harry tileset handler.
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

#ifndef _CAMOTO_TLS_HARRY_HPP_
#define _CAMOTO_TLS_HARRY_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include "pal-vga-raw.hpp"
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

class HarryTilesetType: virtual public TilesetType {

	public:

		HarryTilesetType()
			throw ();

		virtual ~HarryTilesetType()
			throw ();

		virtual std::string getCode() const
			throw ();

		virtual std::string getFriendlyName() const
			throw ();

		virtual std::vector<std::string> getFileExtensions() const
			throw ();

		virtual std::vector<std::string> getGameList() const
			throw ();

		virtual Certainty isInstance(stream::input_sptr fsTileset) const
			throw (stream::error);

		virtual TilesetPtr create(stream::inout_sptr psTileset,
			SuppData& suppData) const
			throw (stream::error);

		virtual TilesetPtr open(stream::inout_sptr fsTileset,
			SuppData& suppData) const
			throw (stream::error);

		virtual SuppFilenames getRequiredSupps(const std::string& filenameTileset) const
			throw ();

};

class HarryTileset: virtual public FATTileset {

	public:

		HarryTileset(stream::inout_sptr data,
			PaletteTablePtr pal)
			throw (stream::error);

		virtual ~HarryTileset()
			throw ();

		virtual int getCaps()
			throw ();

		virtual ImagePtr createImageInstance(const EntryPtr& id,
			stream::inout_sptr content)
			throw (stream::error);

		PaletteTablePtr getPalette()
			throw ();

		void setPalette(PaletteTablePtr newPalette)
			throw (stream::error);

	protected:

		PaletteTablePtr pal;

};

} // namespace gamearchive
} // namespace camoto

#endif // _CAMOTO_TLS_HARRY_HPP_
