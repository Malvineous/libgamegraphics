/**
 * @file   palettetable.hpp
 * @brief  Palette functions, for accessing indexed colour maps used by images.
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

/// Base palette type.  Lower four bits are background colour.
enum CGAPaletteType {
	CGAPal_GreenRed          = 0x00,
	CGAPal_GreenRedBright    = 0x80,
	CGAPal_CyanMagenta       = 0x10,
	CGAPal_CyanMagentaBright = 0x90,
	CGAPal_CyanRed           = 0x20,
	CGAPal_CyanRedBright     = 0xA0,
};

/// Generate a black and white palette.
PaletteTablePtr createPalette_DefaultMono()
	throw ();

/// Generate a CGA palette.
/**
 * @param cgaPal
 *   CGA palette to use.  One of the CGAPaletteType values.  The lower
 *   eight bits are used to represent the background colour.  If this is
 *   not the default black, it can be specified here, e.g. for a background
 *   colour of blue (colour #1) try (CGAPaletteType)(GreenRed | 1).  This
 *   only needs to be specified when the background colour is not black.
 */
PaletteTablePtr createPalette_CGA(CGAPaletteType cgaPal)
	throw ();

/// Allocate memory for a palette and fill it with CGA defaults.
/**
 * @return Palette with 16 entries suitable for drawing CGA and EGA images.
 */
PaletteTablePtr createPalette_FullCGA()
	throw ();

/// The default EGA palette is the same as the full CGA one.
#define createPalette_DefaultEGA createPalette_FullCGA

/// Allocate memory for a palette and fill it with EGA defaults.
/**
 * @return Palette with 64 entries suitable for drawing EGA images that
 *   make use of the EGA palette.
 */
PaletteTablePtr createPalette_FullEGA()
	throw ();

/// Generate the default VGA mode 13 palette.
PaletteTablePtr createPalette_DefaultVGA()
	throw ();

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_GAMEGRAPHICS_PALETTETABLE_HPP_
