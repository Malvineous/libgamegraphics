/**
 * @file   imageconverter.hpp
 * @brief  ImageConverter class interface, used to convert between various game
 *         formats and a standard image data format.  Actual format conversions
 *         are handled by other classes which implement this interface.
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

#ifndef _CAMOTO_GAMEGRAPHICS_IMAGE_HPP_
#define _CAMOTO_GAMEGRAPHICS_IMAGE_HPP_

#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
//#include <exception>
#include <iostream>
//#include <sstream>
//#include <vector>

#include <camoto/types.hpp>
#include <camoto/gamegraphics/palette.hpp>

namespace camoto {
namespace gamegraphics {

/// Shared pointer to the raw image data
typedef boost::shared_array<uint8_t> StdImageDataPtr;

class ImageConverter;

/// Shared pointer to an ImageConverter.
typedef boost::shared_ptr<ImageConverter> ImageConverterPtr;

/// Primary interface to an image file.
/**
 * This class represents a single image.  Its functions are used to convert
 * between a game's custom format and a standard image format (8bpp linear
 * indexed.)
 *
 * The image being converted could be an 8x8 tile, a 320x200 full-screen
 * graphic or a single frame from an animation.
 *
 * Note that this does not have anything to do with palettes, which still need
 * to be obtained externally to this class.
 *
 * @note Multithreading: Only call one function in this class at a time.  Many
 *       of the functions seek around the underlying stream and thus will break
 *       if two or more functions are executing at the same time.
 */
class ImageConverter {

	public:

		/// Truncate callback
		/**
		 * This function is called with a single unsigned long parameter when
		 * the underlying image file needs to be shrunk or enlarged to the given
		 * size.  It must be set to a valid function before fromStandard() is
		 * called.
		 *
		 * The function signature is:
		 * @code
		 * void fnTruncate(unsigned long newLength);
		 * @endcode
		 *
		 * This example uses boost::bind to package up a call to the Linux
		 * truncate() function (which requires both a filename and size) such that
		 * the filename is supplied in advance and not required when flush() makes
		 * the call.
		 *
		 * @code
		 * ImageConverter *img = ...
		 * img->fnTruncate = boost::bind<void>(truncate, "graphics.dat", _1);
		 * img->fromStandard(...);  // calls truncate("graphics.dat", 123)
		 * @endcode
		 *
		 * Unfortunately since there is no cross-platform method for changing a
		 * file's size via its iostream, this is a necessary evil to avoid
		 * passing the graphics filename around all over the place.
		 */
		FN_TRUNCATE fnTruncate;

		ImageConverter()
			throw ();

		virtual ~ImageConverter()
			throw ();

		/// Convert the image into a standard format.
		/**
		 * This function converts whatever the underlying image format is into
		 * 8bpp indexed data.
		 *
		 * @return A shared pointer to a byte array of image data.
		 */
		virtual StdImageDataPtr toStandard()
			throw () = 0;

		/// Convert the image mask into a standard format.
		/**
		 * This function converts whatever the underlying image format is into
		 * 8bpp data.
		 *
		 * The data layout is identical to the image data, only instead of the
		 * values being colours, the least-significant bit of each byte (0x01)
		 * is used to denote opacity (0 == transparent, 1 == opaque) and the
		 * next bit (0x02) is used to denote hitmapping (0 == background, 1 ==
		 * object)
		 *
		 * @return A shared pointer to a byte array of mask data.
		 */
		virtual StdImageDataPtr toStandardMask()
			throw () = 0;

		/// Get the indexed colour map from the file.
		/**
		 * @return NULL/empty pointer if the format doesn't support custom palettes,
		 *   otherwise a shared pointer to a Palette.
		 */
		virtual PalettePtr getPalette()
			throw ();

		/// Replace the image with new content.
		/**
		 * Take image data in the standard format, convert it to the underlying
		 * custom format and overwrite the old image.
		 *
		 * The mask layout is identical to the image data, only instead of the
		 * values being colours, the least-significant bit of each byte (0x01)
		 * is used to denote opacity (0 == transparent, 1 == opaque) and the
		 * next bit (0x02) is used to denote hitmapping (0 == background, 1 ==
		 * object)
		 *
		 * @param newContent  Image data, in the standard 8bpp indexed format, to
		 *   convert and replace the underlying image with.
		 *
		 * @param newMask  Image data, in the standard 8bpp format, to
		 *   convert and replace the underlying mask with.
		 *
		 * @param newPalette  Palette data, but only if getPalette() returned a
		 *   valid palette, otherwise the image doesn't support custom palettes
		 *   and new palette data should not be supplied.
		 */
		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask, PalettePtr newPalette)
			throw () = 0;

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_GAMEGRAPHICS_IMAGE_HPP_
