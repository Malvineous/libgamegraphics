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
	"\x08\x0C\x09\x09\x09\x09\x09\x09\x0A\xFE"

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
	"\x10\x0C\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x0A\xFE"

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
	"\x09\x0C\x09\x09\x09\x09\x09\x09\x09\x0A\xFE"

#define IMG_TYPE "img-zone66_tile"
#define IMG_CLASS img_zone66_tile
#include "test-img.hpp"
