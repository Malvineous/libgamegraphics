/**
 * @file   img-ddave.hpp
 * @brief  Image specialisation for Dangerous Dave CGA/EGA/VGA images.
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
class DDaveCGAImage: virtual public CGAImage {

	public:
		/// Constructor
		/**
		 * No truncate function is required as the image dimensions are fixed, so
		 * the file size will always remain constant.
		 *
		 * @param data
		 *   VGA data
		 *
		 * @param pal
		 *   Image palette
		 */
		DDaveCGAImage(iostream_sptr data, FN_TRUNCATE fnTruncate, bool fixedSize)
			throw ();

		virtual ~DDaveCGAImage()
			throw ();

		virtual int getCaps()
			throw ();

		virtual void getDimensions(unsigned int *width, unsigned int *height)
			throw ();

		virtual void setDimensions(unsigned int width, unsigned int height)
			throw (std::ios::failure);

		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask)
			throw ();

	protected:
		uint16_t width;
		uint16_t height;
		iostream_sptr stream_data; // iostream_sptr copy, as inherited one is a bitstream
		bool fixedSize;

};

/// Dangerous Dave EGA Image implementation.
class DDaveEGAImage: virtual public EGARowPlanarImage {

	public:
		/// Constructor
		/**
		 * No truncate function is required as the image dimensions are fixed, so
		 * the file size will always remain constant.
		 *
		 * @param data
		 *   VGA data
		 *
		 * @param pal
		 *   Image palette
		 */
		DDaveEGAImage(iostream_sptr data, FN_TRUNCATE fnTruncate, bool fixedSize)
			throw ();

		virtual ~DDaveEGAImage()
			throw ();

		virtual int getCaps()
			throw ();

		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask)
			throw ();

	protected:
		uint16_t width;
		uint16_t height;
		bool fixedSize;

};

/// Dangerous Dave VGA Image implementation.
class DDaveVGAImage: virtual public VGAImage {

	public:
		/// Constructor
		/**
		 * No truncate function is required as the image dimensions are fixed, so
		 * the file size will always remain constant.
		 *
		 * @param data
		 *   VGA data
		 *
		 * @param pal
		 *   Image palette
		 */
		DDaveVGAImage(iostream_sptr data, FN_TRUNCATE fnTruncate, bool fixedSize,
			PaletteTablePtr pal)
			throw ();

		virtual ~DDaveVGAImage()
			throw ();

		virtual int getCaps()
			throw ();

		virtual void getDimensions(unsigned int *width, unsigned int *height)
			throw ();

		virtual void setDimensions(unsigned int width, unsigned int height)
			throw (std::ios::failure);

		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask)
			throw ();

		virtual PaletteTablePtr getPalette()
			throw (std::ios::failure);

	protected:
		uint16_t width;
		uint16_t height;
		bool fixedSize;
		PaletteTablePtr pal;

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_DDAVE_HPP_
