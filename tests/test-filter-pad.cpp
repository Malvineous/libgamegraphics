/**
 * @file   test-filter-block-pad.cpp
 * @brief  Test code for padding filter.
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

#include "../src/filter-block-pad.hpp"
#include "test-filter.hpp"

class test_filter_block_pad: public test_filter
{
	public:
		test_filter_block_pad()
		{
		}

		void addTests()
		{
			this->test_filter::addTests();

			this->process(std::make_unique<filter_block_pad>("pad", 10),
				STRING_WITH_NULLS(
				"0123456789ABCDEFGHI"
			), STRING_WITH_NULLS(
				"0123456789padABCDEFGHI"
			));

			this->process(std::make_unique<filter_block_pad>("pad", 10),
				STRING_WITH_NULLS(
				"0123456789ABCDEFGHIJ"
			), STRING_WITH_NULLS(
				"0123456789padABCDEFGHIJpad"
			));

			this->process(std::make_unique<filter_block_unpad>(3, 10),
				STRING_WITH_NULLS(
				"0123456789padABCDEFGHI"
			), STRING_WITH_NULLS(
				"0123456789ABCDEFGHI"
			));

			this->process(std::make_unique<filter_block_unpad>(3, 10),
				STRING_WITH_NULLS(
				"0123456789padABCDEFGHIJpad"
			), STRING_WITH_NULLS(
				"0123456789ABCDEFGHIJ"
			));
		}
};

IMPLEMENT_TESTS(filter_block_pad);
