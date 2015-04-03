/**
 * @file   test-tileset.hpp
 * @brief  Generic test code for Tileset class descendents.
 *
 * Copyright (C) 2010-2015 Adam Nielsen <malvineous@shikadi.net>
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

#ifndef _CAMOTO_GAMETILESET_TEST_TILESET_HPP_
#define _CAMOTO_GAMETILESET_TEST_TILESET_HPP_

#include <camoto/gamegraphics.hpp>
#include "test-archive.hpp"

// This header will only be used by test implementations.
using namespace camoto;
using namespace camoto::gamegraphics;

class test_tileset: public test_archive
{
	public:
		/// Constructor sets some default values.
		test_tileset();

		virtual void addTests();
		virtual void prepareTest(bool empty);

		virtual void test_isInstance(ArchiveType::Certainty result,
			const std::string& content, unsigned int testNumber);

		virtual void test_isinstance_others();
		virtual void test_new_isinstance();

		void test_open_image();

		/// Dimensions of the first tile in the tileset.
		Point firstTileDims;
};

/// Add a test_tileset member function to the test suite
#define ADD_TILESET_TEST(empty, fn) {	  \
	std::function<void()> fnTest = std::bind(fn, this); \
	this->test_tileset::addBoundTest(empty, fnTest, BOOST_TEST_STRINGIZE(fn)); \
}

#endif // _CAMOTO_GAMETILESET_TEST_TILESET_HPP_
