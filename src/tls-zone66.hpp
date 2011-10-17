/**
 * @file   tls-zone66.hpp
 * @brief  Zone 66 tileset handler.
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

		virtual Certainty isInstance(stream::inout_sptr fsTileset) const
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

class Zone66Tileset: virtual public FATTileset {

	protected:
		PaletteTablePtr pal;

	public:
		Zone66Tileset(stream::inout_sptr data,
			PaletteTablePtr pal)
			throw (stream::error);

		virtual ~Zone66Tileset()
			throw ();

		virtual int getCaps()
			throw ();

		virtual ImagePtr createImageInstance(const EntryPtr& id,
			stream::inout_sptr content)
			throw (stream::error);

		virtual PaletteTablePtr getPalette()
			throw ();

		virtual void setPalette(PaletteTablePtr newPalette)
			throw (stream::error);

		virtual void updateFileOffset(const FATEntry *pid, stream::len offDelta)
			throw (stream::error);

		virtual FATEntry *preInsertFile(const FATEntry *idBeforeThis,
			FATEntry *pNewEntry)
			throw (stream::error);

		virtual void postInsertFile(FATEntry *pNewEntry)
			throw (stream::error);

		virtual void postRemoveFile(const FATEntry *pid)
			throw (stream::error);

	private:
		/// Update the number of tiles in the tileset
		void updateFileCount(uint32_t newCount)
			throw (std::ios_base::failure);

};

} // namespace gamearchive
} // namespace camoto

#endif // _CAMOTO_TLS_ZONE66_HPP_
