/**
 * @file   image.hpp
 * @brief  Image class interface, used to convert between various game
 *         formats and a standard image data format.  Actual format conversions
 *         are handled by other classes which implement this interface.
 *
 * Copyright (C) 2010-2013 Adam Nielsen <malvineous@shikadi.net>
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
#include <iostream>

#include <camoto/stream.hpp>
#include <stdint.h>
#include <camoto/gamegraphics/palettetable.hpp>

namespace camoto {
namespace gamegraphics {

/// Shared pointer to the raw image data
typedef boost::shared_array<uint8_t> StdImageDataPtr;

/// Primary interface to an image file.
/**
 * This class represents a single image.  Its functions are used to convert
 * between a game's custom format and a standard image format (8bpp linear
 * indexed.)
 *
 * The image being converted could be an 8x8 tile, a 320x200 full-screen
 * graphic or a single frame from an animation.
 *
 * @note Multithreading: Only call one function in this class at a time.  Many
 *       of the functions seek around the underlying stream and thus will break
 *       if two or more functions are executing at the same time.
 */
class Image
{
	public:
		/// Capabilities of this image format.
		enum Caps {
			/// Set if the image can be resized with setDimensions().
			CanSetDimensions  = 0x01,

			/// Set if getPalette() returns valid data.
			HasPalette        = 0x02,

			/// Set if setPalette() can be used.  Must be used with HasPalette.
			CanSetPalette     = 0x04,

			/// Set if the image is 8bpp (256 colour)
			ColourDepthVGA    = 0x00,

			/// Set if the image is 4bpp (16 colour)
			ColourDepthEGA    = 0x10,

			/// Set if the image is 2bpp (4 colour)
			ColourDepthCGA    = 0x20,

			/// Set if the image is 1bpp (black and white)
			ColourDepthMono   = 0x30,

			/// Mask to isolate the ColourDepth value.  This must always be used when
			/// checking the colour depth.
			ColourDepthMask   = 0x30,
		};

		/// Extract the bit from the image mask that controls visibility.
		const static unsigned int Mask_Visibility = 0x01;

		/// Mask_Visibility bit for opaque pixels.
		const static unsigned int Mask_Vis_Opaque = 0;

		/// Mask_Visibility bit for transparent pixels.
		const static unsigned int Mask_Vis_Transparent = 1;

		/// This bit is set in a mask pixel if that pixel is part of the hitmap.
		const static unsigned int Mask_Hitmap = 0x02;

		/// Mask_Hit bit for passthrough pixels.
		const static unsigned int Mask_Hit_Pass = 0;

		/// Mask_Hit bit for touchable pixels.
		const static unsigned int Mask_Hit_Touch = 1;

		/// Get the capabilities of this image format.
		/**
		 * @return One or more of the \ref Caps enum values (OR'd together.)
		 */
		virtual int getCaps() = 0;

		/// Get the size of this image in pixels.
		/**
		 * @param width
		 *   Pointer to a variable that will receive the image's width.
		 *
		 * @param height
		 *   Pointer to a variable that will receive the image's height.
		 *
		 * @throw stream::error on I/O error.
		 */
		virtual void getDimensions(unsigned int *width, unsigned int *height) = 0;

		/// Set the size of this image in pixels.
		/**
		 * @pre getCaps() return value includes CanSetDimensions.
		 *
		 * @param width
		 *   New width
		 *
		 * @param height
		 *   New height
		 *
		 * @post Image content is undefined, fromStandard() must be called.
		 *
		 * @throw stream::error on I/O error.
		 */
		virtual void setDimensions(unsigned int width, unsigned int height) = 0;

		/// Convert the image into a standard format.
		/**
		 * This function converts whatever the underlying image format is into
		 * 8bpp indexed data.
		 *
		 * @return A shared pointer to a byte array of image data.
		 */
		virtual StdImageDataPtr toStandard() = 0;

		/// Convert the image mask into a standard format.
		/**
		 * This function converts whatever the underlying image format is into
		 * 8bpp data.
		 *
		 * The mask layout is identical to the image data, only instead of the
		 * values being colours, the least-significant bit of each byte
		 * (Mask_Visibility) is used to denote opacity (Mask_Vis_Opaque or
		 * Mask_Vis_Transparent) and the next bit (Mask_Hitmap)
		 * is used to denote hitmapping (Mask_Hit_Touch or Mask_Hit_Pass)
		 *
		 * @return A shared pointer to a byte array of mask data.
		 */
		virtual StdImageDataPtr toStandardMask() = 0;

		/// Replace the image with new content.
		/**
		 * Take image data in the standard format, convert it to the underlying
		 * custom format and overwrite the old image.
		 *
		 * The mask layout is identical to the image data, only instead of the
		 * values being colours, the least-significant bit of each byte
		 * (Mask_Visibility) is used to denote opacity (Mask_Vis_Opaque or
		 * Mask_Vis_Transparent) and the next bit (Mask_Hitmap)
		 * is used to denote hitmapping (Mask_Hit_Touch or Mask_Hit_Pass)
		 *
		 * @param newContent  Image data, in the standard 8bpp indexed format, to
		 *   convert and replace the underlying image with.
		 *
		 * @param newMask  Image data, in the standard 8bpp format, to
		 *   convert and replace the underlying mask with.
		 *
		 * @note If getCaps() reports CanSetDimensions the dimensions should be
		 *   set *before* this function is called (if necessary) as some formats
		 *   encode data differently depending on the image size (e.g. 'end of
		 *   line' codes can't be used unless the line width is known.)
		 */
		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask) = 0;

		/// Get the indexed colour map from the file.
		/**
		 * @pre getCaps() return value includes HasPalette.
		 *
		 * @return Shared pointer to a PaletteTable.
		 */
		virtual PaletteTablePtr getPalette() = 0;

		/// Set the indexed colour map used by the file.
		/**
		 * @pre getCaps() return value includes HasPalette.
		 *
		 * @param newPalette
		 *   New palette data
		 */
		virtual void setPalette(PaletteTablePtr newPalette) = 0;
};

/// Shared pointer to an Image.
typedef boost::shared_ptr<Image> ImagePtr;

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_GAMEGRAPHICS_IMAGE_HPP_
