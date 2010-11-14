/**
 * @file   img-vga.hpp
 * @brief  Image implementation adding support for VGA mode 13 format.
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

#ifndef _CAMOTO_IMG_VGA_HPP_
#define _CAMOTO_IMG_VGA_HPP_

#include <boost/iostreams/stream.hpp>
#include <camoto/gamegraphics/image.hpp>

namespace camoto {
namespace gamegraphics {

namespace io = boost::iostreams;

/// VGA Image implementation.
/**
 * This class adds support for converting to and from VGA mode 13 format.  It
 * does not handle image size (dimensions) so it should be inherited by more
 * specific format handlers.
 */
class VGAImage: virtual public Image {
	protected:
		iostream_sptr data;
		FN_TRUNCATE fnTruncate;
		io::stream_offset off; ///< Offset of image data

	public:
		/// Constructor
		/**
		 * @param data
		 *   VGA data
		 *
		 * @param fnTruncate
		 *   Truncate callback
		 *
		 * @param off
		 *   Offset from start of stream where VGA data begins.
		 */
		VGAImage(iostream_sptr data, FN_TRUNCATE fnTruncate, io::stream_offset off)
			throw ();

		virtual ~VGAImage()
			throw ();

		virtual int getCaps()
			throw ();

		//virtual void getDimensions(unsigned int *width, unsigned int *height)
		//	throw ();

		//virtual void setDimensions(unsigned int width, unsigned int height)
		//	throw (std::ios::failure);

		virtual StdImageDataPtr toStandard()
			throw ();

		virtual StdImageDataPtr toStandardMask()
			throw ();

		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask)
			throw ();

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_VGA_HPP_
