/**
 * @file  filter-vinyl-tileset.cpp
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

#include <algorithm>
#include <cassert>
#include <vector>
#include <camoto/iostream_helpers.hpp>
#include <camoto/util.hpp> // make_unique
#include "filter-vinyl-tileset.hpp"

namespace camoto {
namespace gamegraphics {

void filter_vinyl_tileset_expand::reset(stream::len lenInput)
{
	this->inData = std::make_unique<stream::string>();
	this->inData->truncate(lenInput);
	this->inData->seekg(0, stream::start);
	this->outData.reset();
	return;
}

void filter_vinyl_tileset_expand::transform(uint8_t *out, stream::len *lenOut,
	const uint8_t *in, stream::len *lenIn)
{
	stream::len r = 0, w = 0;
	if (*lenIn > 0) {
		// More data to read
		this->inData->write(in, *lenIn);
		r += *lenIn;
	} else {
		// All data read, start writing output
		if (!this->outData) this->expand();
		w += this->outData->try_read(out, *lenOut);
	}
	*lenOut = w;
	*lenIn = r;
	return;
}

void filter_vinyl_tileset_expand::expand()
{
	this->inData->seekg(0, stream::start);
	this->outData = std::make_unique<stream::string>();

	uint16_t numTiles;
	*this->inData >> u16le(numTiles);
	*this->outData << u16le(numTiles);

	// Read in all the sizes of all the tiles
	std::vector<stream::pos> offsets;
	offsets.reserve(numTiles);
	stream::pos nextOffset = 2;
	for (int i = 0; i < numTiles; i++) {
		offsets.push_back(nextOffset);
		uint16_t size;
		*this->inData >> u16le(size);
		nextOffset += size + 2;
		this->inData->seekg(size, stream::cur);
	}

	// Read in the dictionary/code map
	uint16_t lenDict;
	*this->inData >> u16le(lenDict);
	std::vector<uint8_t> dict(lenDict);
	this->inData->read(dict.data(), lenDict);

	// Expand each tile
	for (auto offset : offsets) {
		this->inData->seekg(offset, stream::start);
		uint16_t inSize, outSize;
		*this->inData >> u16le(inSize);
		switch (inSize) {
			case VGFM_CSOLID_LEN: outSize = VGFM_USOLID_LEN; break;
			case VGFM_CMASKED_LEN: outSize = VGFM_UMASKED_LEN; break;
			default: throw stream::error("Unknown tile type (length is not that of "
				"a solid or a masked tile)");
		}
		*this->outData << u16le(outSize);
		for (int i = 0; i < inSize; i += 2) {
			if (inSize == VGFM_CMASKED_LEN) {
				// Copy the mask byte through unchanged
				uint8_t mask;
				*this->inData >> u8(mask);
				*this->outData << u8(mask);
				i++;
			}
			// Expand the 2-byte code into four bytes of pixels
			uint16_t code;
			*this->inData >> u16le(code);
			if (code * 4 > dict.size() - 4) code = 0;
			this->outData->write(&dict[code * 4], 4);
		}
	}
	this->outData->seekg(0, stream::start);
	return;
}

void filter_vinyl_tileset_compress::reset(stream::len lenInput)
{
	this->phase = Phase::P1_Count;
	this->numTiles = 0; // strictly unnecessary
	this->codes.clear();
	return;
}

void filter_vinyl_tileset_compress::transform(uint8_t *out, stream::len *lenOut,
	const uint8_t *in, stream::len *lenIn)
{
	stream::len r = 0, w = 0;

	switch (this->phase) {
		case Phase::P1_Count:
			if (*lenIn < 2) break;
			if (*lenOut < 2) break;

			// Copy the count across unchanged, and remember it for the next phase
			memcpy(out, in, 2);
			this->numTiles = in[0] | (in[1] << 8);
			r += 2;
			w += 2;
			if (this->numTiles == 0) {
				// Rare case where the tileset is empty
				this->phase = Phase::P3_LookupLen;
			} else {
				this->phase = Phase::P2_Tiles;
			}
			break;

		case Phase::P2_Tiles:
			for (;;) {
				if (*lenIn < 2) break; // not enough data to read the next tile length
				unsigned int lenTile = in[0] | (in[1] << 8);

				// Return if we can't read the whole tile in one go
				if (*lenIn < lenTile + 2) break;
				if (*lenOut < lenTile + 2) break; // technically this should be smaller but it shouldn't matter

				// We've read the length now, so write it too
				in += 2;
				r += 2;

				uint16_t outSize;
				switch (lenTile) {
					case VGFM_USOLID_LEN: outSize = VGFM_CSOLID_LEN; break;
					case VGFM_UMASKED_LEN: outSize = VGFM_CMASKED_LEN; break;
					default: throw stream::error("Unknown tile type (length is not that of "
						"a solid or a masked tile)");
				}
				*out++ =  outSize        & 0xFF;
				*out++ = (outSize >>  8) & 0xFF;
				w += 2;

				for (unsigned int i = 0; i < lenTile; i += 4) {
					if (lenTile == VGFM_UMASKED_LEN) {
						// Masked tile, copy mask byte through unchanged
						*out++ = *in++;
						r++;
						w++;
						i++;
					}
					// Replace four pixels with a two-byte code
					uint32_t pixels = in[0] | (in[1] << 8) | (in[2] << 16) | (in[3] << 24);
					uint16_t val = 0;
					auto it = std::find(this->codes.begin(), this->codes.end(), pixels);
					if (it == this->codes.end()) {
						// Code not found, add it
						val = this->codes.size();
						this->codes.push_back(pixels);
					} else {
						// Pixels already used, use that code
						val = it - this->codes.begin();
					}
					out[0] = val & 0xFF;
					out[1] = (val >> 8) & 0xFF;
					out += 2;
					w += 2;
					in += 4;
					r += 4;
				}
				this->numTiles--;
				if (this->numTiles == 0) {
					this->phase = Phase::P3_LookupLen;
					break;
				}
			}
			break;

		case Phase::P3_LookupLen: {
			if (*lenOut < 2) break;

			uint16_t len = this->codes.size() * 4;
			*out++ =  len        & 0xFF;
			*out++ = (len >>  8) & 0xFF;
			w += 2;
			this->phase = Phase::P4_LookupData;
			break;
		}

		case Phase::P4_LookupData: {
			auto count = std::min((int)*lenOut / 4, (int)this->codes.size());
			while (count--) {
				auto code = this->codes.front();
				*out++ =  code        & 0xFF;
				*out++ = (code >>  8) & 0xFF;
				*out++ = (code >> 16) & 0xFF;
				*out++ = (code >> 24) & 0xFF;
				w += 4;
				this->codes.pop_front();
			}
			break;
		}
	}
	*lenOut = w;
	*lenIn = r;
	return;
}

} // namespace gamegraphics
} // namespace camoto
