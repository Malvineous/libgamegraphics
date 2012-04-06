/**
 * @file  filter-ccomic.hpp
 * @brief Filter algorithm for compressing and expanding Captain Comic images.
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

#ifndef _CAMOTO_FILTER_CCOMIC_HPP_
#define _CAMOTO_FILTER_CCOMIC_HPP_

#include <vector>
#include <camoto/filter.hpp>

namespace camoto {
namespace gamegraphics {

/// RLE expansion filter for Captain Comic images.
class filter_ccomic_unrle: public filter
{
	public:
		filter_ccomic_unrle()
			throw ();

		void transform(uint8_t *out, stream::len *lenOut,
			const uint8_t *in, stream::len *lenIn)
			throw (filter_error);

	protected:
		unsigned int lenBlock; ///< How many bytes left to output in current block
		uint8_t val;           ///< Previous byte read
		unsigned int repeat;   ///< How many times to repeat prev
		unsigned int escape;   ///< How many bytes to read unchanged
};

/// RLE compression filter for Captain Comic images.
class filter_ccomic_rle: public filter
{
	public:
		filter_ccomic_rle()
			throw ();

		void transform(uint8_t *out, stream::len *lenOut,
			const uint8_t *in, stream::len *lenIn)
			throw (filter_error);

	protected:
		uint8_t val;        ///< Previous byte read
		unsigned int count; ///< How many times to repeat prev (after any escapeBuf data)
		std::vector<uint8_t> escapeBuf; ///< Escaped bytes (written before count/val)
		bool writtenSize;   ///< Have we written the first two bytes yet?
		unsigned int col;   ///< Current column (X pixel position) in output compressed data

		bool writeEscapeBuf(uint8_t*& out, stream::len& w, const stream::len *lenOut)
			throw ();
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_FILTER_CCOMIC_HPP_
