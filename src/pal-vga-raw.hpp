/**
 * @file   pal-vga-raw.hpp
 * @brief  Palette interface to 768-byte raw 6-bit VGA palette files.
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

#ifndef _CAMOTO_GAMEGRAPHICS_PAL_VGA_RAW_HPP_
#define _CAMOTO_GAMEGRAPHICS_PAL_VGA_RAW_HPP_

#include <camoto/gamegraphics/palette.hpp>

namespace camoto {
namespace gamegraphics {

/// Palette interface to 768-byte raw 6-bit VGA palette files.
class VGAPalette: virtual public Palette {

	private:
		iostream_sptr data;

	public:
		VGAPalette(iostream_sptr data)
			throw (std::ios::failure);

		virtual ~VGAPalette()
			throw ();

		virtual int getMaxEntries()
			throw ();

		virtual void setEntries(const PaletteEntries& source)
			throw (std::ios::failure);

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_GAMEGRAPHICS_PAL_VGA_RAW_HPP_
