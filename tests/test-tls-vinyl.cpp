/**
 * @file   test-tls-vinyl.cpp
 * @brief  Test code for VinylTileset class.
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

// Dimensions of sample tiles
#define DATA_TILE_WIDTH  16
#define DATA_TILE_HEIGHT 16

#define DATA_TILE_ONE \
	"\xC0\x00" \
	"\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00" \
	"\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00" \
	"\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00" \
	"\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00" \
	"\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00" \
	"\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00" \
	"\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00" \
	"\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00" \
	"\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00" \
	"\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00" \
	"\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00" \
	"\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00" \
	"\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00" \
	"\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00" \
	"\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00" \
	"\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00\x0F\x01\x00"
/*
#define DATA_TILE \
	"\xC0\x00" \
	"\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00\x0F\x00\x00" \
	"\x07\x01\x00\x00\x02\x00\x00\x02\x00\x0E\x03\x00" \
	"\x07\x01\x00\x00\x02\x00\x00\x02\x00\x0E\x03\x00" \
	"\x07\x01\x00\x00\x02\x00\x00\x02\x00\x0E\x03\x00" \
	"\x07\x01\x00\x00\x02\x00\x00\x02\x00\x0E\x03\x00" \
	"\x07\x01\x00\x00\x02\x00\x00\x02\x00\x0E\x03\x00" \
	"\x07\x01\x00\x00\x02\x00\x00\x02\x00\x0E\x03\x00" \
	"\x07\x01\x00\x00\x02\x00\x00\x02\x00\x0E\x03\x00" \
	"\x07\x01\x00\x00\x02\x00\x00\x02\x00\x0E\x03\x00" \
	"\x07\x01\x00\x00\x02\x00\x00\x02\x00\x0E\x03\x00" \
	"\x07\x01\x00\x00\x02\x00\x00\x02\x00\x0E\x03\x00" \
	"\x07\x01\x00\x00\x02\x00\x00\x02\x00\x0E\x03\x00" \
	"\x07\x01\x00\x00\x02\x00\x00\x02\x00\x0E\x03\x00" \
	"\x07\x01\x00\x00\x02\x00\x00\x02\x00\x0E\x03\x00" \
	"\x07\x01\x00\x00\x02\x00\x00\x02\x00\x0E\x03\x00" \
	"\x0F\x04\x00\x0F\x05\x00\x0F\x05\x00\x0F\x06\x00"
*/

#define DATA_TILE_TWO \
	"\xC0\x00" \
	"\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00" \
	"\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00" \
	"\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00" \
	"\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00" \
	"\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00" \
	"\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00" \
	"\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00" \
	"\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00" \
	"\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00" \
	"\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00" \
	"\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00" \
	"\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00" \
	"\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00" \
	"\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00" \
	"\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00" \
	"\x0F\x02\x00\x0F\x02\x00\x0F\x02\x00\x0F\x03\x00"

#define DATA_TILE_THREE \
	"\xC0\x00" \
	"\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00" \
	"\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00" \
	"\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00" \
	"\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00" \
	"\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00" \
	"\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00" \
	"\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00" \
	"\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00" \
	"\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00" \
	"\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00" \
	"\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00" \
	"\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00" \
	"\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00" \
	"\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00" \
	"\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00" \
	"\x0F\x04\x00\x0F\x04\x00\x0F\x04\x00\x0F\x05\x00"

#define DATA_TILE_FOUR \
	"\xC0\x00" \
	"\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00" \
	"\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00" \
	"\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00" \
	"\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00" \
	"\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00" \
	"\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00" \
	"\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00" \
	"\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00" \
	"\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00" \
	"\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00" \
	"\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00" \
	"\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00" \
	"\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00" \
	"\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00" \
	"\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00" \
	"\x0F\x06\x00\x0F\x06\x00\x0F\x06\x00\x0F\x07\x00"

#define LOOKUP_TABLE_LEN0 \
	"\x00\x00"
#define LOOKUP_TABLE_LEN1 \
	"\x08\x00"
#define LOOKUP_TABLE_LEN2 \
	"\x10\x00"
#define LOOKUP_TABLE_LEN3 \
	"\x18\x00"
#define LOOKUP_TABLE_LEN4 \
	"\x20\x00"
#define LOOKUP_TABLE_ONE \
	"\x01\x01\x01\x01" \
	"\x01\x01\x01\x0e"
#define LOOKUP_TABLE_TWO \
	"\x02\x02\x02\x02" \
	"\x02\x02\x02\x0e"
#define LOOKUP_TABLE_THREE \
	"\x03\x03\x03\x03" \
	"\x03\x03\x03\x0e"
