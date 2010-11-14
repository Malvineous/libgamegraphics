/**
 * @file   test-img-ccaves.cpp
 * @brief  Test code for CCavesGraphics class.
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

#define TESTDATA_IMAGE1_8x8 \
	"\xFF\xFF\xFF\xFF\xFF" \
	"\x81\x00\x01\x80\x81" \
	"\x81\x00\x01\x80\x81" \
	"\x81\x00\x01\x80\x81" \
	"\x81\x00\x01\x80\x81" \
	"\x81\x00\x01\x80\x81" \
	"\x81\x00\x01\x80\x81" \
	"\xFF\x7E\x01\x80\xFF"

#define TESTDATA_IMAGE2_8x8 \
	"\xFF\xFF\xFF\xFF\xFF" \
	"\xFF\xFF\xFF\xFF\xFF" \
	"\xFF\xFF\xFF\xFF\xFF" \
	"\xFF\xFF\xFF\xFF\xFF" \
	"\xFF\xFF\xFF\xFF\xFF" \
	"\xFF\xFF\xFF\xFF\xFF" \
	"\xFF\xFF\xFF\xFF\xFF" \
	"\xFF\xFF\xFF\xFF\xFF"

#define TESTDATA_IMAGE3_8x8 \
	"\xFF\x00\x00\x00\x00" \
	"\xFF\x00\x00\x00\x00" \
	"\xFF\x00\x00\x00\x00" \
	"\xFF\x00\x00\x00\x00" \
	"\xFF\x00\x00\x00\x00" \
	"\xFF\x00\x00\x00\x00" \
	"\xFF\x00\x00\x00\x00" \
	"\xFF\x00\x00\x00\x00"

#define testdata_initialstate \
	"\x02" "\x01" "\x08" \
	TESTDATA_IMAGE1_8x8 \
	TESTDATA_IMAGE2_8x8

#define testdata_overwrite \
	"\x02" "\x01" "\x08" \
	TESTDATA_IMAGE3_8x8 \
	TESTDATA_IMAGE2_8x8

#define testdata_insert_end \
	"\x03" "\x01" "\x08" \
	TESTDATA_IMAGE1_8x8 \
	TESTDATA_IMAGE2_8x8 \
	TESTDATA_IMAGE3_8x8

#define testdata_insert_middle \
	"\x03" "\x01" "\x08" \
	TESTDATA_IMAGE1_8x8 \
	TESTDATA_IMAGE3_8x8 \
	TESTDATA_IMAGE2_8x8

#define testdata_delete_first \
	"\x01" "\x01" "\x08" \
	TESTDATA_IMAGE2_8x8

/*
#define testdata_append_tileset \
	"\x02" "\x01" "\x08" \
	TESTDATA_IMAGE1_8x8 \
	TESTDATA_IMAGE2_8x8 \
	"\x01" "\x01" "\x08" \
	TESTDATA_IMAGE3_8x8 \
*/

// The test tiles are in the first subtileset
#define TEST_HAS_SUBTILESET

// The format supports adding new subtilesets
//#define TEST_CAN_ADD_SUBTILESET

#define TILESET_CLASS tls_ccaves
#define TILESET_TYPE  "tls-ccaves"
#include "test-tls.hpp"

// Test some invalid formats to make sure they're not identified as valid
// archives.  Note that they can still be opened though (by 'force'), this
// only checks whether they look like valid files or not.

// The "c00" test has already been performed in test-img.hpp to ensure the
// initial state is correctly identified as a valid archive.

/// Too many images (not enough image data)
ISINSTANCE_TEST(c01,
	// Count, width (bytes), height (pixels/rows)
	"\x03" "\x01" "\x08"
	TESTDATA_IMAGE1_8x8
	TESTDATA_IMAGE2_8x8
	,
	TilesetType::DefinitelyNo
);

/// Make sure an empty tileset doesn't cause a problem
ISINSTANCE_TEST(c02,
	// Count, width (bytes), height (pixels/rows)
	"\x00" "\x01" "\x08"
	"\x02" "\x01" "\x08"
	TESTDATA_IMAGE1_8x8
	TESTDATA_IMAGE2_8x8
	,
	TilesetType::PossiblyYes
);
