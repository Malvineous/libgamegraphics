/**
 * @file   img-magick.hpp
 * @brief  Image specialisation for ImageMagick images.
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

#ifndef _CAMOTO_IMG_MAGICK_HPP_
#define _CAMOTO_IMG_MAGICK_HPP_

#include <Magick++.h>
#include <camoto/gamegraphics/tileset.hpp>
#include <camoto/gamegraphics/imagetype.hpp>

namespace camoto {
namespace gamegraphics {

/// ImageMagick Image implementation.
class MagickImage: virtual public Image {

	public:
		/// Constructor
		/**
		 * Create a subimage from the supplied ImageMagick image.
		 *
		 * @param parent
		 *   Tileset to hold a reference to, to make sure img stays valid.
		 *
		 * @param img
		 *   ImageMagick image.
		 *
		 * @param x
		 *   X-coordinate of pixel in top-left corner.
		 *
		 * @param y
		 *   Y-coordinate of pixel in top-left corner.
		 *
		 * @param width
		 *   Width of subimage, in pixels.
		 *
		 * @param height
		 *   Height of subimage, in pixels.
		 */
		MagickImage(TilesetPtr parent, Magick::Image& img, int x, int y, int width,
			int height)
			throw (std::ios::failure);

		virtual ~MagickImage()
			throw ();

		virtual int getCaps()
			throw ();

		virtual void getDimensions(unsigned int *width, unsigned int *height)
			throw ();

		virtual void setDimensions(unsigned int width, unsigned int height)
			throw (std::ios::failure);

		virtual StdImageDataPtr toStandard()
			throw (std::ios::failure);

		virtual StdImageDataPtr toStandardMask()
			throw ();

		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask)
			throw (std::ios::failure);

		virtual PaletteTablePtr getPalette()
			throw (std::ios::failure);

		void setPalette(PaletteTablePtr newPalette)
			throw (std::ios::failure);

	protected:
		TilesetPtr parent; ///< For reference counting to keep img valid
		Magick::Image& img;
		int x;
		int y;
		int width;
		int height;

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_MAGICK_HPP_
