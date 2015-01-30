/**
 * @file  tls-zone66.hpp
 * @brief Zone 66 tileset handler.
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

#ifndef _CAMOTO_TLS_ZONE66_HPP_
#define _CAMOTO_TLS_ZONE66_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include "pal-vga-raw.hpp"
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

/// Tileset file handler for Zone 66 tilesets.
class Zone66TilesetType: virtual public TilesetType
{
	public:
		Zone66TilesetType();
		virtual ~Zone66TilesetType();

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

/// Tileset handler for Zone 66 tilesets.
class Zone66Tileset: virtual public FATTileset
{
	public:
		Zone66Tileset(stream::inout_sptr data,
			PaletteTablePtr pal);
		virtual ~Zone66Tileset();

		virtual int getCaps();
		virtual ImagePtr createImageInstance(const EntryPtr& id,
			stream::inout_sptr content);
		virtual PaletteTablePtr getPalette();
		virtual void setPalette(PaletteTablePtr newPalette);
		virtual void updateFileOffset(const FATEntry *pid, stream::len offDelta);
		virtual FATEntry *preInsertFile(const FATEntry *idBeforeThis,
			FATEntry *pNewEntry);
		virtual void postInsertFile(FATEntry *pNewEntry);
		virtual void postRemoveFile(const FATEntry *pid);

	protected:
		PaletteTablePtr pal;

	private:
		/// Update the number of tiles in the tileset
		void updateFileCount(uint32_t newCount);
};

} // namespace gamearchive
} // namespace camoto

#endif // _CAMOTO_TLS_ZONE66_HPP_
