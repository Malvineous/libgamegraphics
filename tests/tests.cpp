/**
 * @file   tests.cpp
 * @brief  Test code core.
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

#define BOOST_TEST_MODULE libgamegraphics
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <boost/algorithm/string.hpp> // for case-insensitive string compare
#include <iostream>
#include <iomanip>

#include <camoto/debug.hpp>
#include "tests.hpp"

void default_sample::printNice(boost::test_tools::predicate_result& res,
	const std::string& s, const std::string& diff, unsigned int width)
{
	const char *c = CLR_YELLOW;
	res.message() << c;
	for (int i = 0; i < s.length(); i++) {
		if ((i > 0) && (i % width == 0)) {
			res.message() << CLR_NORM << "\n" << std::setfill('0') << std::setw(3)
				<< std::hex << i << ": " << c;
		}
		if ((i >= diff.length()) || (s[i] != diff[i])) {
			if (c != CLR_MAG) {
				c = CLR_MAG;
				res.message() << CLR_MAG;
			}
		} else {
			if (c != CLR_YELLOW) {
				c = CLR_YELLOW;
				res.message() << CLR_YELLOW;
			}
		}
		//if (s[i] < 32) {
			res.message() << " " << std::setfill('0') << std::setw(2)
				<< std::hex << (int)((uint8_t)s[i]);
		//} else {
		//	res.message() << s[i];
		//}
	}
	return;
}

void default_sample::print_wrong(boost::test_tools::predicate_result& res,
	const std::string& strExpected, const std::string& strResult,
	unsigned int width)
{
	res.message() << "\nExp: ";
	this->printNice(res, strExpected, strResult, width);
	res.message() << CLR_NORM "\n" << "Got: ";
	this->printNice(res, strResult, strExpected, width);
	res.message() << CLR_NORM;

	return;
}

boost::test_tools::predicate_result default_sample::is_equal(const uint8_t *expected,
	const uint8_t *check, unsigned int len, unsigned int width)
//boost::test_tools::predicate_result default_sample::is_equal(
//	const std::string& strExpected, const std::string& strCheck)
{
	std::string strExpected((char *)expected, len);
	std::string strCheck((char *)check, len);
	if (strExpected.compare(strCheck)) {
		boost::test_tools::predicate_result res(false);
		this->print_wrong(res, strExpected, strCheck, width);
		return res;
	}

	return true;
}
