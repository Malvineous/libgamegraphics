/**
 * @file   test-pal-defaults.cpp
 * @brief  Test code for the default palette generation code.
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

#include <boost/test/unit_test.hpp>
#include <boost/bind.hpp>

#include <camoto/gamegraphics/palettetable.hpp>

#include "tests.hpp"

using namespace camoto;
using namespace camoto::gamegraphics;

BOOST_AUTO_TEST_CASE(pal_default_cga)
{
	BOOST_TEST_MESSAGE("Generate default CGA palette");

	PaletteTablePtr pal = createDefaultCGAPalette();

	BOOST_REQUIRE_EQUAL((*pal)[0].red,    0x00);
	BOOST_REQUIRE_EQUAL((*pal)[0].green,  0x00);
	BOOST_REQUIRE_EQUAL((*pal)[0].blue,   0x00);

	BOOST_REQUIRE_EQUAL((*pal)[1].red,    0x00);
	BOOST_REQUIRE_EQUAL((*pal)[1].green,  0x00);
	BOOST_REQUIRE_EQUAL((*pal)[1].blue,   0xAA);

	BOOST_REQUIRE_EQUAL((*pal)[6].red,    0xAA);
	BOOST_REQUIRE_EQUAL((*pal)[6].green,  0x55);
	BOOST_REQUIRE_EQUAL((*pal)[6].blue,   0x00);

	BOOST_REQUIRE_EQUAL((*pal)[14].red,   0xFF);
	BOOST_REQUIRE_EQUAL((*pal)[14].green, 0xFF);
	BOOST_REQUIRE_EQUAL((*pal)[14].blue,  0x55);

	BOOST_REQUIRE_EQUAL((*pal)[15].red,   0xFF);
	BOOST_REQUIRE_EQUAL((*pal)[15].green, 0xFF);
	BOOST_REQUIRE_EQUAL((*pal)[15].blue,  0xFF);
}

BOOST_AUTO_TEST_CASE(pal_default_ega)
{
	BOOST_TEST_MESSAGE("Generate default EGA palette");

	PaletteTablePtr pal = createDefaultEGAPalette();

	BOOST_REQUIRE_EQUAL((*pal)[0].red,    0x00);
	BOOST_REQUIRE_EQUAL((*pal)[0].green,  0x00);
	BOOST_REQUIRE_EQUAL((*pal)[0].blue,   0x00);

	BOOST_REQUIRE_EQUAL((*pal)[1].red,    0x00);
	BOOST_REQUIRE_EQUAL((*pal)[1].green,  0x00);
	BOOST_REQUIRE_EQUAL((*pal)[1].blue,   0xAA);

	BOOST_REQUIRE_EQUAL((*pal)[6].red,    0xAA);
	BOOST_REQUIRE_EQUAL((*pal)[6].green,  0xAA);
	BOOST_REQUIRE_EQUAL((*pal)[6].blue,   0x00);

	BOOST_REQUIRE_EQUAL((*pal)[20].red,   0xAA);
	BOOST_REQUIRE_EQUAL((*pal)[20].green, 0x55);
	BOOST_REQUIRE_EQUAL((*pal)[20].blue,  0x00);

	BOOST_REQUIRE_EQUAL((*pal)[33].red,   0x55);
	BOOST_REQUIRE_EQUAL((*pal)[33].green, 0x00);
	BOOST_REQUIRE_EQUAL((*pal)[33].blue,  0xAA);

	BOOST_REQUIRE_EQUAL((*pal)[46].red,   0xFF);
	BOOST_REQUIRE_EQUAL((*pal)[46].green, 0xAA);
	BOOST_REQUIRE_EQUAL((*pal)[46].blue,  0x55);

	BOOST_REQUIRE_EQUAL((*pal)[62].red,   0xFF);
	BOOST_REQUIRE_EQUAL((*pal)[62].green, 0xFF);
	BOOST_REQUIRE_EQUAL((*pal)[62].blue,  0x55);

	BOOST_REQUIRE_EQUAL((*pal)[63].red,   0xFF);
	BOOST_REQUIRE_EQUAL((*pal)[63].green, 0xFF);
	BOOST_REQUIRE_EQUAL((*pal)[63].blue,  0xFF);
}
