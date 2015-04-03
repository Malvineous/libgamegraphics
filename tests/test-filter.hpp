/**
 * @file  test-filter.hpp
 * @brief Generic test code for filters.
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

#include <boost/test/unit_test.hpp>

#include <camoto/stream_string.hpp>
#include <camoto/stream_filtered.hpp>
#include <camoto/gamegraphics.hpp>
#include "tests.hpp"

using namespace camoto;
using namespace camoto::gamegraphics;

class test_filter: public test_main
{
	public:
		/// Constructor sets some default values.
		test_filter();

		/// Add all the standard tests.
		/**
		 * This can be overridden by descendent classes to add more tests for
		 * particular file formats.  If this is done, remember to call this
		 * function from the overridden one or the standard tests won't get run.
		 */
		virtual void addTests();

		/// Reset filter back to a known state.
		/**
		 * @param empty
		 *   true resets to empty content while
		 *   false resets to initialstate().
		 */
		virtual void prepareTest(bool empty);

		/// Add a test to the suite.  Used by ADD_FILTER_TEST().
		void addBoundTest(bool empty, std::function<void()> fnTest,
			boost::unit_test::const_string name);

		/// Reset the filter to the initial state and run the given test.
		/**
		 * @param empty
		 *   true resets to empty content while
		 *   false resets to initialstate().
		 *
		 * @param fnTest
		 *   Function to call once filter is back to initial state.
		 */
		void runTest(bool empty, std::function<void()> fnTest);

		/// Add a test encoding (filtering) the data
		void process(std::unique_ptr<filter> algo, const std::string& input,
			const std::string& output);

		/// Add a test encoding (filtering) the data that should fail.
		/**
		 * Typically this is only when turning previously filtered data back into
		 * its original format (e.g. decompressing) and invalid data is presented.
		 * When filtering data for the first time (e.g. compressing) all possible
		 * input data should be valid so there should be no failure states.
		 */
		void fail(std::unique_ptr<filter> algo, const std::string& input);

	protected:
		/// Perform an encode/decode check now, running the given filter.
		void test_process(std::unique_ptr<filter> algo, const std::string& input,
			const std::string& output, unsigned int testNumber);

		/// Perform an encode/decode failure check now, running the given filter.
		void test_fail(std::unique_ptr<filter> algo, const std::string& input,
			unsigned int testNumber);

	private:
		/// Number of encode/decode tests, used to number them sequentially.
		unsigned int numProcessTests;

		/// Number of failure tests, used to number them sequentially.
		unsigned int numFailTests;
};
