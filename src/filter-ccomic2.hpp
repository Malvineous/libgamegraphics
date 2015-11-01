/**
 * @file  filter-ccomic2.hpp
 * @brief Filter algorithm for compressing and expanding Captain Comic 2 images.
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

#ifndef _CAMOTO_FILTER_CCOMIC2_HPP_
#define _CAMOTO_FILTER_CCOMIC2_HPP_

#include <vector>
#include <camoto/config.hpp>
#include <camoto/filter.hpp>

namespace camoto {
namespace gamegraphics {

/// RLE expansion filter for Captain Comic images.
class CAMOTO_GAMEGRAPHICS_API filter_ccomic2_unrle: virtual public filter
{
	public:
		// Constructor
		/**
		 * @param lenHeader
		 *   Number of bytes at the start of the data to pass through unchanged.
		 */
		filter_ccomic2_unrle(unsigned int lenHeader);

		virtual void reset(stream::len lenInput);
		virtual void transform(uint8_t *out, stream::len *lenOut,
			const uint8_t *in, stream::len *lenIn);

	protected:
		unsigned int lenHeader; ///< Number of bytes to pass through unchanged
		uint8_t val;           ///< Previous byte read
		unsigned int repeat;   ///< How many times to repeat prev
		unsigned int escape;   ///< How many bytes to read unchanged
};

/// RLE compression filter for Captain Comic images.
class CAMOTO_GAMEGRAPHICS_API filter_ccomic2_rle: virtual public filter
{
	public:
		// Constructor
		/**
		 * @param lenHeader
		 *   Number of bytes at the start of the data to pass through unchanged.
		 */
		filter_ccomic2_rle(unsigned int lenHeader);

		virtual void reset(stream::len lenInput);
		virtual void transform(uint8_t *out, stream::len *lenOut,
			const uint8_t *in, stream::len *lenIn);

	protected:
		unsigned int lenHeader;   ///< Number of bytes to pass through unchanged
		stream::len totalWritten; ///< Total number of bytes written
		uint8_t val;              ///< Previous byte read
		unsigned int count;       ///< How many times to repeat prev (after any escapeBuf data)
		std::vector<uint8_t> escapeBuf; ///< Escaped bytes (written before count/val)
		unsigned int col;         ///< Current column (X pixel position) in output compressed data

		bool writeEscapeBuf(uint8_t*& out, stream::len& w, const stream::len *lenOut);
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_FILTER_CCOMIC2_HPP_
