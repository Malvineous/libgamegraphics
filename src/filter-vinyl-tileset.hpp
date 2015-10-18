/**
 * @file  filter-vinyl-tileset.hpp
 * @brief Filter algorithm for compressing and expanding Vinyl tilesets.
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

#ifndef _CAMOTO_FILTER_VINYL_TILESET_HPP_
#define _CAMOTO_FILTER_VINYL_TILESET_HPP_

#include <deque>
#include <camoto/filter.hpp>
#include <camoto/stream_string.hpp>

namespace camoto {
namespace gamegraphics {

/// Length of a compressed solid tile, in bytes.
const unsigned int VGFM_CSOLID_LEN  = 0x80;

/// Length of a compressed masked tile, in bytes.
const unsigned int VGFM_CMASKED_LEN = 0xC0;

/// Length of a decompressed solid tile, in bytes.
const unsigned int VGFM_USOLID_LEN  = 0x100;

/// Length of a decompressed masked tile, in bytes.
const unsigned int VGFM_UMASKED_LEN = 0x140;

/// Decompression filter for Vinyl tilesets.
/**
 * This turns VGFM tileset files into a custom Camoto format which is easier
 * for manipulation - it allows us to use pixel values instead of lookup codes.
 */
class filter_vinyl_tileset_expand: virtual public filter
{
	public:
		virtual void reset(stream::len lenInput);
		virtual void transform(uint8_t *out, stream::len *lenOut,
			const uint8_t *in, stream::len *lenIn);

	protected:
		/// Decompress inData into outData
		void expand();

		/// All the input data, in VGFM tileset format
		std::unique_ptr<stream::string> inData;

		/// All the output data, after decompression
		std::unique_ptr<stream::string> outData;
};

/// Compression filter for Vinyl tilesets.
/**
 * This turns the custom Camoto format back into a VGFM tileset file, by
 * replacing the pixel values with Vinyl's lookup codes.
 */
class filter_vinyl_tileset_compress: virtual public filter
{
	public:
		virtual void reset(stream::len lenInput);
		virtual void transform(uint8_t *out, stream::len *lenOut,
			const uint8_t *in, stream::len *lenIn);

	protected:
		enum class Phase {
			P1_Count,      ///< Write the number of tiles
			P2_Tiles,      ///< Write the tile data, and remember the lookup codes
			P3_LookupLen,  ///< Write the number of lookup codes
			P4_LookupData, ///< Write the actual lookup codes themselves
		};

		/// Which phase of the process we are currently in?
		Phase phase;

		/// Number of tiles to write in Phase::P2_Tiles
		unsigned int numTiles;

		/// List of lookup codes to write in Phase::P4_LookupData
		std::deque<uint32_t> codes;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_FILTER_VINYL_TILESET_HPP_
