/**
 * @file   test-img-pic-raptor.cpp
 * @brief  Test code for Raptor PIC images.
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

#include <camoto/gamegraphics/image.hpp>

using namespace camoto::gamegraphics;
using namespace camoto;

#define TESTDATA_INITIAL_8x8 \
	"\x01\x00\x00\x00" "\x01\x00\x00\x00" "\x00\x00\x00\x00" \
	"\x08\x00\x00\x00" "\x08\x00\x00\x00" \
	"\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F" \
	"\x0C\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x09\x09\x09\x09\x09\x09\x0A"

#define TESTDATA_INITIAL_16x16 \
	"\x01\x00\x00\x00" "\x01\x00\x00\x00" "\x00\x00\x00\x00" \
	"\x10\x00\x00\x00" "\x10\x00\x00\x00" \
	"\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F" \
	"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x0A"

#define TESTDATA_INITIAL_9x9 \
	"\x01\x00\x00\x00" "\x01\x00\x00\x00" "\x00\x00\x00\x00" \
	"\x09\x00\x00\x00" "\x09\x00\x00\x00" \
	"\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F" \
	"\x0C\x00\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x00\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x00\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x00\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x00\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x00\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x00\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x09\x09\x09\x09\x09\x09\x09\x0A"

#define TESTDATA_INITIAL_8x4 \
	"\x01\x00\x00\x00" "\x01\x00\x00\x00" "\x00\x00\x00\x00" \
	"\x08\x00\x00\x00" "\x04\x00\x00\x00" \
	"\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F" \
	"\x0C\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x00\x00\x00\x00\x00\x00\x0A" \
	"\x0C\x09\x09\x09\x09\x09\x09\x0A"

#define IMG_TYPE "img-pic-raptor"
#define IMG_CLASS img_pic_raptor
#include "test-img.hpp"

// Test some invalid formats to make sure they're not identified as valid
// files.  Note that they can still be opened though (by 'force'), this
// only checks whether they look like valid files or not.

ISINSTANCE_TEST(c00, TESTDATA_INITIAL_8x8, DefinitelyYes);

// Image dimensions larger than available image data
ISINSTANCE_TEST(c01,
	"\x01\x00\x00\x00" "\x01\x00\x00\x00" "\x00\x00\x00\x00"
	"\x08\x00\x00\x00" "\x09\x00\x00\x00"
	"\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F"
	"\x0C\x00\x00\x00\x00\x00\x00\x0A"
	"\x0C\x00\x00\x00\x00\x00\x00\x0A"
	"\x0C\x00\x00\x00\x00\x00\x00\x0A"
	"\x0C\x00\x00\x00\x00\x00\x00\x0A"
	"\x0C\x00\x00\x00\x00\x00\x00\x0A"
	"\x0C\x00\x00\x00\x00\x00\x00\x0A"
	"\x0C\x09\x09\x09\x09\x09\x09\x0A"
	,
	DefinitelyNo
);
