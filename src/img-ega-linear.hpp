/**
 * @file  img-ega-linear.hpp
 * @brief Image implementation adding support for linear EGA graphics.
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

#ifndef _CAMOTO_IMG_EGA_LINEAR_HPP_
#define _CAMOTO_IMG_EGA_LINEAR_HPP_

#include <camoto/bitstream.hpp>
#include "img-ega.hpp"

namespace camoto {
namespace gamegraphics {

/// EGA Image implementation.
/**
 * This class adds support for converting to and from 4bpp linear EGA format
 * (that is, two whole pixels stored in each byte.)  It does not handle image
 * size (dimensions) so it should be inherited by more specific format
 * handlers if the underlying format has fields for these values.
 */
class Image_EGA_Linear: virtual public Image_EGA {

	public:
		Image_EGA_Linear(std::unique_ptr<stream::inout> content,
			stream::pos offset, Point dimensions, EGAPlaneLayout planes,
			bitstream::endian endian, std::shared_ptr<const Palette> pal);
		virtual ~Image_EGA_Linear();

		virtual void convert(const Pixels& newContent, const Pixels& newMask);

	protected:
		/// Populate this->pixels and this->mask
		virtual void doConversion();

		stream::pos offset;   ///< Offset of image data from start of stream
		bitstream bits;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_EGA_LINEAR_HPP_
