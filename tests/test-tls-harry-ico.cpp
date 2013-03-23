/**
 * @file   test-tls-harry-ico.cpp
 * @brief  Test code for HarryICOTileset class.
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
#define DATA_TILE_WIDTH  2
#define DATA_TILE_HEIGHT 3

#define DATA_TILE_ONE \
	"\x02\x00\x03\x00" \
	"\x01\x01" \
	"\x01\x01" \
	"\x01\x0e"

#define DATA_TILE_TWO \
	"\x02\x00\x03\x00" \
	"\x02\x02" \
	"\x02\x02" \
	"\x02\x0e"

#define DATA_TILE_THREE \
	"\x02\x00\x03\x00" \
	"\x03\x03" \
	"\x03\x03" \
	"\x03\x0e"

#define DATA_TILE_FOUR \
	"\x02\x00\x03\x00" \
	"\x04\x04" \
	"\x04\x04" \
	"\x04\x0e"


#define test_tileset_initialstate \
	DATA_TILE_ONE \
	DATA_TILE_TWO

#define test_tileset_insert_end \
	DATA_TILE_ONE \
	DATA_TILE_TWO \
	DATA_TILE_THREE

#define test_tileset_insert_mid \
	DATA_TILE_ONE \
	DATA_TILE_THREE \
	DATA_TILE_TWO

#define test_tileset_insert2 \
	DATA_TILE_ONE \
	DATA_TILE_THREE \
	DATA_TILE_FOUR \
	DATA_TILE_TWO

#define test_tileset_remove \
	DATA_TILE_TWO

#define test_tileset_remove2 \
	""

#define test_tileset_insert_remove \
	DATA_TILE_THREE \
	DATA_TILE_TWO

#define TILESET_CLASS tls_harry_ico
#define TILESET_TYPE  "tls-harry-ico"
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

// Header cut off
ISINSTANCE_TEST(c02,
	DATA_TILE_ONE
	DATA_TILE_TWO
	"\x00\x00"
	,
	DefinitelyNo
);

// Data past EOF
ISINSTANCE_TEST(c03,
	DATA_TILE_ONE
	DATA_TILE_TWO
	"\x02\x00\x03\x00"
	"\x05\x05\x05"
	"\x05\x05"
	,
	DefinitelyNo
);
