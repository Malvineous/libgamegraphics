/**
 * @file   palettetable.hpp
 * @brief  Declaration of PaletteTable class, for accessing indexed colour maps
 *         used by game images.
 *
 * Copyright (C) 2010-2011 Adam Nielsen <malvineous@shikadi.net>
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

#ifndef _CAMOTO_GAMEGRAPHICS_PALETTETABLE_HPP_
#define _CAMOTO_GAMEGRAPHICS_PALETTETABLE_HPP_

#include <boost/shared_ptr.hpp>
#include <vector>

#include <camoto/stream.hpp>
#include <stdint.h>

namespace camoto {
namespace gamegraphics {

/// Single entry in a PaletteTable
struct PaletteEntry {
	uint8_t red;     ///< Red value (0-255)
	uint8_t green;   ///< Green value (0-255)
	uint8_t blue;    ///< Blue value (0-255)
	uint8_t alpha;   ///< Alpha value (0-255)

	/// Default constructor
	PaletteEntry();

	/// Helper constructor
	/**
	 * @param red
	 *   Red value (0-255)
	 *
	 * @param green
	 *   Green value (0-255)
	 *
	 * @param blue
	 *   Blue value (0-255)
	 *
	 * @param alpha
	 *   Alpha value (0-255).  0 is transparent, 255 is opaque.
	 */
		PaletteEntry(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255);
};

/// Shared pointer to a Palette.
typedef std::vector<PaletteEntry> PaletteTable;

/// Shared pointer to a Palette.
typedef boost::shared_ptr<PaletteTable> PaletteTablePtr;

/// Allocate memory for a palette and fill it with CGA defaults.
/**
 * @return Palette with 16 entries suitable for drawing CGA and EGA images.
 */
PaletteTablePtr createDefaultCGAPalette()
	throw ();

/// Allocate memory for a palette and fill it with EGA defaults.
/**
 * @return Palette with 64 entries suitable for drawing EGA images that
 *   make use of the EGA palette.
 */
PaletteTablePtr createDefaultEGAPalette()
	throw ();

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_GAMEGRAPHICS_PALETTETABLE_HPP_
