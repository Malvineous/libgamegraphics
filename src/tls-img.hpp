/**
 * @file  tls-img.hpp
 * @brief Tiles stored as a grid in an image file.
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

#ifndef _CAMOTO_TLS_IMG_HPP_
#define _CAMOTO_TLS_IMG_HPP_

#include "basetileset.hpp"
#include "subimage.hpp"

namespace camoto {
namespace gamegraphics {

class Image_TilesetFrom: virtual public Tileset_Base
{
	public:
		Image_TilesetFrom(ImagePtr img, unsigned int tileWidth,
			unsigned int tileHeight, unsigned int tilesWide, unsigned int tilesHigh);
		virtual ~Image_TilesetFrom();

		virtual int getCaps();
		virtual const VC_ENTRYPTR& getItems() const;
		virtual ImagePtr openImage(const EntryPtr& id);
		virtual EntryPtr insert(const EntryPtr& idBeforeThis, int attr);
		virtual void remove(EntryPtr& id);
		virtual void resize(EntryPtr& id, stream::len newSize);
		virtual void flush();
		virtual void getTilesetDimensions(unsigned int *width, unsigned int *height);
		virtual unsigned int getLayoutWidth();
		virtual PaletteTablePtr getPalette();
		virtual void setPalette(PaletteTablePtr newPalette);

	protected:
		ImagePtr img;            ///< Underlying image file
		StdImageDataPtr stdImg;  ///< Raw image data
		StdImageDataPtr stdMask; ///< Raw image mask
		VC_ENTRYPTR items;       ///< List of tiles
		unsigned int tileWidth;  ///< Width of each tile in pixels
		unsigned int tileHeight; ///< Height of each tile in pixels
		unsigned int tilesWide;  ///< Number of tiles horizontally in each .PCX
		unsigned int tilesHigh;  ///< Number of tiles vertically in each .PCX
		bool hasImageChanged;    ///< Do we need to call fromStandard() on flush?
		fn_image_changed fnImageChanged; ///< Callback function

		void imageChanged();
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TLS_IMG_HPP_
