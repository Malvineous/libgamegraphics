/**
 * @file  camoto/gamegraphics/palette.hpp
 * @brief Palette functions, for accessing indexed colour maps used by images.
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

#ifndef _CAMOTO_GAMEGRAPHICS_PALETTE_HPP_
#define _CAMOTO_GAMEGRAPHICS_PALETTE_HPP_

#include <cstdint>
#include <memory>
#include <vector>
#include <camoto/config.hpp>

namespace camoto {
namespace gamegraphics {

/// Single entry in a Palette
struct PaletteEntry {
	std::uint8_t red;     ///< Red value (0-255)
	std::uint8_t green;   ///< Green value (0-255)
	std::uint8_t blue;    ///< Blue value (0-255)
	std::uint8_t alpha;   ///< Alpha value (0-255)
};

/// Shared pointer to a Palette.
typedef std::vector<PaletteEntry> Palette;

/// Base palette type.  Lower four bits are background colour.
enum class CGAPaletteType {
	GreenRed          = 0x00,
	GreenRedBright    = 0x80,
	CyanMagenta       = 0x10,
	CyanMagentaBright = 0x90,
	CyanRed           = 0x20,
	CyanRedBright     = 0xA0,
};

/// Generate a black and white palette.
std::unique_ptr<Palette> CAMOTO_GAMEGRAPHICS_API createPalette_DefaultMono();

/// Generate a CGA palette.
/**
 * @param cgaPal
 *   CGA palette to use.  One of the CGAPaletteType values.  The lower
 *   eight bits are used to represent the background colour.  If this is
 *   not the default black, it can be specified here, e.g. for a background
 *   colour of blue (colour #1) try (CGAPaletteType)(GreenRed | 1).  This
 *   only needs to be specified when the background colour is not black.
 */
std::unique_ptr<Palette> CAMOTO_GAMEGRAPHICS_API createPalette_CGA(
	CGAPaletteType cgaPal);

/// Allocate memory for a palette and fill it with CGA defaults.
/**
 * @return Palette with 16 entries suitable for drawing CGA and EGA images.
 */
std::unique_ptr<Palette> CAMOTO_GAMEGRAPHICS_API createPalette_FullCGA();

/// The default EGA palette is the same as the full CGA one.
#define createPalette_DefaultEGA createPalette_FullCGA

/// Allocate memory for a palette and fill it with EGA defaults.
/**
 * @return Palette with 64 entries suitable for drawing EGA images that
 *   make use of the EGA palette.
 */
std::unique_ptr<Palette> CAMOTO_GAMEGRAPHICS_API createPalette_FullEGA();

/// Generate the default VGA mode 13 palette.
std::unique_ptr<Palette> CAMOTO_GAMEGRAPHICS_API createPalette_DefaultVGA();

/// Convert a 6-bit VGA palette value (0-63) to 8-bit (0-255)
constexpr std::uint8_t pal_6to8(std::uint8_t six)
{
	return (six > 63) ? 255 : (six << 2) | (six >> 4);
}

/// Convert an 8-bit (0-255) palette value to 6-bit VGA (0-63)
constexpr std::uint8_t pal_8to6(std::uint8_t eight)
{
	return eight >> 2;
}


} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_GAMEGRAPHICS_PALETTE_HPP_
