/**
 * @file   test-tls-zone66.cpp
 * @brief  Test code for Zone66Tileset class.
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

// Dimensions of sample tiles
#define DATA_TILE_WIDTH  8
#define DATA_TILE_HEIGHT 8

// Length 0x54
#define DATA_TILE_ONE \
	"\x08\x00\x08\x00" \
	"\x08\x01\x01\x01\x01\x01\x01\x01\x01\xFE" \
	"\x08\x01\x01\x01\x01\x01\x01\x01\x01\xFE" \
	"\x08\x01\x01\x01\x01\x01\x01\x01\x01\xFE" \
	"\x08\x01\x01\x01\x01\x01\x01\x01\x01\xFE" \
	"\x08\x01\x01\x01\x01\x01\x01\x01\x01\xFE" \
	"\x08\x01\x01\x01\x01\x01\x01\x01\x01\xFE" \
	"\x08\x01\x01\x01\x01\x01\x01\x01\x01\xFE" \
	"\x08\x01\x01\x01\x01\x01\x01\x01\x0e" \
	"\xFF"

// Length 0x54
#define DATA_TILE_TWO \
	"\x08\x00\x08\x00" \
	"\x08\x02\x02\x02\x02\x02\x02\x02\x02\xFE" \
	"\x08\x02\x02\x02\x02\x02\x02\x02\x02\xFE" \
	"\x08\x02\x02\x02\x02\x02\x02\x02\x02\xFE" \
	"\x08\x02\x02\x02\x02\x02\x02\x02\x02\xFE" \
	"\x08\x02\x02\x02\x02\x02\x02\x02\x02\xFE" \
	"\x08\x02\x02\x02\x02\x02\x02\x02\x02\xFE" \
	"\x08\x02\x02\x02\x02\x02\x02\x02\x02\xFE" \
	"\x08\x02\x02\x02\x02\x02\x02\x02\x0e" \
	"\xFF"

// Length 0x54
#define DATA_TILE_THREE \
	"\x08\x00\x08\x00" \
	"\x08\x03\x03\x03\x03\x03\x03\x03\x03\xFE" \
	"\x08\x03\x03\x03\x03\x03\x03\x03\x03\xFE" \
	"\x08\x03\x03\x03\x03\x03\x03\x03\x03\xFE" \
	"\x08\x03\x03\x03\x03\x03\x03\x03\x03\xFE" \
	"\x08\x03\x03\x03\x03\x03\x03\x03\x03\xFE" \
	"\x08\x03\x03\x03\x03\x03\x03\x03\x03\xFE" \
	"\x08\x03\x03\x03\x03\x03\x03\x03\x03\xFE" \
	"\x08\x03\x03\x03\x03\x03\x03\x03\x0e" \
	"\xFF"

// Length 0x54
#define DATA_TILE_FOUR \
	"\x08\x00\x08\x00" \
	"\x08\x04\x04\x04\x04\x04\x04\x04\x04\xFE" \
	"\x08\x04\x04\x04\x04\x04\x04\x04\x04\xFE" \
	"\x08\x04\x04\x04\x04\x04\x04\x04\x04\xFE" \
	"\x08\x04\x04\x04\x04\x04\x04\x04\x04\xFE" \
	"\x08\x04\x04\x04\x04\x04\x04\x04\x04\xFE" \
	"\x08\x04\x04\x04\x04\x04\x04\x04\x04\xFE" \
	"\x08\x04\x04\x04\x04\x04\x04\x04\x04\xFE" \
	"\x08\x04\x04\x04\x04\x04\x04\x04\x0e" \
	"\xFF"


#define test_tileset_initialstate \
	"\x02\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	"\x54\x00\x00\x00" \
	DATA_TILE_ONE \
	DATA_TILE_TWO

#define test_tileset_insert_end \
	"\x03\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	"\x54\x00\x00\x00" \
	"\xa8\x00\x00\x00" \
	DATA_TILE_ONE \
	DATA_TILE_TWO \
	DATA_TILE_THREE

#define test_tileset_insert_mid \
	"\x03\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	"\x54\x00\x00\x00" \
	"\xa8\x00\x00\x00" \
	DATA_TILE_ONE \
	DATA_TILE_THREE \
	DATA_TILE_TWO


#define test_tileset_insert2 \
	"\x04\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	"\x54\x00\x00\x00" \
	"\xa8\x00\x00\x00" \
	"\xfc\x00\x00\x00" \
	DATA_TILE_ONE \
	DATA_TILE_THREE \
	DATA_TILE_FOUR \
	DATA_TILE_TWO

#define test_tileset_remove \
	"\x01\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	DATA_TILE_TWO

#define test_tileset_remove2 \
	"\x00\x00\x00\x00"

#define test_tileset_insert_remove \
	"\x02\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	"\x54\x00\x00\x00" \
	DATA_TILE_THREE \
	DATA_TILE_TWO

#define TILESET_CLASS tls_zone66
#define TILESET_TYPE  "tls-zone66"
#include "test-tileset.hpp"

// Test some invalid formats to make sure they're not identified as valid
// tilesets.  Note that they can still be opened though (by 'force'), this
// only checks whether they look like valid files or not.

// The "c00" test has already been performed in test-tileset.hpp to ensure the
// initial state is correctly identified as a valid tileset.

// Test first file's offset != 0
ISINSTANCE_TEST(c01,
	"\x02\x00\x00\x00"
	"\x01\x00\x00\x00"
	"\x05\x00\x00\x00"
	DATA_TILE_ONE
	DATA_TILE_TWO,
	DefinitelyNo
);

// Test offset decreasing (resulting in a negative filesize)
ISINSTANCE_TEST(c02,
	"\x03\x00\x00\x00"
	"\x00\x00\x00\x00"
	"\x05\x00\x00\x00"
	"\x04\x00\x00\x00"
	DATA_TILE_ONE
	DATA_TILE_TWO
	DATA_TILE_TWO,
	DefinitelyNo
);

// Make sure all tiles are contained within the file
ISINSTANCE_TEST(c03,
	"\x02\x00\x00\x00"
	"\x00\x00\x00\x00"
	"\x05\x01\x00\x00"
	DATA_TILE_ONE
	DATA_TILE_TWO,
	DefinitelyNo
);

// Too short
ISINSTANCE_TEST(c04,
	"\x00\x00\x00",
	DefinitelyNo
);
