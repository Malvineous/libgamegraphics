/**
 * @file  filter-pad.cpp
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

#include "filter-pad.hpp"

namespace camoto {
namespace gamegraphics {

filter_pad::filter_pad(std::string padData, stream::len lenPadPos)
	:	padData(padData),
		lenPadPos(lenPadPos)
{
}

void filter_pad::reset(stream::len lenInput)
{
	this->lenProcessed = 0;
	return;
}

void filter_pad::transform(uint8_t *out, stream::len *lenOut,
	const uint8_t *in, stream::len *lenIn)
{
	stream::len r = 0, w = 0;

	if (this->lenProcessed >= this->lenPadPos) {
		// Do padding now
		unsigned long lenPadData = this->padData.size();
		if (lenPadData > *lenOut) {
			throw stream::error("filter_pad: Buffer too small for padding data");
		}
		memcpy(out, this->padData.data(), lenPadData);
		w += lenPadData;
		*lenOut -= lenPadData;
		out += lenPadData;
		this->lenProcessed = 0;
	}

	stream::len lenCopy;
	if (this->lenProcessed + *lenIn > this->lenPadPos) {
		lenCopy = this->lenPadPos - this->lenProcessed;
	} else {
		lenCopy = *lenIn;
	}
	if (lenCopy > *lenOut) lenCopy = *lenOut;
	memcpy(out, in, lenCopy);
	r += lenCopy;
	w += lenCopy;
	this->lenProcessed += lenCopy;

	*lenOut = w;
	*lenIn = r;
	return;
}


filter_unpad::filter_unpad(stream::len lenPadData, stream::len lenPadPos)
	:	lenPadData(lenPadData),
		lenPadPos(lenPadPos)
{
}

void filter_unpad::reset(stream::len lenInput)
{
	this->lenProcessed = 0;
	return;
}

void filter_unpad::transform(uint8_t *out, stream::len *lenOut,
	const uint8_t *in, stream::len *lenIn)
{
	stream::len r = 0, w = 0;

	if (this->lenProcessed >= this->lenPadPos) {
		// Remove padding now
		if (this->lenPadData > *lenIn) {
			throw stream::error("filter_unpad: Input buffer smaller than data to remove");
		}
		// Discard this data
		r += this->lenPadData;
		*lenIn -= this->lenPadData;
		in += this->lenPadData;
		this->lenProcessed = 0;
	}

	stream::len lenCopy;
	if (this->lenProcessed + *lenIn > this->lenPadPos) {
		lenCopy = this->lenPadPos - this->lenProcessed;
	} else {
		lenCopy = *lenIn;
	}
	if (lenCopy > *lenOut) lenCopy = *lenOut;
	memcpy(out, in, lenCopy);
	r += lenCopy;
	w += lenCopy;
	this->lenProcessed += lenCopy;

	*lenOut = w;
	*lenIn = r;
	return;
}


} // namespace gamegraphics
} // namespace camoto
