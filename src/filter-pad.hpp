/**
 * @file  filter-pad.hpp
 * @brief Filter algorithm for inserting and removing padding data.
 *
 * Copyright (C) 2010-2013 Adam Nielsen <malvineous@shikadi.net>
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

#ifndef _CAMOTO_FILTER_PAD_HPP_
#define _CAMOTO_FILTER_PAD_HPP_

#include <vector>
#include <camoto/filter.hpp>

namespace camoto {
namespace gamegraphics {

/// Filter that adds extra padding data every fixed number of bytes.
class filter_pad: virtual public filter
{
	public:
		/// Constructor.
		/**
		 * @param padData
		 *   Data to pad with.  This string is only ever added in one go, so it must
		 *   be less than the block size used for transforms (typically 4kB).
		 *
		 * @param lenPadPos
		 *   How many bytes between padding.  The pad data is inserted after this
		 *   number of bytes have been read.
		 */
		filter_pad(std::string padData, stream::len lenPadPos);

		virtual void reset(stream::len lenInput);
		virtual void transform(uint8_t *out, stream::len *lenOut,
			const uint8_t *in, stream::len *lenIn);

	protected:
		stream::len lenProcessed; ///< How many bytes left to output in current block
		std::string padData;      ///< What data to pad with
		stream::len lenPadPos;    ///< How many bytes between pads
};

/// Filter that removes extra padding data every fixed number of bytes.
class filter_unpad: virtual public filter
{
	public:
		/// Constructor.
		/**
		 * @param lenPadData
		 *   Number of bytes to remove.  The data is only ever removed in one go, so
		 *   it must be less than the block size used for transforms (typically 4kB)
		 *
		 * @param lenPadPos
		 *   How many bytes between padding.  The pad data is removed after every
		 *   lot of this number of bytes have been read.
		 */
		filter_unpad(stream::len lenPadData, stream::len lenPadPos);

		virtual void reset(stream::len lenInput);
		virtual void transform(uint8_t *out, stream::len *lenOut,
			const uint8_t *in, stream::len *lenIn);

	protected:
		stream::len lenProcessed; ///< How many bytes left to output in current block
		stream::len lenPadData;   ///< Number of padding bytes to remove
		stream::len lenPadPos;    ///< How many bytes between pads
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_FILTER_PAD_HPP_
