/**
 * @file   test-tls-bash-sprite.cpp
 * @brief  Test code for Monster Bash sprites.
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

//
// Make sure the pixel data is decoded correctly
//
#include <camoto/gamegraphics/image.hpp>
#include "../src/img-bash-sprite.hpp"

using namespace camoto::gamegraphics;
using namespace camoto;

// Dimensions of sample tiles
#define DATA_TILE_WIDTH  8
#define DATA_TILE_HEIGHT 8

#define DATA_TILE_ONE_LEN   "\x3A\x00"
#define DATA_TILE_ONE \
	"\x00\x08\x08\x00" \
	"\x01\x00\x00\x00\x00\x00\x01\x00" \
	"\x01\x00\x00\x00\x00\x00\x00\x00\x00" \
	"\x01\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFE" \
	"\x02\x00\x00\x00\x00\x00\x00\x00\x01" \
	"\x04\x00\x00\x00\x00\x00\x00\x00\x01" \
	"\x08\x00\x00\x00\x00\x00\x00\x00\x01" \
	"\x00"

#define DATA_TILE_TWO_LEN   "\x3A\x00"
#define DATA_TILE_TWO \
	"\x00\x08\x08\x00" \
	"\x02\x00\x00\x00\x00\x00\x02\x00" \
	"\x01\x00\x00\x00\x00\x00\x00\x00\x00" \
	"\x01\x00\x00\x00\x00\x00\x00\x00\x00" \
	"\x02\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF" \
	"\x04\x00\x00\x00\x00\x00\x00\x00\x01" \
	"\x08\x00\x00\x00\x00\x00\x00\x00\x01" \
	"\x00"

#define DATA_TILE_THREE_LEN "\x3A\x00"
#define DATA_TILE_THREE \
	"\x00\x08\x08\x00" \
	"\x03\x00\x00\x00\x00\x00\x03\x00" \
	"\x01\x00\x00\x00\x00\x00\x00\x00\x00" \
	"\x01\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFE" \
	"\x02\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF" \
	"\x04\x00\x00\x00\x00\x00\x00\x00\x01" \
	"\x08\x00\x00\x00\x00\x00\x00\x00\x01" \
	"\x00"

#define DATA_TILE_FOUR_LEN  "\x3A\x00"
#define DATA_TILE_FOUR \
	"\x00\x08\x08\x00" \
	"\x04\x00\x00\x00\x00\x00\x04\x00" \
	"\x01\x00\x00\x00\x00\x00\x00\x00\x00" \
	"\x01\x00\x00\x00\x00\x00\x00\x00\x00" \
	"\x02\x00\x00\x00\x00\x00\x00\x00\x01" \
	"\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF" \
	"\x08\x00\x00\x00\x00\x00\x00\x00\x01" \
	"\x00"

#define test_tileset_initialstate \
	"\xFF" \
	DATA_TILE_ONE_LEN \
	DATA_TILE_ONE \
	DATA_TILE_TWO_LEN \
	DATA_TILE_TWO

#define test_tileset_insert_end \
	"\xFF" \
	DATA_TILE_ONE_LEN \
	DATA_TILE_ONE \
	DATA_TILE_TWO_LEN \
	DATA_TILE_TWO \
	DATA_TILE_THREE_LEN \
	DATA_TILE_THREE

#define test_tileset_insert_mid \
	"\xFF" \
	DATA_TILE_ONE_LEN \
	DATA_TILE_ONE \
	DATA_TILE_THREE_LEN \
	DATA_TILE_THREE \
	DATA_TILE_TWO_LEN \
	DATA_TILE_TWO

#define test_tileset_insert2 \
	"\xFF" \
	DATA_TILE_ONE_LEN \
	DATA_TILE_ONE \
	DATA_TILE_THREE_LEN \
	DATA_TILE_THREE \
	DATA_TILE_FOUR_LEN \
	DATA_TILE_FOUR \
	DATA_TILE_TWO_LEN \
	DATA_TILE_TWO

#define test_tileset_remove \
	"\xFF" \
	DATA_TILE_TWO_LEN \
	DATA_TILE_TWO

#define test_tileset_remove2 \
	"\xFF"

#define test_tileset_insert_remove \
	"\xFF" \
	DATA_TILE_THREE_LEN \
	DATA_TILE_THREE \
	DATA_TILE_TWO_LEN \
	DATA_TILE_TWO

#define test_tileset_manip_zero \
	test_tileset_insert_end

#define TILESET_CLASS tls_bash_sprite
#define TILESET_TYPE  "tls-bash-sprite"
#include "test-tileset.hpp"

// Test some invalid formats to make sure they're not identified as valid
// tilesets.  Note that they can still be opened though (by 'force'), this
// only checks whether they look like valid files or not.

// The "c00" test has already been performed in test-tileset.hpp to ensure the
// initial state is correctly identified as a valid tileset.

// Minimum file length
ISINSTANCE_TEST(c01,
	"\xFF"
	,
	DefinitelyYes
);

// Correct fields, but less than minimum file length
ISINSTANCE_TEST(c02,
	"\xFF"
	"\x00\x0B"
	"\x00\x08\x08\x00"
	"\x00\x00\x00\x00\x08\x00\x08"
	,
	DefinitelyNo
);

// Bad signature byte
ISINSTANCE_TEST(c03,
	"\xFE"
	DATA_TILE_ONE_LEN
	DATA_TILE_ONE
	,
	DefinitelyNo
);

// Correct fields, but less than minimum image length
ISINSTANCE_TEST(c04,
	"\xFF"
	DATA_TILE_ONE_LEN
	DATA_TILE_ONE
	"\x00\x0B"
	"\x00\x08\x08\x00"
	"\x00\x00\x00\x00\x08\x00\x08"
	,
	DefinitelyNo
);

// Test image goes past EOF
ISINSTANCE_TEST(c05,
	"\xFF"
	DATA_TILE_ONE_LEN
	DATA_TILE_ONE
	"\xFF\x00"
	DATA_TILE_TWO,
	DefinitelyNo
);

#define TEST_C06 \
	"\xFF" \
	DATA_TILE_ONE_LEN \
	"\x00\x08\x08\x00" \
	"\x00\x00\x00\x00\x08\x00\x08\x00" \
	"\x01\x00\x00\x00\x00\x00\x00\x00\x00" \
	"\x01\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFE" \
	"\x02\x00\x00\x00\x00\x00\x00\x00\x01" \
	"\x04\x00\x00\x00\x00\x00\x00\x00\x01" \
	"\x08\x00\x00\x00\x00\x00\x00\x00\x01"

// Last plane ID byte is zero (confirm rest of data is good)
ISINSTANCE_TEST(c06a,
	TEST_C06
	"\x00"
	,
	DefinitelyYes
);

// Last plane ID byte is not zero
ISINSTANCE_TEST(c06b,
	TEST_C06
	"\x01"
	,
	DefinitelyNo
);
