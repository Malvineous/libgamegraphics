/**
 * @file   tls-ddave.hpp
 * @brief  Dangerous Dave tileset handler.
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

#ifndef _CAMOTO_TLS_DDAVE_HPP_
#define _CAMOTO_TLS_DDAVE_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include <camoto/gamegraphics/palettetable.hpp>
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

class DDaveTilesetType: virtual public TilesetType {

	public:
		DDaveTilesetType()
			throw ();

		virtual ~DDaveTilesetType()
			throw ();

		virtual std::vector<std::string> getFileExtensions() const
			throw ();

		virtual std::vector<std::string> getGameList() const
			throw ();

		virtual Certainty isInstance(stream::inout_sptr fsTileset) const
			throw (stream::error);

		virtual SuppFilenames getRequiredSupps(const std::string& filenameTileset) const
			throw ();

	protected:
		/// Given the size of the first tile (in bytes), is this a valid instance?
		virtual bool isInstance(int firstTileSize) const
			throw () = 0;

};

class DDaveCGATilesetType: virtual public DDaveTilesetType {

	public:

		virtual std::string getCode() const
			throw ();

		virtual std::string getFriendlyName() const
			throw ();

		virtual TilesetPtr create(stream::inout_sptr psTileset,
			SuppData& suppData) const
			throw (stream::error);

		virtual TilesetPtr open(stream::inout_sptr fsTileset,
			SuppData& suppData) const
			throw (stream::error);

	protected:
		virtual bool isInstance(int firstTileSize) const
			throw ();

};

class DDaveEGATilesetType: virtual public DDaveTilesetType {

	public:

		virtual std::string getCode() const
			throw ();

		virtual std::string getFriendlyName() const
			throw ();

		virtual TilesetPtr create(stream::inout_sptr psTileset,
			SuppData& suppData) const
			throw (stream::error);

		virtual TilesetPtr open(stream::inout_sptr fsTileset,
			SuppData& suppData) const
			throw (stream::error);

	protected:
		virtual bool isInstance(int firstTileSize) const
			throw ();

};

class DDaveVGATilesetType: virtual public DDaveTilesetType {

	public:

		virtual std::string getCode() const
			throw ();

		virtual std::string getFriendlyName() const
			throw ();

		virtual TilesetPtr create(stream::inout_sptr psTileset,
			SuppData& suppData) const
			throw (stream::error);

		virtual TilesetPtr open(stream::inout_sptr fsTileset,
			SuppData& suppData) const
			throw (stream::error);

		// Extra one to add palette as supp
		virtual SuppFilenames getRequiredSupps(const std::string& filenameTileset) const
			throw ();

	protected:
		virtual bool isInstance(int firstTileSize) const
			throw ();

};


class DDaveTileset: virtual public FATTileset {

	public:
		enum ImageType {CGA, EGA, VGA};

		DDaveTileset(stream::inout_sptr data, ImageType imgType,
			PaletteTablePtr pal)
			throw (stream::error);

		virtual ~DDaveTileset()
			throw ();

		virtual int getCaps()
			throw ();

		virtual ImagePtr createImageInstance(const EntryPtr& id,
			stream::inout_sptr content)
			throw (stream::error);

		virtual PaletteTablePtr getPalette()
			throw ();

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
		ImageType imgType;
		PaletteTablePtr pal;

		/// Update the number of tiles in the tileset
		void updateFileCount(uint32_t newCount)
			throw (std::ios_base::failure);

};

} // namespace gamearchive
} // namespace camoto

#endif // _CAMOTO_TLS_DDAVE_HPP_
