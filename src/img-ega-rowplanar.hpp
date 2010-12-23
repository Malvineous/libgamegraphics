/**
 * @file   img-ega-rowplanar.hpp
 * @brief  Image implementation adding support for the EGA row-planar format.
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

#ifndef _CAMOTO_IMG_EGA_ROWPLANAR_HPP_
#define _CAMOTO_IMG_EGA_ROWPLANAR_HPP_

#include <camoto/gamegraphics/image.hpp>
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
class EGARowPlanarImage: virtual public Image {
	protected:
		iostream_sptr data;
		FN_TRUNCATE fnTruncate;
		io::stream_offset offset;
		int width, height;
		PLANE_LAYOUT planes;

	public:
		EGARowPlanarImage()
			throw ();

		virtual ~EGARowPlanarImage()
			throw ();

		/// These could be set in the constructor, but often descendent classes
		/// won't have these values until the end of their constructors.
		virtual void setParams(iostream_sptr data, FN_TRUNCATE fnTruncate,
			io::stream_offset offset, int width, int height,
			const PLANE_LAYOUT& planes)
			throw ();

		virtual int getCaps()
			throw ();

		virtual void getDimensions(unsigned int *width, unsigned int *height)
			throw ();

		virtual void setDimensions(unsigned int width, unsigned int height)
			throw (std::ios::failure);

		virtual StdImageDataPtr toStandard()
			throw ();

		virtual StdImageDataPtr toStandardMask()
			throw ();

		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask)
			throw ();

	protected:
		StdImageDataPtr doConversion(bool mask)
			throw ();

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_EGA_ROWPLANAR_HPP_
