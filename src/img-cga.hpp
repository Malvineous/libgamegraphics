/**
 * @file   img-cga.hpp
 * @brief  Image implementation adding support for CGA graphics.
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

#ifndef _CAMOTO_IMG_CGA_HPP_
#define _CAMOTO_IMG_CGA_HPP_

#include <boost/iostreams/stream.hpp>
#include <camoto/bitstream.hpp>
#include <camoto/gamegraphics/image.hpp>

namespace camoto {
namespace gamegraphics {

namespace io = boost::iostreams;

/// CGA Image implementation.
/**
 * This class adds support for converting to and from CGA format.  It
 * does not handle image size (dimensions) so it should be inherited by more
 * specific format handlers.
 *
 * Currently it only supports one format - 2bpp packed (not planar)
 */
class CGAImage: virtual public Image {

	public:
		/// Base palette type.  Lower four bits are background colour.
		enum CGAPaletteType {
			GreenRed          = 0x00,
			GreenRedBright    = 0x80,
			CyanMagenta       = 0x10,
			CyanMagentaBright = 0x90,
			CyanRed           = 0x20,
			CyanRedBright     = 0xA0,
		};

		/// Constructor
		/**
		 * @param data
		 *   CGA data
		 *
		 * @param fnTruncate
		 *   Truncate callback
		 *
		 * @param off
		 *   Offset from start of stream where CGA data begins.
		 *
		 * @param cgaPal
		 *   CGA palette to use.  One of the CGAPaletteType values.  The lower
		 *   eight bits are used to represent the background colour.  If this is
		 *   not the default black, it can be specified here, e.g. for a background
		 *   colour of blue (colour #1) try (CGAPaletteType)(GreenRed | 1).  This
		 *   only needs to be specified when the background colour is not black.
		 */
		CGAImage(iostream_sptr data, FN_TRUNCATE fnTruncate, io::stream_offset off,
			CGAPaletteType cgaPal)
			throw ();

		/// Destructor.
		virtual ~CGAImage()
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

		virtual PaletteTablePtr getPalette()
			throw (std::ios::failure);

	protected:
		bitstream_sptr data;     ///< CGA image data
		FN_TRUNCATE fnTruncate;  ///< Truncate function for data stream
		io::stream_offset off;   ///< Offset of image data
		CGAPaletteType cgaPal;   ///< CGA palette to use

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_CGA_HPP_
