/**
 * @file   tls-zone66.hpp
 * @brief  Zone 66 tileset handler.
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

#ifndef _CAMOTO_TLS_ZONE66_HPP_
#define _CAMOTO_TLS_ZONE66_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include "pal-vga-raw.hpp"
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

class Zone66TilesetType: virtual public TilesetType {

	public:

		Zone66TilesetType()
			throw ();

		virtual ~Zone66TilesetType()
			throw ();

		virtual std::string getCode() const
			throw ();

		virtual std::string getFriendlyName() const
			throw ();

		virtual std::vector<std::string> getFileExtensions() const
			throw ();

		virtual std::vector<std::string> getGameList() const
			throw ();

		virtual Certainty isInstance(iostream_sptr fsTileset) const
			throw (std::ios::failure);

		virtual TilesetPtr create(iostream_sptr psTileset, FN_TRUNCATE fnTruncate,
			MP_SUPPDATA& suppData) const
			throw (std::ios::failure);

		virtual TilesetPtr open(iostream_sptr fsTileset, FN_TRUNCATE fnTruncate,
			MP_SUPPDATA& suppData) const
			throw (std::ios::failure);

		virtual MP_SUPPLIST getRequiredSupps(const std::string& filenameTileset) const
			throw ();

};

class Zone66Tileset: virtual public FATTileset {

	protected:
		PaletteTablePtr pal;

	public:
		Zone66Tileset(iostream_sptr data, FN_TRUNCATE fnTruncate,
			PaletteTablePtr pal)
			throw (std::ios::failure);

		virtual ~Zone66Tileset()
			throw ();

		virtual int getCaps()
			throw ();

		virtual ImagePtr createImageInstance(const EntryPtr& id,
			iostream_sptr content, FN_TRUNCATE fnTruncate)
			throw (std::ios::failure);

		virtual PaletteTablePtr getPalette()
			throw ();

		virtual void setPalette(PaletteTablePtr newPalette)
			throw (std::ios::failure);

		virtual void updateFileOffset(const FATEntry *pid, std::streamsize offDelta)
			throw (std::ios::failure);

		virtual FATEntry *preInsertFile(const FATEntry *idBeforeThis,
			FATEntry *pNewEntry)
			throw (std::ios::failure);

};

} // namespace gamearchive
} // namespace camoto

#endif // _CAMOTO_TLS_ZONE66_HPP_
