/**
 * @file  test-img-defines.hpp
 * @brief Generic image conversion test code.
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

#ifndef _CAMOTO_TEST_IMG_DEFINES_HPP_
#define _CAMOTO_TEST_IMG_DEFINES_HPP_

#include <boost/test/unit_test.hpp>
#include <camoto/gamegraphics.hpp>
#include <camoto/stream_string.hpp>
#include <camoto/util.hpp> // for TOSTRING macro
#include "tests.hpp"

// Defines to allow code reuse
#define COMBINE_CLASSNAME_EXP(c, n)  c ## _ ## n
#define COMBINE_CLASSNAME(c, n)  COMBINE_CLASSNAME_EXP(c, n)

#define TEST_VAR(n)        COMBINE_CLASSNAME(IMG_CLASS, n)
#define TEST_NAME(n)       TEST_VAR(n)
#define TEST_RESULT(n)     testdata_ ## n

#define FIXTURE_NAME       TEST_VAR(sample)
#define EMPTY_FIXTURE_NAME TEST_VAR(sample_empty)
#define SUITE_NAME         TEST_VAR(suite)
#define EMPTY_SUITE_NAME   TEST_VAR(suite_empty)
#define INITIALSTATE_NAME  TEST_RESULT(initialstate)

// Define an ISINSTANCE_TEST macro which we use to confirm the initial state
// is a valid instance of this format.  This is defined as a macro so the
// format-specific code can reuse it later to test various invalid formats.
#define ISINSTANCE_TEST(c, d, r) \
	BOOST_AUTO_TEST_CASE(TEST_NAME(isinstance_ ## c)) \
	{ \
		BOOST_TEST_MESSAGE("isInstance check (" IMG_TYPE "; " #c ")"); \
		\
		ManagerPtr pManager(getManager()); \
		ImageTypePtr pTestType(pManager->getImageTypeByCode(IMG_TYPE)); \
		BOOST_REQUIRE_MESSAGE(pTestType, "Could not find image type " IMG_TYPE); \
		\
		stream::string_sptr ss(new stream::string()); \
		ss->write(makeString(d)); \
		\
		BOOST_CHECK_EQUAL(pTestType->isInstance(ss), ImageType::r); \
	}

#endif // _CAMOTO_TEST_IMG_DEFINES_HPP_
