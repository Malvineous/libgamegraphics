/**
 * @file  tls-jill.hpp
 * @brief Jill of the Jungle tileset
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

#ifndef _CAMOTO_TLS_JILL_HPP_
#define _CAMOTO_TLS_JILL_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include "tileset-fat.hpp"
#include "img-vga.hpp"

namespace camoto {
namespace gamegraphics {

class TilesetType_Jill: virtual public TilesetType
{
	public:
		TilesetType_Jill();
		virtual ~TilesetType_Jill();

		virtual std::string getCode() const;
		virtual std::string getFriendlyName() const;
		virtual std::vector<std::string> getFileExtensions() const;
		virtual std::vector<std::string> getGameList() const;
		virtual Certainty isInstance(stream::input_sptr fsGraphics) const;
		virtual TilesetPtr create(stream::inout_sptr psGraphics,
			SuppData& suppData) const;
		virtual TilesetPtr open(stream::inout_sptr fsGraphics,
			SuppData& suppData) const;
		virtual SuppFilenames getRequiredSupps(const std::string& filenameTileset)
			const;
};

class Tileset_Jill: virtual public Tileset_FAT
{
	public:
		Tileset_Jill(stream::inout_sptr data, PaletteTablePtr pal);
		virtual ~Tileset_Jill();

		virtual int getCaps();
		virtual PaletteTablePtr getPalette();
		// Tileset_FAT
		virtual TilesetPtr createTilesetInstance(const EntryPtr& id,
			stream::inout_sptr content);
		virtual void updateFileOffset(const FATEntry *pid, stream::len offDelta);
		virtual void updateFileSize(const FATEntry *pid, stream::len sizeDelta);

	protected:
		PaletteTablePtr pal; ///< Palette for entire tileset (optional)
};

class Tileset_JillSub: virtual public Tileset_FAT
{
	public:
		Tileset_JillSub(stream::inout_sptr data);
		virtual ~Tileset_JillSub();

		virtual int getCaps();
		virtual unsigned int getLayoutWidth();
		// Tileset_FAT
		virtual ImagePtr createImageInstance(const EntryPtr& id,
			stream::inout_sptr content);
		virtual FATEntry *preInsertFile(const FATEntry *idBeforeThis,
			FATEntry *pNewEntry);
		virtual void postRemoveFile(const FATEntry *pid);

	protected:
		StdImageDataPtr colourMap;
};

/// Image implementation for a Jill of the Jungle tile.
class Image_Jill: virtual public Image_VGA
{
	public:
		/// Constructor
		/**
		 * Create an image from the supplied stream.
		 *
		 * @param data
		 *   Image data, including width/height header.
		 *
		 * @param colourMap
		 *   Colour mapping table (not a palette) from the parent tileset.
		 */
		Image_Jill(stream::inout_sptr data, const StdImageDataPtr colourMap);
		virtual ~Image_Jill();

		virtual int getCaps();
		virtual void getDimensions(unsigned int *width, unsigned int *height);
		virtual void setDimensions(unsigned int width, unsigned int height);
		virtual StdImageDataPtr toStandard();
		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask);

	protected:
		uint8_t width;
		uint8_t height;
		const StdImageDataPtr colourMap;
};


} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TLS_JILL_HPP_