#define LOOKUP_TABLE_FOUR \
	"\x04\x04\x04\x04" \
	"\x04\x04\x04\x0e"

#define test_tileset_initialstate \
	"\x02\x00" \
	DATA_TILE_ONE \
	DATA_TILE_TWO \
	LOOKUP_TABLE_LEN2 \
	LOOKUP_TABLE_ONE \
	LOOKUP_TABLE_TWO

#define test_tileset_insert_end \
	"\x03\x00" \
	DATA_TILE_ONE \
	DATA_TILE_TWO \
	DATA_TILE_THREE \
	LOOKUP_TABLE_LEN3 \
	LOOKUP_TABLE_ONE \
	LOOKUP_TABLE_TWO \
	LOOKUP_TABLE_THREE

#define test_tileset_insert_mid \
	"\x03\x00" \
	DATA_TILE_ONE \
	DATA_TILE_THREE \
	DATA_TILE_TWO \
	LOOKUP_TABLE_LEN3 \
	LOOKUP_TABLE_ONE \
	LOOKUP_TABLE_TWO \
	LOOKUP_TABLE_THREE

#define test_tileset_insert2 \
	"\x04\x00" \
	DATA_TILE_ONE \
	DATA_TILE_THREE \
	DATA_TILE_FOUR \
	DATA_TILE_TWO \
	LOOKUP_TABLE_LEN4 \
	LOOKUP_TABLE_ONE \
	LOOKUP_TABLE_TWO \
	LOOKUP_TABLE_THREE \
	LOOKUP_TABLE_FOUR

#define test_tileset_remove \
	"\x01\x00" \
	DATA_TILE_ONE \
	LOOKUP_TABLE_LEN1 \
	LOOKUP_TABLE_TWO

#define test_tileset_remove2 \
	"\x00\x00" \
	LOOKUP_TABLE_LEN0

#define test_tileset_insert_remove \
	"\x02\x00" \
	DATA_TILE_TWO \
	DATA_TILE_ONE \
	LOOKUP_TABLE_LEN2 \
	LOOKUP_TABLE_TWO \
	LOOKUP_TABLE_THREE

#define test_tileset_manip_zero \
	"\x03\x00" \
	DATA_TILE_TWO \
	DATA_TILE_ONE \
	DATA_TILE_THREE \
	LOOKUP_TABLE_LEN3 \
	LOOKUP_TABLE_TWO \
	LOOKUP_TABLE_ONE \
	LOOKUP_TABLE_THREE

#define test_tileset_overwrite_first \
	"\x02\x00" \
	DATA_TILE_TWO \
	DATA_TILE_ONE \
	LOOKUP_TABLE_LEN2 \
	LOOKUP_TABLE_TWO \
	LOOKUP_TABLE_FOUR

#define HAS_PAL
// 16 entries should be ok, the palette won't ever be used
#define test_tileset_PAL_initialstate \
	"\x00\x00\x00" "\x01\x01\x01" "\x02\x02\x02" "\x03\x03\x03" \
	"\x00\x00\x00" "\x01\x01\x01" "\x02\x02\x02" "\x03\x03\x03" \
	"\x00\x00\x00" "\x01\x01\x01" "\x02\x02\x02" "\x03\x03\x03" \
	"\x00\x00\x00" "\x01\x01\x01" "\x02\x02\x02" "\x03\x03\x03"

#define TILESET_CLASS tls_vinyl
#define TILESET_TYPE  "tls-vinyl"
#define TILESET_DETECTION_UNCERTAIN
#include "test-tileset.hpp"

// Test some invalid formats to make sure they're not identified as valid
// tilesets.  Note that they can still be opened though (by 'force'), this
// only checks whether they look like valid files or not.

// The "c00" test has already been performed in test-tileset.hpp to ensure the
// initial state is correctly identified as a valid tileset.

// File too short
ISINSTANCE_TEST(c01,
	"\x00\x00"
	,
	DefinitelyNo
);

// Too many tiles
ISINSTANCE_TEST(c02,
	"\xFF\x7F" \
	DATA_TILE_ONE \
	DATA_TILE_TWO \
	LOOKUP_TABLE_LEN2 \
	LOOKUP_TABLE_ONE \
	LOOKUP_TABLE_TWO
	,
	DefinitelyNo
);

// Data truncated
ISINSTANCE_TEST(c03,
	"\xFF\x00" \
	DATA_TILE_ONE \
	DATA_TILE_TWO \
	LOOKUP_TABLE_LEN2 \
	LOOKUP_TABLE_ONE \
	LOOKUP_TABLE_TWO
	,
	DefinitelyNo
);
