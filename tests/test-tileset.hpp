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
		virtual Archive::FileHandle getFileAt(const Archive::FileVector& files,
			unsigned int index);
		virtual std::shared_ptr<Tileset> openTileset(
			std::unique_ptr<stream::inout> content,	bool create);

		virtual void test_isInstance(ArchiveType::Certainty result,
			const std::string& content, unsigned int testNumber);

		virtual void test_isinstance_others();
		virtual void test_new_isinstance();
		virtual void test_new_to_initialstate();

		/// Open tiles 1 and 2 and verify content.
		void test_open_image();

		/// Replace tile 1 with tile's 3 content and verify against insert_remove()
		void test_change_image();

		/// Dimensions of the first tile in the tileset.
		Point firstTileDims;

		/// Does the format support transparent pixels?
		bool hasMask;

		/// Does the format support hitmask pixels?
		bool hasHitmask;

		/// Set to true if the colour indices can only be 0..3 (false == 0..15)
		bool cga;

		/// Set to true if the tiles have a hotspot value.
		bool hasHotspot;

		/// The hotspot values of the tiles, if hasHotspot is true.
		Point ptHotspot[4];

		/// Set to true if the tiles have a hitrect value.
		bool hasHitrect;

		/// The hitrect values of the tiles, if hasHitrect is true.
		Point ptHitrect[4];
};

/// Add a test_tileset member function to the test suite
#define ADD_TILESET_TEST(empty, fn) \
	this->test_tileset::addBoundTest( \
		empty, \
		std::bind(fn, this), \
		__FILE__, __LINE__, \
		BOOST_TEST_STRINGIZE(fn) \
	);

#endif // _CAMOTO_GAMETILESET_TEST_TILESET_HPP_
