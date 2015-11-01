/**
 * @file  img-ega-rowplanar.hpp
 * @brief Image implementation adding support for the EGA row-planar format.
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

#include <camoto/config.hpp>
#include "img-ega.hpp"

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
class CAMOTO_GAMEGRAPHICS_API Image_EGA_RowPlanar: public Image_EGA
{
	public:
		Image_EGA_RowPlanar(std::unique_ptr<stream::inout> content,
			stream::pos offset, Point dimensions, EGAPlaneLayout planes,
			std::shared_ptr<const Palette> pal);
		virtual ~Image_EGA_RowPlanar();

		virtual void convert(const Pixels& newContent, const Pixels& newMask);

	protected:
		/// Populate this->pixels and this->mask
		virtual void doConversion();
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_EGA_ROWPLANAR_HPP_
