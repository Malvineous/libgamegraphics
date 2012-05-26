/**
 * @file   img-cga.hpp
 * @brief  Image implementation adding support for CGA graphics.
 *
 * Copyright (C) 2010-2012 Adam Nielsen <malvineous@shikadi.net>
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
#include <camoto/gamegraphics/imagetype.hpp>

namespace camoto {
namespace gamegraphics {

/// CGA Image implementation.
/**
 * This class adds support for converting to and from CGA format.  It
 * does not handle image size (dimensions) so it should be inherited by more
 * specific format handlers.
 *
 * Currently it only supports one format - 2bpp packed (not planar)
 */
class CGAImage: virtual public Image
{
	public:
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
		 *   CGA palette to use.  See generatePalette() for details.
		 */
		CGAImage(stream::inout_sptr data, stream::pos off, unsigned int width,
			unsigned int height, CGAPaletteType cgaPal)
			throw ();

		/// Destructor.
		virtual ~CGAImage()
			throw ();

		virtual int getCaps()
			throw ();

		virtual void getDimensions(unsigned int *width, unsigned int *height)
			throw ();

		virtual void setDimensions(unsigned int width, unsigned int height)
			throw (stream::error);

		virtual StdImageDataPtr toStandard()
			throw ();

		virtual StdImageDataPtr toStandardMask()
			throw ();

		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask)
			throw ();

		virtual PaletteTablePtr getPalette()
			throw (stream::error);

	protected:
		stream::inout_sptr parent; ///< Parent stream under the bitstream
		bitstream_sptr data;       ///< CGA image data
		stream::pos off;           ///< Offset of image data
		unsigned int width;        ///< Image width in pixels
		unsigned int height;       ///< Image height in pixels
		CGAPaletteType cgaPal;     ///< CGA palette to use
};

/// Filetype handler for full screen raw CGA images.
class CGARawLinearImageType: virtual public ImageType
{
	public:
		CGARawLinearImageType()
			throw ();

		virtual ~CGARawLinearImageType()
			throw ();

		virtual std::string getCode() const
			throw ();

		virtual std::string getFriendlyName() const
			throw ();

		virtual std::vector<std::string> getFileExtensions() const
			throw ();

		virtual std::vector<std::string> getGameList() const
			throw ();

		virtual Certainty isInstance(stream::input_sptr fsImage) const
			throw (stream::error);

		virtual ImagePtr create(stream::inout_sptr psImage,
			SuppData& suppData) const
			throw (stream::error);

		virtual ImagePtr open(stream::inout_sptr fsImage,
			SuppData& suppData) const
			throw (stream::error);

		virtual SuppFilenames getRequiredSupps(const std::string& filenameImage) const
			throw ();

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_CGA_HPP_
