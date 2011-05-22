/**
 * @file   test-img-ega-planar.cpp
 * @brief  Test code for conversion to and from planar EGA data.
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

#include <camoto/gamegraphics/image.hpp>
#include "../src/img-ega-planar.hpp"

using namespace camoto::gamegraphics;
using namespace camoto;

#define TESTDATA_INITIAL_8x8 \
	"\xFF\x81\x81\x81\x81\x81\x81\xFF" \
	"\xFF\x00\x00\x00\x00\x00\x00\x7E" \
	"\xFF\x01\x01\x01\x01\x01\x01\x01" \
	"\xFF\x80\x80\x80\x80\x80\x80\x80" \
	"\xFF\x81\x81\x81\x81\x81\x81\xFF" \

#define TESTDATA_INITIAL_16x16 \
	"\xFF\xFF\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01" "\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\xFF\xFF" \
	"\xFF\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x7F\xFE" \
	"\xFF\xFF\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01" "\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01" \
	"\xFF\xFF\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00" "\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00" \
	"\xFF\xFF\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01" "\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\xFF\xFF" \

#define TESTDATA_INITIAL_9x9 \
	"\xFF\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\xFF\x80" \
	"\xFF\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x7F\x00" \
	"\xFF\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80" \
	"\xFF\x80\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00" \
	"\xFF\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\xFF\x80"

#define TESTDATA_INITIAL_8x4 \
	"\xFF\x81\x81\xFF" \
	"\xFF\x00\x00\x7E" \
	"\xFF\x01\x01\x01" \
	"\xFF\x80\x80\x80" \
	"\xFF\x81\x81\xFF"

// Because this format isn't exposed as an image type we need some custom code
// to create instances of it.
#define IMG_OPEN_CODE \
	PLANE_LAYOUT planes; \
	planes[PLANE_BLUE] = 2; \
	planes[PLANE_GREEN] = 3; \
	planes[PLANE_RED] = 4; \
	planes[PLANE_INTENSITY] = 5; \
	planes[PLANE_HITMAP] = 0; \
	planes[PLANE_OPACITY] = -1; \
	EGAPlanarImage *ega = new EGAPlanarImage(); \
	this->img = ImagePtr(ega); \
	ega->setParams(this->baseStream, this->fnTruncate, 0, width, height, planes); \
	this->dataWidth = width / 8 * height * 5; // 5 == numplanes

// Same code for creating empty images
#define IMG_CREATE_CODE IMG_OPEN_CODE

#define IMG_CLASS img_ega_planar
#include "test-img.hpp"
