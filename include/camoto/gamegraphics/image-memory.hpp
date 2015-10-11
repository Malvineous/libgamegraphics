/**
 * @file  camoto/gamegraphics/image-memory.hpp
 * @brief Implementation of an in-memory Image.
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

#ifndef _CAMOTO_GAMEGRAPHICS_IMAGE_MEMORY_HPP_
#define _CAMOTO_GAMEGRAPHICS_IMAGE_MEMORY_HPP_

#include <camoto/gamegraphics/image.hpp>

namespace camoto {
namespace gamegraphics {

/// In-memory implementation of an Image.
class Image_Memory: public Image
{
	public:
		/**
		 * @param dims
		 *   Image dimensions, in pixels.
		 *
		 * @param pixels
		 *   Main image content.
		 *
		 * @param mask
		 *   Image mask.
		 *
		 * @param hotspot
		 *   Hotspot, in pixels.  Use (0,0) if no hotspot.
		 *
		 * @param hitrect
		 *   Lower-right corner of hitmap rect (hotspot is upper-left.)  Use (0,0)
		 *   if no hitrect.
		 *
		 * @param pal
		 *   Image palette, or nullptr if no palette.
		 */
		Image_Memory(const Point& dims, const Pixels& pixels, const Pixels& mask,
			const Point& hotspot, const Point& hitrect,
			std::shared_ptr<const Palette> pal);
		virtual ~Image_Memory();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual Point dimensions() const;
		virtual Point hotspot() const;
		virtual Point hitrect() const;
		virtual Pixels convert() const;
		virtual Pixels convert_mask() const;
		virtual void convert(const Pixels& newContent,
			const Pixels& newMask);

	protected:
		Point dims;
		Pixels pixels;
		Pixels mask;
		Point ptHotspot;
		Point ptHitrect;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_GAMEGRAPHICS_IMAGE_MEMORY_HPP_
