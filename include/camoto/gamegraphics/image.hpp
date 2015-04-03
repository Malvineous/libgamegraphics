/**
 * @file  camoto/gamegraphics/image.hpp
 * @brief Image class interface, used to convert between various game
 *         formats and a standard image data format.  Actual format conversions
 *         are handled by other classes which implement this interface.
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

#ifndef _CAMOTO_GAMEGRAPHICS_IMAGE_HPP_
#define _CAMOTO_GAMEGRAPHICS_IMAGE_HPP_

#include <array>
#include <memory>
#include <cstdint>

#include <camoto/stream.hpp> // for stream::error
#include <camoto/gamegraphics/palette.hpp>

namespace camoto {
namespace gamegraphics {

/// Shared pointer to the raw image data
typedef std::vector<uint8_t> Pixels;

struct Point
{
	unsigned int x;
	unsigned int y;
};

enum class ColourDepth
{
	Mono, ///< Set if the image is 1bpp (black and white)
	CGA,  ///< Set if the image is 2bpp (4 colour)
	EGA,  ///< Set if the image is 4bpp (16 colour)
	VGA,  ///< Set if the image is 8bpp (256 colour)
};

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
		enum class Caps {
			/// No special capabilities.
			Default       = 0x00,

			/// Set if the image can be resized with setDimensions().
			SetDimensions = 0x01,

			/// Set if palette() returns valid data.
			HasPalette    = 0x02,

			/// Set if palette() can be used.  Must be used with HasPalette.
			SetPalette    = 0x04,

			/// Set if get/setHotspot() can be used.
			HasHotspot    = 0x08,

			/// Set if get/setHitRect() can be used.
			HasHitRect        = 0x40,
		};

		/// Bit values for image mask data
		enum class Mask : uint8_t {
			Transparent = 0x01, ///< Mask: 0=opaque, 1=transparent
			Touch       = 0x02, ///< Mask: 0=pass, 1=hit
		};

		Image();
		virtual ~Image();

		/// Get the capabilities of this image format.
		/**
		 * @return One or more of the \ref Caps enum values (OR'd together.)
		 */
		virtual Caps caps() const = 0;

		/// Get the number of colours that can be stored in this image format.
		/**
		 * @return One \ref ColourDepth value.
		 */
		virtual ColourDepth colourDepth() const = 0;

		/// Get the size of this image in pixels.
		/**
		 * @return Point structure with .x as image width and .y as image height.
		 *
		 * @throw stream::error on I/O error.
		 */
		virtual Point dimensions() const = 0;

		/// Set the size of this image in pixels.
		/**
		 * @pre caps() return value includes CanSetDimensions.
		 *
		 * @param newDimensions
		 *   New dimensions to set.
		 *
		 * @post Image content is undefined, fromStandard() must be called.
		 *
		 * @throw stream::error on I/O error.
		 */
		virtual void dimensions(const Point& newDimensions);

		/// Get the coordinates of the image hotspot, in pixels.
		/**
		 * The hotspot is the part of the image that appears at its origin.
		 *
		 * @pre caps() return value includes HasHotspot.
		 *
		 * @return Point containing hotspot coordinates, in pixels from the top-left.
		 *
		 * @throw stream::error on I/O error.
		 */
		virtual Point hotspot() const;

		/// Set the coordinates of the image hotspot, in pixels.
		/**
		 * @pre caps() return value includes HasHotspot.
		 *
		 * @param newHotspot
		 *   Coordinates of the new hotspot to set.
		 *
		 * @throw stream::error on I/O error.
		 */
		virtual void hotspot(const Point& newHotspot);

		/// Get the coordinates of the lower-right corner of the hitmap rectangle.
		/**
		 * This function is used for image formats that have a rectangular hitmap
		 * rectangle rather than a hitmap plane in the image data.
		 *
		 * @pre caps() return value includes HasHitRect.
		 *
		 * @param x
		 *   Pointer to a variable that will receive the hitmap rectangle's right
		 *   coordinate.
		 *
		 * @param y
		 *   Pointer to a variable that will receive the hitmap rectangle's bottom
		 *   coordinate.
		 *
		 * @throw stream::error on I/O error.
		 */
		virtual Point hitrect() const;

		/// Set the coordinates of the lower-right corner of the hitmap rectangle.
		/**
		 * This function is used for image formats that have a rectangular hitmap
		 * rectangle rather than a hitmap plane in the image data.
		 *
		 * @pre caps() return value includes HasHitRect.
		 *
		 * @param x
		 *   X coordinate of lower-right rectangle corner.
		 *
		 * @param y
		 *   Y coordinate of lower-right rectangle corner.
		 *
		 * @throw stream::error on I/O error.
		 */
		virtual void hitrect(const Point& newHitRect);

		/// Convert the image into a standard format.
		/**
		 * This function converts whatever the underlying image format is into
		 * 8bpp indexed data.
		 *
		 * @return A shared pointer to a byte array of image data.
		 */
		virtual Pixels convert() const = 0;

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
		virtual Pixels convert_mask() const = 0;

		/// Replace the image with new content.
		/**
		 * Take image data in the standard format, convert it to the underlying
		 * custom format and overwrite the old image.
		 *
		 * The mask layout is identical to the image data, only instead of the
		 * values being colours, the values in \ref Mask are bitwise-OR'd and
		 * stored as one-byte-per-pixel.
		 *
		 * @param newContent
		 *   Image data, in the standard 8bpp indexed format, to convert and
		 *   replace the underlying image with.
		 *
		 * @param newMask
		 *   Image data, in the standard 8bpp format, to convert and replace the
		 *   underlying mask with.
		 *
		 * @note If caps() reports CanSetDimensions the dimensions should be
		 *   set *before* this function is called (if necessary) as some formats
		 *   encode data differently depending on the image size (e.g. 'end of
		 *   line' codes can't be used unless the line width is known.)
		 */
		virtual void convert(const Pixels& newContent,
			const Pixels& newMask) = 0;

		/// Get the indexed colour map from the file.
		/**
		 * @pre caps() return value includes HasPalette.
		 *
		 * @return Shared pointer to a Palette.
		 */
		virtual std::shared_ptr<const Palette> palette() const;

		/// Set the indexed colour map used by the file.
		/**
		 * @pre caps() return value includes CanSetPalette.
		 *
		 * @param newPalette
		 *   New palette data
		 */
		virtual void palette(std::shared_ptr<const Palette> newPalette);

	private:
		std::shared_ptr<const Palette> pal; ///< Palette storage, may be null
};

inline Image::Caps operator| (Image::Caps a, Image::Caps b) {
	return static_cast<Image::Caps>(
		static_cast<unsigned int>(a) | static_cast<unsigned int>(b)
	);
}

inline bool operator& (Image::Caps a, Image::Caps b) {
	return
		static_cast<unsigned int>(a) & static_cast<unsigned int>(b)
	;
}

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_GAMEGRAPHICS_IMAGE_HPP_
