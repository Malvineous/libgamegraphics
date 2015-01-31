/**
 * @file  tls-ddave.hpp
 * @brief Dangerous Dave tileset handler.
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

#ifndef _CAMOTO_TLS_DDAVE_HPP_
#define _CAMOTO_TLS_DDAVE_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include <camoto/gamegraphics/palettetable.hpp>
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

class TilesetType_DDave: virtual public TilesetType {

	public:
		TilesetType_DDave();

		virtual ~TilesetType_DDave();

		virtual std::vector<std::string> getFileExtensions() const;

		virtual std::vector<std::string> getGameList() const;

		virtual Certainty isInstance(stream::input_sptr fsTileset) const;

		virtual SuppFilenames getRequiredSupps(const std::string& filenameTileset) const;

	protected:
		/// Given the size of the first tile (in bytes), is this a valid instance?
		virtual bool isInstance(int firstTileSize) const = 0;

};

class TilesetType_DDaveCGA: virtual public TilesetType_DDave {

	public:

		virtual std::string getCode() const;

		virtual std::string getFriendlyName() const;

		virtual TilesetPtr create(stream::inout_sptr psTileset,
			SuppData& suppData) const;

		virtual TilesetPtr open(stream::inout_sptr fsTileset,
			SuppData& suppData) const;

	protected:
		virtual bool isInstance(int firstTileSize) const;

};

class TilesetType_DDaveEGA: virtual public TilesetType_DDave {

	public:

		virtual std::string getCode() const;

		virtual std::string getFriendlyName() const;

		virtual TilesetPtr create(stream::inout_sptr psTileset,
			SuppData& suppData) const;

		virtual TilesetPtr open(stream::inout_sptr fsTileset,
			SuppData& suppData) const;

	protected:
		virtual bool isInstance(int firstTileSize) const;

};

class TilesetType_DDaveVGA: virtual public TilesetType_DDave {

	public:

		virtual std::string getCode() const;

		virtual std::string getFriendlyName() const;

		virtual TilesetPtr create(stream::inout_sptr psTileset,
			SuppData& suppData) const;

		virtual TilesetPtr open(stream::inout_sptr fsTileset,
			SuppData& suppData) const;

		// Extra one to add palette as supp
		virtual SuppFilenames getRequiredSupps(const std::string& filenameTileset) const;

	protected:
		virtual bool isInstance(int firstTileSize) const;

};


class Tileset_DDave: virtual public Tileset_FAT {

	public:
		enum ImageType {CGA, EGA, VGA};

		Tileset_DDave(stream::inout_sptr data, ImageType imgType,
			PaletteTablePtr pal);

		virtual ~Tileset_DDave();

		virtual int getCaps();

		virtual ImagePtr createImageInstance(const EntryPtr& id,
			stream::inout_sptr content);

		virtual PaletteTablePtr getPalette();

		virtual void updateFileOffset(const FATEntry *pid, stream::len offDelta);

		virtual FATEntry *preInsertFile(const FATEntry *idBeforeThis,
			FATEntry *pNewEntry);

		virtual void postInsertFile(FATEntry *pNewEntry);

		virtual void postRemoveFile(const FATEntry *pid);

	private:
		ImageType imgType;
		PaletteTablePtr pal;

		/// Update the number of tiles in the tileset
		void updateFileCount(uint32_t newCount);

};

} // namespace gamearchive
} // namespace camoto

#endif // _CAMOTO_TLS_DDAVE_HPP_
