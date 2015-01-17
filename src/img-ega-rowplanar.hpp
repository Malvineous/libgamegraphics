/**
 * @file   img-ega-rowplanar.hpp
 * @brief  Image implementation adding support for the EGA row-planar format.
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

#ifndef _CAMOTO_IMG_EGA_ROWPLANAR_HPP_
#define _CAMOTO_IMG_EGA_ROWPLANAR_HPP_

#include "img-ega-common.hpp"

namespace camoto {
namespace gamegraphics {

/// EGA row-planar Image implementation.
/**
 * This class adds support for converting to and from EGA row-planar format.
 *
 * Up to six image planes are supported - the usual RGBI planes, as well as
 * transparency and hitmapping.
 *
 */
class EGARowPlanarImage: virtual public BaseImage {
	protected:
		stream::inout_sptr data;
		stream::pos offset;
		int width, height;
		PLANE_LAYOUT planes;

	public:
		EGARowPlanarImage();

		virtual ~EGARowPlanarImage();

		/// These could be set in the constructor, but often descendent classes
		/// won't have these values until the end of their constructors.
		virtual void setParams(stream::inout_sptr data,
			stream::pos offset, int width, int height,
			const PLANE_LAYOUT& planes);

		virtual int getCaps();

		virtual void getDimensions(unsigned int *width, unsigned int *height);

		virtual void setDimensions(unsigned int width, unsigned int height);

		virtual StdImageDataPtr toStandard();

		virtual StdImageDataPtr toStandardMask();

		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask);

	protected:
		StdImageDataPtr doConversion(bool mask);

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_EGA_ROWPLANAR_HPP_
