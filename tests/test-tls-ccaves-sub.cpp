/**
 * @file   test-tls-ccaves.cpp
 * @brief  Test code for CCavesTiles class.
 *
 * Copyright (C) 2010-2011 Adam Nielsen <malvineous@shikadi.net>
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

#define DATA_TILE_ONE \
	"\xFF\xFF\x00\x00\x00" \
	"\xFF\xFF\x00\x00\x00" \
	"\xFF\xFF\x00\x00\x00" \
	"\xFF\xFF\x00\x00\x00" \
	"\xFF\xFF\x00\x00\x00" \
	"\xFF\xFF\x00\x00\x00" \
	"\xFF\xFF\x00\x00\x00" \
	"\xFF\xFE\x01\x01\x01"

#define DATA_TILE_TWO \
	"\xFF\x00\xFF\x00\x00" \
	"\xFF\x00\xFF\x00\x00" \
	"\xFF\x00\xFF\x00\x00" \
	"\xFF\x00\xFF\x00\x00" \
	"\xFF\x00\xFF\x00\x00" \
	"\xFF\x00\xFF\x00\x00" \
	"\xFF\x00\xFF\x00\x00" \
	"\xFF\x00\xFF\x01\x01"

#define DATA_TILE_THREE \
	"\xFF\xFF\xFF\x00\x00" \
	"\xFF\xFF\xFF\x00\x00" \
	"\xFF\xFF\xFF\x00\x00" \
	"\xFF\xFF\xFF\x00\x00" \
	"\xFF\xFF\xFF\x00\x00" \
	"\xFF\xFF\xFF\x00\x00" \
	"\xFF\xFF\xFF\x00\x00" \
	"\xFF\xFE\xFF\x01\x01"

#define DATA_TILE_FOUR \
	"\xFF\x00\x00\xFF\x00" \
	"\xFF\x00\x00\xFF\x00" \
	"\xFF\x00\x00\xFF\x00" \
	"\xFF\x00\x00\xFF\x00" \
	"\xFF\x00\x00\xFF\x00" \
	"\xFF\x00\x00\xFF\x00" \
	"\xFF\x00\x00\xFF\x00" \
	"\xFF\x00\x01\xFF\x01"


#define test_tileset_initialstate \
	"\x02" "\x01" "\x08" \
	DATA_TILE_ONE \
	DATA_TILE_TWO

#define test_tileset_insert_end \
	"\x03" "\x01" "\x08" \
	DATA_TILE_ONE \
	DATA_TILE_TWO \
	DATA_TILE_THREE

#define test_tileset_insert_mid \
	"\x03" "\x01" "\x08" \
	DATA_TILE_ONE \
	DATA_TILE_THREE \
	DATA_TILE_TWO


#define test_tileset_insert2 \
	"\x04" "\x01" "\x08" \
	DATA_TILE_ONE \
	DATA_TILE_THREE \
	DATA_TILE_FOUR \
	DATA_TILE_TWO

#define test_tileset_remove \
	"\x01" "\x01" "\x08" \
	DATA_TILE_TWO

#define test_tileset_remove2 \
	"\x00" "\x01" "\x08"

#define test_tileset_insert_remove \
	"\x02" "\x01" "\x08" \
	DATA_TILE_THREE \
	DATA_TILE_TWO

#define test_tileset_manip_zero \
	test_tileset_insert_end

// We can't always guarantee data is in this tileset type, so the best the
// isInstance function returns is PossiblyYes.  This #define allows for that.
#define TILESET_DETECTION_UNCERTAIN

#define TILESET_CLASS tls_ccaves_sub
#define TILESET_TYPE  "tls-ccaves-sub"
#include "test-tileset.hpp"

// Test some invalid formats to make sure they're not identified as valid
// archives.  Note that they can still be opened though (by 'force'), this
// only checks whether they look like valid files or not.

// The "c00" test has already been performed in test-img.hpp to ensure the
// initial state is correctly identified as a valid archive.

/// Too many images (not enough image data)
ISINSTANCE_TEST(c01,
	// Count, width (bytes), height (pixels/rows)
	"\x03" "\x01" "\x08"
	DATA_TILE_ONE
	DATA_TILE_TWO
	,
	DefinitelyNo
);

/// Too short
ISINSTANCE_TEST(c02,
	"\x00" "\x01"
	,
	DefinitelyNo
);
