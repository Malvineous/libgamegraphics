/**
 * @file  img-ega-byteplanar-tiled.hpp
 * @brief Image composed of 8x8 EGA byte-planar tiles.
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

#ifndef _CAMOTO_IMG_EGA_BYTEPLANAR_TILED_HPP_
#define _CAMOTO_IMG_EGA_BYTEPLANAR_TILED_HPP_

#include <camoto/gamegraphics/palettetable.hpp>
#include "img-ega-common.hpp"

namespace camoto {
namespace gamegraphics {

/// EGA byte-planar-tiled Image implementation.
/**
 * This class adds support for converting to and from images composed of
 * multiple 8x8 EGA byte-planar format tiles.
 *
 * Up to six image planes are supported - the usual RGBI planes, as well as
 * transparency and hitmapping.
 *
 */
class Image_EGABytePlanarTiled: virtual public Image_Base {
	protected:
		stream::inout_sptr data;
		stream::pos offset;
		unsigned int width, height;
		PLANE_LAYOUT planes;
		PaletteTablePtr pal;

	public:
		Image_EGABytePlanarTiled();
		virtual ~Image_EGABytePlanarTiled();

		/// These could be set in the constructor, but often descendent classes
		/// won't have these values until the end of their constructors.
		virtual void setParams(stream::inout_sptr data,
			stream::pos offset, int width, int height,
			const PLANE_LAYOUT& planes, PaletteTablePtr pal);

		virtual int getCaps();
		virtual void getDimensions(unsigned int *width, unsigned int *height);
		virtual void setDimensions(unsigned int width, unsigned int height);
		virtual StdImageDataPtr toStandard();
		virtual StdImageDataPtr toStandardMask();
		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask);
		virtual PaletteTablePtr getPalette();

	protected:
		StdImageDataPtr doConversion(bool mask);

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_EGA_BYTEPLANAR_TILED_HPP_
