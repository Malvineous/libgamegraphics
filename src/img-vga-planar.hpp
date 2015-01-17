/**
 * @file   img-vga-planar.hpp
 * @brief  Image implementation adding support for VGA mode X planar format.
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

#include <boost/iostreams/stream.hpp>
#include "baseimage.hpp"

namespace camoto {
namespace gamegraphics {

/// VGA Image implementation.
/**
 * This class adds support for converting to and from VGA mode 13 format.  It
 * does not handle image size (dimensions) so it should be inherited by more
 * specific format handlers.
 */
class VGAPlanarImage: virtual public BaseImage
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
		VGAPlanarImage(stream::inout_sptr data, stream::pos off);
		virtual ~VGAPlanarImage();

		virtual int getCaps();
		//virtual void getDimensions(unsigned int *width, unsigned int *height);
		//virtual void setDimensions(unsigned int width, unsigned int height);
		virtual StdImageDataPtr toStandard();
		virtual StdImageDataPtr toStandardMask();
		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask);

	protected:
		stream::inout_sptr data; ///< Image content
		stream::pos off;         ///< Offset of image data in \ref data
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_VGA_PLANAR_HPP_
