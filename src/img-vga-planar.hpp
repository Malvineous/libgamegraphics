/**
 * @file  img-vga-planar.hpp
 * @brief Image implementation adding support for VGA mode X planar format.
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

#ifndef _CAMOTO_IMG_VGA_PLANAR_HPP_
#define _CAMOTO_IMG_VGA_PLANAR_HPP_

#include <camoto/gamegraphics/image.hpp>

namespace camoto {
namespace gamegraphics {

/// Planar VGA Image implementation.
/**
 * This class adds support for converting to and from planar VGA mode formats.
 * It does not handle image size (dimensions) so it should be inherited by more
 * specific format handlers.
 */
class Image_VGA_Planar: virtual public Image
{
	public:
		/// Constructor
		/**
		 * @param data
		 *   VGA data
		 *
		 * @param off
		 *   Offset from start of stream where VGA data begins.
		 */
		Image_VGA_Planar(std::unique_ptr<stream::inout> content, stream::pos off);
		virtual ~Image_VGA_Planar();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual Pixels convert() const;
		virtual Pixels convert_mask() const;
		virtual void convert(const Pixels& newContent, const Pixels& newMask);

	protected:
		std::unique_ptr<stream::inout> content; ///< Image content
		stream::pos off;         ///< Offset of image data in \ref data
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_VGA_PLANAR_HPP_
