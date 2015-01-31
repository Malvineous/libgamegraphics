/**
 * @file  img-ddave.hpp
 * @brief Image specialisation for Dangerous Dave CGA/EGA/VGA images.
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

#ifndef _CAMOTO_IMG_DDAVE_HPP_
#define _CAMOTO_IMG_DDAVE_HPP_

#include <camoto/gamegraphics/imagetype.hpp>
#include "img-cga.hpp"
#include "img-ega-rowplanar.hpp"
#include "img-vga.hpp"

#include "img-ega-byteplanar.hpp" //temp
namespace camoto {
namespace gamegraphics {

/// Dangerous Dave CGA Image implementation.
class Image_DDaveCGA: virtual public Image_CGA {

	public:
		/// Constructor
		/**
		 * Create an image from the supplied stream.
		 *
		 * @param data
		 *   CGA image data.
		 *
		 * @param fixedSize
		 *   True if this image is fixed at 16x16 (no width/height header)
		 */
		Image_DDaveCGA(stream::inout_sptr data, bool fixedSize);

		virtual ~Image_DDaveCGA();

		virtual int getCaps();

		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask);

	protected:
		stream::inout_sptr stream_data; // stream::inout_sptr copy, as inherited one is a bitstream
		bool fixedSize;

};

/// Dangerous Dave EGA Image implementation.
class Image_DDaveEGA: virtual public Image_EGARowPlanar {

	public:
		/// Constructor
		/**
		 * Create an image from the supplied stream.
		 *
		 * @param data
		 *   EGA image data.
		 *
		 * @param fixedSize
		 *   True if this image is fixed at 16x16 (no width/height header)
		 */
		Image_DDaveEGA(stream::inout_sptr data, bool fixedSize);

		virtual ~Image_DDaveEGA();

		virtual int getCaps();

		// getDimension and setDimensions inherited from parent class

		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask);

	protected:
		bool fixedSize;

};

/// Dangerous Dave VGA Image implementation.
class Image_DDaveVGA: virtual public Image_VGA {

	public:
		/// Constructor
		/**
		 * Create an image from the supplied stream.
		 *
		 * @param data
		 *   VGA image data.
		 *
		 * @param fixedSize
		 *   True if this image is fixed at 16x16 (no width/height header)
		 *
		 * @param pal
		 *   Image palette.
		 */
		Image_DDaveVGA(stream::inout_sptr data, bool fixedSize,
			PaletteTablePtr pal);

		virtual ~Image_DDaveVGA();

		virtual int getCaps();

		virtual void getDimensions(unsigned int *width, unsigned int *height);

		virtual void setDimensions(unsigned int width, unsigned int height);

		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask);

		virtual PaletteTablePtr getPalette();

	protected:
		uint16_t width;
		uint16_t height;
		bool fixedSize;
		PaletteTablePtr pal;

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_DDAVE_HPP_
