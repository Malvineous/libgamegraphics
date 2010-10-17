/**
 * @file   palette.hpp
 * @brief  Declaration of Palette class, for accessing indexed colour maps used
 *         by game images.
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

#ifndef _CAMOTO_GAMEGRAPHICS_PALETTE_HPP_
#define _CAMOTO_GAMEGRAPHICS_PALETTE_HPP_

#include <boost/shared_ptr.hpp>
#include <vector>

#include <camoto/types.hpp>

namespace camoto {
namespace gamegraphics {

struct PaletteEntry {
	PaletteEntry();
	PaletteEntry(uint8_t red, uint8_t green, uint8_t blue);
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

/// Shared pointer to a Palette.
typedef std::vector<PaletteEntry> PaletteEntries;

/// Primary interface to palette data.
/**
 * This class represents palette data, either within an image or in an external
 * file.  Its functions are used to manipulate the contents of the palette.
 *
 * @note Multithreading: Only call one function in this class at a time.  Many
 *       of the functions seek around the underlying stream and thus will break
 *       if two or more functions are executing at the same time.
 */
class Palette {

	protected:
		PaletteEntries entries;

	public:

		Palette()
			throw ();

		virtual ~Palette()
			throw ();

		/// Get the maximum number of entries within this palette.
		/**
		 * @note The default implementation of this function returns zero.
		 *
		 * @return Maximum number of palette entries.  Zero means no maximum.
		 */
		virtual int getMaxEntries()
			throw ();

		/// Get a vector of all the entries in the palette.
		virtual const PaletteEntries& getEntries()
			throw ();

		/// Set all the entries in the palette.
		/**
		 * @precondition source.size() must be less than the value returned by
		 *   getMaxEntries()
		 */
		virtual void setEntries(const PaletteEntries& source)
			throw (std::ios::failure) = 0;

};

/// Shared pointer to a Palette.
typedef boost::shared_ptr<Palette> PalettePtr;

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_GAMEGRAPHICS_PALETTE_HPP_
