/**
 * @file  test-img-nukem2.cpp
 * @brief Test code for Duke Nukem II full-screen images.
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

#include "test-image.hpp"

class test_img_nukem2: public test_image
{
	public:
		test_img_nukem2()
		{
			this->type = "img-nukem2";
			this->hasMask = false;
			this->hasHitmask = false;
		}

		void addTests()
		{
			this->test_image::addTests();

			this->sizedContent({320, 200}, ImageType::DefinitelyYes,
				this->initialstate());

			// c00: Initial state
			this->isInstance(ImageType::DefinitelyYes, this->initialstate());

			// c01: Wrong size
			this->isInstance(ImageType::DefinitelyNo, this->initialstate() + "test");

			// c02: Palette entry out of range
			auto c02 = this->initialstate();
			c02[32001] = 0x41;
			this->isInstance(ImageType::DefinitelyNo, c02);
		}

		virtual std::string initialstate() const
		{
			std::string content;

			// Blue
			content += std::string(40, '\xFF');
			for (int y = 1; y < 199; y++) {
				content += '\x00';
				content += std::string(38, '\x00');
				content += '\x00';
			}
			content += '\x7F';
			content += std::string(38, '\xFF');
			content += '\xFE';

			// Green
			content += std::string(40, '\xFF');
			for (int y = 1; y < 199; y++) {
				content += '\x00';
				content += std::string(38, '\x00');
				content += '\x01';
			}
			content += '\x00';
			content += std::string(38, '\x00');
			content += '\x01';

			// Red
			content += std::string(40, '\xFF');
			for (int y = 1; y < 199; y++) {
				content += '\x80';
				content += std::string(38, '\x00');
				content += '\x00';
			}
			content += '\x80';
			content += std::string(38, '\x00');
			content += '\x01';

			// Intensity
			content += std::string(40, '\xFF');
			for (int y = 1; y < 199; y++) {
				content += '\x80';
				content += std::string(38, '\x00');
				content += '\x01';
			}
			content += '\xFF';
			content += std::string(38, '\xFF');
			content += '\xFF';

			// Add palette
			content += STRING_WITH_NULLS(
				"\x00\x00\x00"
				"\x00\x00\x2A"
				"\x00\x2A\x00"
				"\x00\x2A\x2A"
				"\x2A\x00\x00"
				"\x2A\x00\x2A"
				"\x2A\x15\x00"
				"\x2A\x2A\x2A"
				"\x15\x15\x15"
				"\x15\x15\x3F"
				"\x15\x3F\x15"
				"\x15\x3F\x3F"
				"\x3F\x15\x15"
				"\x3F\x15\x3F"
				"\x3F\x3F\x15"
				"\x3F\x3F\x3F"
			);
			return content;
		}
};

IMPLEMENT_TESTS(img_nukem2);
