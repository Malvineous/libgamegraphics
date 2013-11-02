/**
 * @file   test-filter-pad.cpp
 * @brief  Test code for padding filter.
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

#include <boost/test/unit_test.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <camoto/util.hpp>

#include "tests.hpp"
#include "test-filter.hpp"
#include "../src/filter-pad.hpp"

using namespace camoto;
using namespace camoto::gamegraphics;

struct pad_sample: public filter_sample {
	pad_sample()
	{
		this->filter.reset(new filter_pad("pad", 10));
	}
};

struct unpad_sample: public filter_sample {
	unpad_sample()
	{
		this->filter.reset(new filter_unpad(3, 10));
	}
};


BOOST_FIXTURE_TEST_SUITE(unpad_suite, unpad_sample)

BOOST_AUTO_TEST_CASE(decode)
{
	BOOST_TEST_MESSAGE("Unpad some padded data");

	in << makeString(
		"0123456789padABCDEFGHI"
	);

	BOOST_CHECK_MESSAGE(is_equal(makeString(
		"0123456789ABCDEFGHI"
	)),
		"Unpadding some padded data failed"
	);
}

BOOST_AUTO_TEST_CASE(decode_eof)
{
	BOOST_TEST_MESSAGE("Unpad some padded data, with pad at EOF");

	in << makeString(
		"0123456789padABCDEFGHIJpad"
	);

	BOOST_CHECK_MESSAGE(is_equal(makeString(
		"0123456789ABCDEFGHIJ"
	)),
		"Unpadding some padded data, with pad at EOF, failed"
	);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(pad_suite, pad_sample)

BOOST_AUTO_TEST_CASE(encode)
{
	BOOST_TEST_MESSAGE("Pad some data");

	in << makeString(
		"0123456789ABCDEFGHI"
	);

	BOOST_CHECK_MESSAGE(is_equal(makeString(
		"0123456789padABCDEFGHI"
	)),
		"Padding data failed"
	);
}

BOOST_AUTO_TEST_CASE(encode_eof)
{
	BOOST_TEST_MESSAGE("Pad some data, with pad at EOF");

	in << makeString(
		"0123456789ABCDEFGHIJ"
	);

	BOOST_CHECK_MESSAGE(is_equal(makeString(
		"0123456789padABCDEFGHIJpad"
	)),
		"Padding data with pad at EOF failed"
	);
}

BOOST_AUTO_TEST_SUITE_END()
