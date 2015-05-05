/**
 * @file  test-img-ccomic.cpp
 * @brief Test code for Captain Comic full-screen image handler.
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

class test_img_ccomic: public test_image
{
	public:
		test_img_ccomic()
		{
			this->type = "img-ccomic";
			this->hasMask = false;
			this->hasHitmask = false;
			this->fixedDimensions = true;
			this->dimensions = {320, 200};
		}

		void addTests()
		{
			this->test_image::addTests();

			this->sizedContent({320, 200}, ImageType::DefinitelyYes,
				this->initialstate());

			// c00: Initial state
			this->isInstance(ImageType::DefinitelyYes, this->initialstate());

			// c01: Too short
			this->isInstance(ImageType::DefinitelyNo, STRING_WITH_NULLS(
				"\x01"
			));

			// c02: Wrong plane size
			auto c02 = this->initialstate();
			c02[0] = '\xF0';
			this->isInstance(ImageType::DefinitelyNo, c02);

			// c03: Incomplete read
			auto c03 = this->initialstate().substr(0, 8);
			this->isInstance(ImageType::DefinitelyNo, c03);

			// c04: Trailing data
			auto c04 = this->initialstate();
			c04.append("hello");
			this->isInstance(ImageType::DefinitelyNo, c04);
		}

		virtual std::string initialstate() const
		{
			auto content = STRING_WITH_NULLS(
				"\x40\x1F"
				"\xA8\xFF\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
				"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
				"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
				"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
				"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
				"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
				"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
				"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xAE\x00"
				"\x01\x7F\xA6\xFF\x01\xFE\xA8\xFF"
			);
			for (unsigned int i = 0; i < 199; i++) {
				content.append("\xA7\x00\x01\x01", 4);
			}
			content.append("\xA8\xFF\x01\x80", 4);
			for (unsigned int i = 0; i < 198; i++) {
				content.append("\xA7\x00\x01\x80", 4);
			}
			content.append("\xA6\x00\x01\x01\xA8\xFF\x01\x80", 8);
			for (unsigned int i = 0; i < 197; i++) {
				content.append("\xA6\x00\x02\x01\x80", 5);
			}
			content.append("\xA6\x00\x01\x01\xA8\xFF", 6);
			return content;
		}
};

IMPLEMENT_TESTS(img_ccomic);
