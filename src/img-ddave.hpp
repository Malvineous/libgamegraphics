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
#include "img-ega-linear.hpp"
#include "img-ega-rowplanar.hpp"
#include "img-vga.hpp"

namespace camoto {
namespace gamegraphics {

/// Dangerous Dave CGA Image implementation.
class Image_DDaveCGA: virtual public Image_EGA_Linear
{
	public:
		/// Constructor
		/**
		 * Create an image from the supplied stream.
		 *
		 * @param content
		 *   CGA image data.
		 *
		 * @param fixedSize
		 *   True if this image is fixed at 16x16 (no width/height header)
		 */
		Image_DDaveCGA(std::unique_ptr<stream::inout> content, bool fixedSize);
		virtual ~Image_DDaveCGA();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		void convert(const Pixels& newContent, const Pixels& newMask);

	protected:
		bool fixedSize;
};

/// Dangerous Dave EGA Image implementation.
class Image_DDaveEGA: virtual public Image_EGA_RowPlanar
{
	public:
		/// Constructor
		/**
		 * Create an image from the supplied stream.
		 *
		 * @param content
		 *   EGA image data.
		 *
		 * @param fixedSize
		 *   True if this image is fixed at 16x16 (no width/height header)
		 */
		Image_DDaveEGA(std::unique_ptr<stream::inout> content, bool fixedSize);
		virtual ~Image_DDaveEGA();

		virtual Caps caps() const;
		virtual void convert(const Pixels& newContent, const Pixels& newMask);

	protected:
		bool fixedSize;
};

/// Dangerous Dave VGA Image implementation.
class Image_DDaveVGA: virtual public Image_VGA
{
	public:
		/// Constructor
		/**
		 * Create an image from the supplied stream.
		 *
		 * @param content
		 *   VGA image data.
		 *
		 * @param fixedSize
		 *   True if this image is fixed at 16x16 (no width/height header)
		 *
		 * @param pal
		 *   Image palette.
		 */
		Image_DDaveVGA(std::unique_ptr<stream::inout> content, bool fixedSize,
			std::shared_ptr<const Palette> pal);
		virtual ~Image_DDaveVGA();

		virtual Caps caps() const;
		virtual Point dimensions() const;
		virtual void dimensions(const Point& newDimensions);
		virtual void convert(const Pixels& newContent, const Pixels& newMask);

	protected:
		Point dims;
		bool fixedSize;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_DDAVE_HPP_
