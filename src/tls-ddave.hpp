/**
 * @file   tls-ddave.hpp
 * @brief  Dangerous Dave tileset handler.
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

#ifndef _CAMOTO_TLS_DDAVE_HPP_
#define _CAMOTO_TLS_DDAVE_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include "pal-vga-raw.hpp"
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

class DDaveTilesetType: virtual public TilesetType {

	public:

		DDaveTilesetType()
			throw ();

		virtual ~DDaveTilesetType()
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

class DDaveTileset: virtual public FATTileset {

	public:
		DDaveTileset(iostream_sptr data, FN_TRUNCATE fnTruncate)
			throw (std::ios::failure);

		virtual ~DDaveTileset()
			throw ();

		virtual int getCaps()
			throw ();

		virtual ImagePtr createImageInstance(const EntryPtr& id,
			iostream_sptr content, FN_TRUNCATE fnTruncate)
			throw (std::ios::failure);

		virtual void updateFileOffset(const FATEntry *pid, std::streamsize offDelta)
			throw (std::ios::failure);

		virtual FATEntry *preInsertFile(const FATEntry *idBeforeThis,
			FATEntry *pNewEntry)
			throw (std::ios::failure);

		virtual void postInsertFile(FATEntry *pNewEntry)
			throw (std::ios::failure);

		virtual void postRemoveFile(const FATEntry *pid)
			throw (std::ios::failure);

	private:
		/// Update the number of tiles in the tileset
		void updateFileCount(uint32_t newCount)
			throw (std::ios_base::failure);

};

} // namespace gamearchive
} // namespace camoto

#endif // _CAMOTO_TLS_DDAVE_HPP_
