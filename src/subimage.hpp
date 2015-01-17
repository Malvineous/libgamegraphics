/**
 * @file   subimage.hpp
 * @brief  Image specialisation images within other images.
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

#ifndef _CAMOTO_SUBIMAGE_HPP_
#define _CAMOTO_SUBIMAGE_HPP_

#include <boost/function.hpp>
#include <camoto/gamegraphics/imagetype.hpp>
#include "baseimage.hpp"

namespace camoto {
namespace gamegraphics {

/// Function to call when a subimage has been changed.
/**
 * This function should make note that one (or more) subimages have been
 * changed, and eventually call img->fromStandard() to write the changes
 * to the underlying image in one pass.
 */
typedef boost::function<void()> fn_image_changed;

/// Image stored within another Image.
class SubImage: virtual public BaseImage
{
	public:
		/// Constructor
		/**
		 * Create an image from the supplied stream.
		 *
		 * @param img
		 *   Underlying image.
		 *
		 * @param stdImg
		 *   Value of img->toStandard(), supplied here to avoid calling it once for
		 *   every sub-image.
		 *
		 * @param stdMask
		 *   Value of img->toStandardMask(), supplied here to avoid calling it once
		 *   for every sub-image.
		 *
		 * @param xOffset
		 *   X-coordinate of pixel in underlying image that will appear at (0,0) in
		 *   this image.  Must be contained within the underlying image.
		 *
		 * @param yOffset
		 *   Y-coordinate of pixel in underlying image that will appear at (0,0) in
		 *   this image.  Must be contained within the underlying image.
		 *
		 * @param width
		 *   Width of this image.  x + width must not exceed the width of the
		 *   underlying image.
		 *
		 * @param height
		 *   Height of this image.  y + height must not exceed the width of the
		 *   underlying image.
		 *
		 * @param fnImageChanged
		 *   Callback function called when fromStandard() has been used to update
		 *   this subimage.  Unless this function has been called at least once,
		 *   there is no need for the owner to write back to img.
		 */
		SubImage(ImagePtr img, StdImageDataPtr stdImg, StdImageDataPtr stdMask,
			unsigned int xOffset, unsigned int yOffset, unsigned int width,
			unsigned int height, fn_image_changed fnImageChanged);
		virtual ~SubImage();

		virtual int getCaps();
		virtual void getDimensions(unsigned int *width, unsigned int *height);
		virtual void setDimensions(unsigned int width, unsigned int height);
		virtual StdImageDataPtr toStandard();
		virtual StdImageDataPtr toStandardMask();
		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask);

	protected:
		/// Common code between toStandard() and toStandardMask()
		StdImageDataPtr extractPortion(const StdImageDataPtr& source);

		ImagePtr img;               ///< Underlying image
		StdImageDataPtr parent;     ///< Pixel data cache
		StdImageDataPtr parentMask; ///< Pixel data cache for mask
		unsigned int xOffset;
		unsigned int yOffset;
		unsigned int width;
		unsigned int height;
		fn_image_changed fnImageChanged; ///< Called to flag a change
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_SUBIMAGE_HPP_
