/**
 * @file   img-palette_only.cpp
 * @brief  Graphics implementation of a Palette file.
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

#ifndef _CAMOTO_IMG_PALETTE_HPP_
#define _CAMOTO_IMG_PALETTE_HPP_

#include <vector>
#include <camoto/gamegraphics/graphicstype.hpp>
#include <camoto/gamegraphics/graphics.hpp>
#include <camoto/segmented_stream.hpp>
#include <camoto/iostream_helpers.hpp>

namespace camoto {
namespace gamegraphics {

class PaletteGraphics: virtual public Graphics {

	public:
		PaletteGraphics()
			throw ();

		virtual ~PaletteGraphics()
			throw ();

		virtual int getCaps()
			throw ();

		virtual int getTilesetCount()
			throw ();

		virtual GraphicsPtr getTileset(int index)
			throw ();

		virtual void getTileSize(unsigned int *width, unsigned int *height)
			throw ();

		virtual unsigned int getLayoutWidth()
			throw ();

		/// @note This function should never be called (as per getCaps() response.)
		virtual void setTileSize(unsigned int x, unsigned int y)
			throw (std::ios::failure);

		/// @note For this format, this function will always return 0.)
		virtual int getImageCount()
			throw ();

		/// @note This function should never be called (as per getCaps() response.)
		virtual void setImageCount(int count)
			throw (std::ios::failure);

		/// @note This function should never be called (as getImageCount() returns 0.)
		virtual ImageConverterPtr openImage(int index)
			throw ();

};

} // namespace gamearchive
} // namespace camoto

#endif // _CAMOTO_IMG_PALETTE_HPP_
