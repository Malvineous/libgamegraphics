/**
 * @file   tls-jill.hpp
 * @brief  Jill of the Jungle tileset
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

#ifndef _CAMOTO_TLS_JILL_HPP_
#define _CAMOTO_TLS_JILL_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include "tileset-fat.hpp"
#include "img-vga.hpp"

namespace camoto {
namespace gamegraphics {

class JillTilesetType: virtual public TilesetType {

	public:
		JillTilesetType()
			throw ();

		virtual ~JillTilesetType()
			throw ();

		virtual std::string getCode() const
			throw ();

		virtual std::string getFriendlyName() const
			throw ();

		virtual std::vector<std::string> getFileExtensions() const
			throw ();

		virtual std::vector<std::string> getGameList() const
			throw ();

		virtual Certainty isInstance(stream::input_sptr fsGraphics) const
			throw (stream::error);

		virtual TilesetPtr create(stream::inout_sptr psGraphics,
			SuppData& suppData) const
			throw (stream::error);

		virtual TilesetPtr open(stream::inout_sptr fsGraphics,
			SuppData& suppData) const
			throw (stream::error);

		virtual SuppFilenames getRequiredSupps(const std::string& filenameGraphics) const
			throw ();

};

class JillTileset: virtual public FATTileset {

	public:
		JillTileset(stream::inout_sptr data, PaletteTablePtr pal)
			throw (stream::error);

		virtual ~JillTileset()
			throw ();

		virtual int getCaps()
			throw ();

		virtual PaletteTablePtr getPalette()
			throw ();

		// FATTileset

		virtual TilesetPtr createTilesetInstance(const EntryPtr& id,
			stream::inout_sptr content)
			throw (stream::error);

		virtual void updateFileOffset(const FATEntry *pid, stream::len offDelta)
			throw (stream::error);

		virtual void updateFileSize(const FATEntry *pid, stream::len sizeDelta)
			throw (stream::error);

	protected:
		PaletteTablePtr pal; ///< Palette for entire tileset (optional)

};

class JillTiles: virtual public FATTileset {

	public:
		JillTiles(stream::inout_sptr data)
			throw (stream::error);

		virtual ~JillTiles()
			throw ();

		virtual int getCaps()
			throw ();

		virtual unsigned int getLayoutWidth()
			throw ();

		// FATTileset

		virtual ImagePtr createImageInstance(const EntryPtr& id,
			stream::inout_sptr content)
			throw (stream::error);

		virtual FATEntry *preInsertFile(const FATEntry *idBeforeThis,
			FATEntry *pNewEntry)
			throw (stream::error);

		virtual void postRemoveFile(const FATEntry *pid)
			throw (stream::error);

	protected:
		StdImageDataPtr colourMap;

};

/// Image implementation for a Jill of the Jungle tile.
class JillImage: virtual public VGAImage {

	public:
		/// Constructor
		/**
		 * Create an image from the supplied stream.
		 *
		 * @param data
		 *   Image data, including width/height header.
		 */
		JillImage(stream::inout_sptr data, const StdImageDataPtr colourMap)
			throw ();

		virtual ~JillImage()
			throw ();

		virtual int getCaps()
			throw ();

		virtual void getDimensions(unsigned int *width, unsigned int *height)
			throw ();

		virtual void setDimensions(unsigned int width, unsigned int height)
			throw (stream::error);

		virtual StdImageDataPtr toStandard()
			throw (stream::read_error);

		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask)
			throw ();

	protected:
		uint8_t width;
		uint8_t height;
		const StdImageDataPtr colourMap;

};


} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TLS_JILL_HPP_
