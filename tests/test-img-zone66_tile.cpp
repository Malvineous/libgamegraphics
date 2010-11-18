/**
 * @file   test-img-zone66_tile.cpp
 * @brief  Test code for conversion to and from Zone 66 tiles.
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

#include "tests.hpp"

#define TESTDATA_INITIAL_8x8 \
	"\x08\x00\x08\x00" \
	"\x08\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\xFE" \
	"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE" \
	"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE" \
	"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE" \
	"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE" \
	"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE" \
	"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE" \
	"\x08\x0C\x09\x09\x09\x09\x09\x09\x0A\xFE" \
	"\xFF"

#define TESTDATA_INITIAL_16x16 \
	"\x10\x00\x10\x00" \
	"\x10\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\xFE" \
	"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE" \
	"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE" \
	"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE" \
	"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE" \
	"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE" \
	"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE" \
	"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE" \
	"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE" \
	"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE" \
	"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE" \
	"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE" \
	"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE" \
	"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE" \
	"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE" \
	"\x10\x0C\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x0A\xFE" \
	"\xFF"

#define TESTDATA_INITIAL_9x9 \
	"\x09\x00\x09\x00" \
	"\x09\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\xFE" \
	"\x01\x0C" "\xFD\x07"           "\x01\x0A\xFE" \
	"\x01\x0C" "\xFD\x07"           "\x01\x0A\xFE" \
	"\x01\x0C" "\xFD\x07"           "\x01\x0A\xFE" \
	"\x01\x0C" "\xFD\x07"           "\x01\x0A\xFE" \
	"\x01\x0C" "\xFD\x07"           "\x01\x0A\xFE" \
	"\x01\x0C" "\xFD\x07"           "\x01\x0A\xFE" \
	"\x01\x0C" "\xFD\x07"           "\x01\x0A\xFE" \
	"\x09\x0C\x09\x09\x09\x09\x09\x09\x09\x0A\xFE" \
	"\xFF"

#define TESTDATA_INITIAL_8x4 \
	"\x08\x00\x04\x00" \
	"\x08\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\xFE" \
	"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE" \
	"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE" \
	"\x08\x0C\x09\x09\x09\x09\x09\x09\x0A\xFE" \
	"\xFF"

// This format doesn't support masks
#undef IMG_HAS_MASK

#define IMG_TYPE "img-zone66_tile"
#define IMG_CLASS img_zone66_tile
#include "test-img.hpp"

// Some custom tests for this particular format

// Exercise the various encoding types
#define TESTDATA_INITIAL_8x3 \
	"\x08\x00\x03\x00" \
	"\x04\x00\x01\x01\x01" "\xFE" \
	"\xFE" \
	"\x04\x02\x00\x00\x03" "\xFD\x03" "\x01\x04" "\xFE" \
	"\xFF"

const uint8_t stdformat_test_image_8x3[] = {
	0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x02, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04,
};
const uint8_t stdformat_test_mask_8x3[] = {
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
};

BOOST_FIXTURE_TEST_SUITE(SUITE_NAME, FIXTURE_NAME)
TO_STANDARD_TEST(8, 3);
FROM_STANDARD_TEST(8, 3)
BOOST_AUTO_TEST_SUITE_END()

// Make sure it works with a max-size format (no 'compression' at all)
#define TESTDATA_INITIAL_4x3 \
	"\x04\x00\x03\x00" \
	"\x04\x00\x01\x00\x01" "\xFE" \
	"\x04\x01\x00\x00\x01" "\xFE" \
	"\x04\x00\x01\x00\x01" "\xFE" \
	"\xFF"

const uint8_t stdformat_test_image_4x3[] = {
	0x00, 0x01, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x01,
	0x00, 0x01, 0x00, 0x01
};
const uint8_t stdformat_test_mask_4x3[] = {
	0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01
};

BOOST_FIXTURE_TEST_SUITE(SUITE_NAME, FIXTURE_NAME)
TO_STANDARD_TEST(4, 3);
FROM_STANDARD_TEST(4, 3)
BOOST_AUTO_TEST_SUITE_END()
