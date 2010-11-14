/**
 * @file   test-img-ega-byteplanar.cpp
 * @brief  Test code for conversion to and from byte-planar EGA data.
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

#include <camoto/gamegraphics/image.hpp>
#include "../src/img-ega-byteplanar.hpp"

using namespace camoto::gamegraphics;
using namespace camoto;

#define TESTDATA_INITIAL_8x8 \
	"\xFF\xFF\xFF\xFF\xFF" \
	"\x81\x00\x01\x80\x81" \
	"\x81\x00\x01\x80\x81" \
	"\x81\x00\x01\x80\x81" \
	"\x81\x00\x01\x80\x81" \
	"\x81\x00\x01\x80\x81" \
	"\x81\x00\x01\x80\x81" \
	"\xFF\x7E\x01\x80\xFF"

#define TESTDATA_INITIAL_16x16 \
	"\xFF\xFF\xFF\xFF\xFF" "\xFF\xFF\xFF\xFF\xFF" \
	"\x80\x00\x00\x80\x80" "\x01\x00\x01\x00\x01" \
	"\x80\x00\x00\x80\x80" "\x01\x00\x01\x00\x01" \
	"\x80\x00\x00\x80\x80" "\x01\x00\x01\x00\x01" \
	"\x80\x00\x00\x80\x80" "\x01\x00\x01\x00\x01" \
	"\x80\x00\x00\x80\x80" "\x01\x00\x01\x00\x01" \
	"\x80\x00\x00\x80\x80" "\x01\x00\x01\x00\x01" \
	"\x80\x00\x00\x80\x80" "\x01\x00\x01\x00\x01" \
	"\x80\x00\x00\x80\x80" "\x01\x00\x01\x00\x01" \
	"\x80\x00\x00\x80\x80" "\x01\x00\x01\x00\x01" \
	"\x80\x00\x00\x80\x80" "\x01\x00\x01\x00\x01" \
	"\x80\x00\x00\x80\x80" "\x01\x00\x01\x00\x01" \
	"\x80\x00\x00\x80\x80" "\x01\x00\x01\x00\x01" \
	"\x80\x00\x00\x80\x80" "\x01\x00\x01\x00\x01" \
	"\x80\x00\x00\x80\x80" "\x01\x00\x01\x00\x01" \
	"\xFF\x7F\x00\x80\xFF" "\xFF\xFE\x01\x00\xFF"

#define TESTDATA_INITIAL_9x9 \
	"\xFF\xFF\xFF\xFF\xFF" "\x80\x80\x80\x80\x80" \
	"\x80\x00\x00\x80\x80" "\x80\x00\x80\x00\x80" \
	"\x80\x00\x00\x80\x80" "\x80\x00\x80\x00\x80" \
	"\x80\x00\x00\x80\x80" "\x80\x00\x80\x00\x80" \
	"\x80\x00\x00\x80\x80" "\x80\x00\x80\x00\x80" \
	"\x80\x00\x00\x80\x80" "\x80\x00\x80\x00\x80" \
	"\x80\x00\x00\x80\x80" "\x80\x00\x80\x00\x80" \
	"\x80\x00\x00\x80\x80" "\x80\x00\x80\x00\x80" \
	"\xFF\x7F\x00\x80\xFF" "\x80\x00\x80\x00\x80"

ImagePtr img_ega_byteplanar_get_converter(iostream_sptr imgStream,
	FN_TRUNCATE fnTruncate, unsigned int width, unsigned int height)
{
	PLANE_LAYOUT planes;
	planes[PLANE_BLUE] = 2;
	planes[PLANE_GREEN] = 3;
	planes[PLANE_RED] = 4;
	planes[PLANE_INTENSITY] = 5;
	planes[PLANE_HITMAP] = 0;
	planes[PLANE_OPACITY] = 1;
	return ImagePtr(new EGABytePlanarImage(
		imgStream, fnTruncate, width, height, planes));
}

#define IMG_CLASS img_ega_byteplanar
#include "test-img.hpp"
