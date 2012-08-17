/**
 * @file   baseimage.hpp
 * @brief  Base class for all Image subclasses.
 *
 * Copyright (C) 2010-2012 Adam Nielsen <malvineous@shikadi.net>
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

#ifndef _CAMOTO_GAMEGRAPHICS_BASEIMAGE_HPP_
#define _CAMOTO_GAMEGRAPHICS_BASEIMAGE_HPP_

#include <camoto/gamegraphics/image.hpp>

namespace camoto {
namespace gamegraphics {

/// Base class for images.
/**
 * This class implements common functions that would otherwise have to be
 * implemented in each subclass.
 */
class BaseImage: virtual public Image
{
	public:
		BaseImage();
		virtual ~BaseImage();

		/// Default function to throw exception.
		/**
		 * Always throws stream::error, complaining the caller should have checked
		 * getCaps() for the presence of CanSetDimensions.
		 *
		 * @throw stream::error on every call.
		 */
		virtual void setDimensions(unsigned int width, unsigned int height);

		/// Default function to return an empty palette.
		/**
		 * @return NULL pointer.
		 */
		virtual PaletteTablePtr getPalette();

		/// Default function to throw exception.
		/**
		 * Always throws stream::error, complaining the caller should have checked
		 * getCaps() for the presence of HasPalette.
		 *
		 * @throw stream::error on every call.
		 */
		virtual void setPalette(PaletteTablePtr newPalette);
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_GAMEGRAPHICS_BASEIMAGE_HPP_
