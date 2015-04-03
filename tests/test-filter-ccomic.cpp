/**
 * @file   test-filter-ccomic.cpp
 * @brief  Test code for Captain Comic RLE algorithm.
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

#include "../src/filter-ccomic.hpp"
#include "test-filter.hpp"

class test_filter_ccomic: public test_filter
{
	public:
		test_filter_ccomic()
		{
		}

		void addTests()
		{
			this->test_filter::addTests();

			constexpr const unsigned int oneLine = 40;
			std::string egaBufferBlank(oneLine * 200 * 4, '\x00');

			// A single encoded plane (expands to exactly 8000 bytes)
			std::string ccPlaneBlank;
			for (int i = 0; i < 62; i++) ccPlaneBlank.append("\xFF\x00", 2);
			ccPlaneBlank.append("\xFE\x00", 2);

			this->process(std::make_unique<filter_ccomic_unrle>(),
				STRING_WITH_NULLS("\x40\x1F")
				+ ccPlaneBlank
				+ ccPlaneBlank
				+ ccPlaneBlank
				+ ccPlaneBlank,
				egaBufferBlank
			);

			std::string enc_alt = STRING_WITH_NULLS(
				"\x40\x1F"
				"\x0F\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
				"\xF0\x00"
			);
			for (int i = 0; i < 250; i++) enc_alt.append("\xFF\x00", 2);
			enc_alt.append("\xFA\x00\x01\x00", 4);

			this->process(std::make_unique<filter_ccomic_unrle>(),
				enc_alt,
				egaBufferBlank
			);

			assert(egaBufferBlank.length() == 32000);

			this->process(std::make_unique<filter_ccomic_rle>(),
				egaBufferBlank,
				STRING_WITH_NULLS("\x40\x1F")
				+ ccPlaneBlank
				+ ccPlaneBlank
				+ ccPlaneBlank
				+ ccPlaneBlank
			);
		}
};

IMPLEMENT_TESTS(filter_ccomic);
