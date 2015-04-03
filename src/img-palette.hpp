/**
 * @file  img-palette.hpp
 * @brief Image implementation of a Palette file.  This will be inherited by
 *        classes implementing a specific palette file formats.
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

#ifndef _CAMOTO_IMG_PALETTE_HPP_
#define _CAMOTO_IMG_PALETTE_HPP_

#include <camoto/gamegraphics/image.hpp>

namespace camoto {
namespace gamegraphics {

/// Image specialisation for palette files.
class Image_Palette: virtual public Image
{
	public:
		Image_Palette();
		virtual ~Image_Palette();

		virtual Caps caps() const;
		virtual Point dimensions() const;
		virtual Pixels convert() const;
		virtual Pixels convert_mask() const;
		virtual void convert(const Pixels& newContent, const Pixels& newMask);
};

} // namespace gamearchive
} // namespace camoto

#endif // _CAMOTO_IMG_PALETTE_HPP_
