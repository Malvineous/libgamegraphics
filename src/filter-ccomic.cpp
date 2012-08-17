/**
 * @file  filter-ccomic.cpp
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

#include "filter-ccomic.hpp"

namespace camoto {
namespace gamegraphics {

/// Largest RLE length
const unsigned int MAX_RLE_COUNT = 0x7F;

/// Largest number of continuous escaped bytes
const unsigned int MAX_ESCAPE_LEN = 0x7F;

/// Length of each plane in input (1bpp) data
const unsigned int PLANE_LEN = 8000;

filter_ccomic_unrle::filter_ccomic_unrle()
	:	lenBlock(0),
		repeat(0),
		escape(0)
{
}

void filter_ccomic_unrle::transform(uint8_t *out, stream::len *lenOut,
	const uint8_t *in, stream::len *lenIn)
{
	stream::len r = 0, w = 0;

	if ((lenBlock == 0) && (*lenIn != 0)) {
		// Need to read block size at start
		if (*lenIn - r < 2) throw filter_error("No room to read plane size");
		this->lenBlock = *in++;
		this->lenBlock |= (*in++) << 8;
		r += 2;
		this->lenBlock *= 4; // number of planes
	}

	// While there's more space to write, and either more data to read or
	// more data to write
	while (               // while there is...
		(w < *lenOut)       // more space to write into, and
		&& (
			(r+1 < *lenIn)    // more data to read, or
			|| (this->repeat) // more data to write without reading any more
			|| (
				this->escape    // or there is escaped data
				&& (r < *lenIn) // and one last byte in the buffer
			)
		)
		&& lenBlock         // and the format says there's data to read
	) {

		// Write out any repeats we've got stored up
		while ((w < *lenOut) && this->repeat && lenBlock) {
			*out++ = this->val;
			this->repeat--;
			this->lenBlock--;
			w++;
		}

		// Write out any escaped data
		while ((r < *lenIn) && (w < *lenOut) && this->escape && lenBlock) {
			*out++ = *in++;
			this->escape--;
			this->lenBlock--;
			r++;
			w++;
		}

		// Loop while there's no bytes to write but more to read
		while ((this->repeat == 0) && (this->escape == 0) && (r < *lenIn - 1) && lenBlock) {
			if (*in & 0x80) { // RLE trigger
				this->repeat = (*in++) & 0x7F;
				this->val = *in++;
				r += 2;
			} else { // escaped byte
				this->escape = *in++;
				r++;
			}
		}

	}
	*lenOut = w;
	*lenIn = r;
	return;
}

filter_ccomic_rle::filter_ccomic_rle()
	:	val(0),
		count(0),
		writtenSize(false),
		col(0)
{
}

bool filter_ccomic_rle::writeEscapeBuf(uint8_t*& out, stream::len& w, const stream::len *lenOut)
{
	if (this->escapeBuf.size()) {
		// If there's not enough space to write it now, wait until we're
		// called again with an empty buffer.
		if (w + this->escapeBuf.size() + 1 > *lenOut) return false;
		assert(this->escapeBuf.size() <= MAX_ESCAPE_LEN);

		unsigned int len;
		if (((this->col % PLANE_LEN) + this->escapeBuf.size()) > PLANE_LEN) {
			len = PLANE_LEN - (this->col % PLANE_LEN);
		} else {
			len = this->escapeBuf.size();
		}
		*out++ = (uint8_t)len;
		w++;
		std::vector<uint8_t>::iterator itStart = this->escapeBuf.begin();
		std::vector<uint8_t>::iterator itEnd = itStart + len;
		for (std::vector<uint8_t>::const_iterator
			i = itStart; i != itEnd; i++
		) {
			*out++ = *i;
			w++;
		}
		this->col += len;
		this->escapeBuf.erase(itStart, itEnd);
		this->escapeBuf.clear();
	}
	return true;
}

void filter_ccomic_rle::transform(uint8_t *out, stream::len *lenOut,
	const uint8_t *in, stream::len *lenIn)
{
	stream::len r = 0, w = 0;
	if (!this->writtenSize) {
		// Write 8000 as a UINT16LE at the start of the file
		assert(*lenOut > 2); // will always be >= 4096 at this point
		*out++ = 0x40;
		*out++ = 0x1F;
		w += 2;
		this->writtenSize = true;
	}
	while (
		(w + 2 < *lenOut) && ( // while there's enough room to write two or more bytes, and
			(r < *lenIn)         // there's more data to read
			|| (
				(*lenIn == 0)      // or there's no more data to read...
				&& (
					(this->count)     // ...but there's a repeated byte to write
					|| (escapeBuf.size() > 0) // or there's escaped data to write
				)
			)
		)
	) {
		if ((*lenIn != 0) && (*in == this->val) && (this->count < MAX_RLE_COUNT)) {
			this->count++;
			in++;
			r++;
		} else {
			// byte changed, no more input data or RLE count at max

			if ((this->count == 2) && (this->escapeBuf.size())) {
				// If there are only two repeated bytes and there's already escape data,
				// append them to the escape data as that's more efficient.
				this->escapeBuf.push_back(this->val);
				this->escapeBuf.push_back(this->val);
				this->count = 0;
			} else if (this->count > 1) {
				// More than one repeated byte, write an RLE code
				if (!this->writeEscapeBuf(out, w, lenOut)) break;
				if (*lenOut - w < 2) break; // need more data to continue
				assert(this->escapeBuf.size() == 0);
				assert(this->count <= MAX_RLE_COUNT);

				if (((this->col % PLANE_LEN) + this->count) > PLANE_LEN) {
					// This RLE code would run across a scanline boundary, so split it
					// into two RLE codes - one for the top scanline and one for the
					// bottom one.
					unsigned int first = PLANE_LEN - (this->col % PLANE_LEN);
					assert(first > 0);
					*out++ = 0x80 | (uint8_t)first;
					*out++ = this->val;
					w += 2;
					this->col += first;
					this->count -= first;
					// Don't write out the rest of the repeated values now, because we
					// might be able to add to them and write out a larger number later.
					continue;
				}
				*out++ = 0x80 | (uint8_t)this->count;
				*out++ = this->val;
				w += 2;
				this->col += this->count;
				this->count = 0;
			}

			// This byte is different to the last one, but the last one wasn't
			// a repeat, OR we've run out of input data, OR the RLE count was at
			// max and is now at zero.
			assert(this->count <= 1);
			if (this->count) {
				if (this->escapeBuf.size() > MAX_ESCAPE_LEN - 1) {
					if (!this->writeEscapeBuf(out, w, lenOut)) break;
				}
				this->escapeBuf.push_back(this->val);
				// this->count should be set to 0 now, but there's no need
			}
			if (*lenIn - r > 0) {
				// There's more input data, get the next byte
				this->val = *in++;
				this->count = 1;
				r++;
			} else {
				// No more input data
				this->count = 0;
			}

			// Write out the escape buffer if it's reached maximum size, or
			// there's no more data to read.
			if (*lenIn == 0) {
				assert(this->count <= 1);
				if (!this->writeEscapeBuf(out, w, lenOut)) break;
			}

		}
	}

	*lenOut = w;
	*lenIn = r;
	return;
}

} // namespace gamegraphics
} // namespace camoto
