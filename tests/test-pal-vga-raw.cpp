/**
 * @file   test-pal-vga-raw.cpp
 * @brief  Test code for reading raw VGA palette files.
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

#include <boost/test/unit_test.hpp>

#include <boost/algorithm/string.hpp> // for case-insensitive string compare
#include <boost/iostreams/copy.hpp>
#include <iostream>

#include "../src/pal-vga-raw.hpp"

#include "tests.hpp"

using namespace camoto;
using namespace camoto::gamegraphics;

BOOST_AUTO_TEST_CASE(pal_vga_raw_read)
{
	BOOST_TEST_MESSAGE("Read from VGA palette");

	uint8_t data[768];
	memset(data, 0, 768);
	data[3] = data[4] = data[5] = 0x3F;
	data[6] = data[7] = data[8] = 0x40;

	iostream_sptr ss(new std::stringstream());
	ss->write((char *)data, 768);
	VGAPalette pal(ss);

	BOOST_REQUIRE_EQUAL(pal.getMaxEntries(), 256);

	const PaletteEntries& ent = pal.getEntries();

	BOOST_REQUIRE_EQUAL(ent[0].red,   0);
	BOOST_REQUIRE_EQUAL(ent[0].green, 0);
	BOOST_REQUIRE_EQUAL(ent[0].blue,  0);

	BOOST_REQUIRE_EQUAL(ent[1].red,   255);
	BOOST_REQUIRE_EQUAL(ent[1].green, 255);
	BOOST_REQUIRE_EQUAL(ent[1].blue,  255);

	BOOST_REQUIRE_EQUAL(ent[1].red,   255);
	BOOST_REQUIRE_EQUAL(ent[1].green, 255);
	BOOST_REQUIRE_EQUAL(ent[1].blue,  255);
}

BOOST_AUTO_TEST_CASE(pal_vga_raw_write)
{
	BOOST_TEST_MESSAGE("Write to VGA palette");

	PaletteEntries ent;
	ent.push_back(PaletteEntry(  0,   0,   0));
	ent.push_back(PaletteEntry(255, 255, 255));

	std::stringstream *pss = new std::stringstream();
	iostream_sptr ss(pss);
	VGAPalette pal(ss);
	pal.setEntries(ent);

	std::string s = pss->str();
	uint8_t *buf = (uint8_t *)s.c_str();

	BOOST_REQUIRE_EQUAL(buf[0], 0);
	BOOST_REQUIRE_EQUAL(buf[1], 0);
	BOOST_REQUIRE_EQUAL(buf[2], 0);

	BOOST_REQUIRE_EQUAL(buf[3], 63);
	BOOST_REQUIRE_EQUAL(buf[4], 63);
	BOOST_REQUIRE_EQUAL(buf[5], 63);
}
