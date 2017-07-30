/**
 * @file   test-img-sw93beta-planar.cpp
 * @brief  Test code for Shadow Warrior 1993 Beta planar images.
 *
 * Copyright (C) 2010-2017 Adam Nielsen <malvineous@shikadi.net>
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

#include <camoto/iostream_helpers.hpp>
#include <camoto/util.hpp> // make_unique
#include "test-image.hpp"
#include "../src/img-bash-sprite.hpp"

class test_img_sw93beta_planar: public test_image
{
	public:
		test_img_sw93beta_planar()
		{
			this->type = "img-sw93beta-planar";
			this->hasMask = false;
			this->hasHitmask = false;
		}

		void addTests()
		{
			this->test_image::addTests();

			// c00: Initial state
			this->isInstance(ImageType::DefinitelyYes, this->initialstate());

			// c01: Too short
			this->isInstance(ImageType::DefinitelyNo, STRING_WITH_NULLS(
				"\x00\x00\x00\x00\x00\x00"
			));

			// c01a: OK at minimum size
			this->isInstance(ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x00\x00\x00\x00\x00\x00\x00"
			));

			// c02: Missing a plane
			this->isInstance(ImageType::DefinitelyNo, STRING_WITH_NULLS(
				"\x08\x08\x00"
				"\x02"
				"\x0F\x0F"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x09"
				"\x02"
				"\x0F\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x09\x09"
				"\x02"
				"\x0F\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x09\x09"
			));

			// c03: Incomplete plane
			this->isInstance(ImageType::DefinitelyNo, STRING_WITH_NULLS(
				"\x08\x08\x00"
				"\x02"
				"\x0F\x0F"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x09"
				"\x02"
				"\x0F\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x09\x09"
				"\x02"
				"\x0F\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x09\x09"
				"\x02"
				"\x0F\x0F"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x09"
			));

			// c04: Extra trailing data
			this->isInstance(ImageType::DefinitelyNo, this->initialstate() + "x");

			this->sizedContent({8, 8}, ImageType::DefinitelyYes,
				this->initialstate());

			this->sizedContent({16, 16}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x10\x10\x00"
				"\x04"
				"\x0F\x0F\x0F\x0F"
				"\x0C\x00\x00\x00"
				"\x0C\x00\x00\x00"
				"\x0C\x00\x00\x00"
				"\x0C\x00\x00\x00"
				"\x0C\x00\x00\x00"
				"\x0C\x00\x00\x00"
				"\x0C\x00\x00\x00"
				"\x0C\x00\x00\x00"
				"\x0C\x00\x00\x00"
				"\x0C\x00\x00\x00"
				"\x0C\x00\x00\x00"
				"\x0C\x00\x00\x00"
				"\x0C\x00\x00\x00"
				"\x0C\x00\x00\x00"
				"\x0C\x09\x09\x09"
				"\x04"
				"\x0F\x0F\x0F\x0F"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x09\x09\x09\x09"
				"\x04"
				"\x0F\x0F\x0F\x0F"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x09\x09\x09\x09"
				"\x04"
				"\x0F\x0F\x0F\x0F"
				"\x00\x00\x00\x0A"
				"\x00\x00\x00\x0A"
				"\x00\x00\x00\x0A"
				"\x00\x00\x00\x0A"
				"\x00\x00\x00\x0A"
				"\x00\x00\x00\x0A"
				"\x00\x00\x00\x0A"
				"\x00\x00\x00\x0A"
				"\x00\x00\x00\x0A"
				"\x00\x00\x00\x0A"
				"\x00\x00\x00\x0A"
				"\x00\x00\x00\x0A"
				"\x00\x00\x00\x0A"
				"\x00\x00\x00\x0A"
				"\x09\x09\x09\x0E"
			));

			this->sizedContent({9, 9}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x09\x09\x00"
				"\x03"
				"\x0F\x0F\x0F"
				"\x0C\x00\x0A"
				"\x0C\x00\x0A"
				"\x0C\x00\x0A"
				"\x0C\x00\x0A"
				"\x0C\x00\x0A"
				"\x0C\x00\x0A"
				"\x0C\x00\x0A"
				"\x0C\x09\x0E"
				"\x02"
				"\x0F\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x09\x09"
				"\x02"
				"\x0F\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x09\x09"
				"\x02"
				"\x0F\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x09\x09"
			));

			this->sizedContent({8, 4}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x04\x08\x00"
				"\x02"
				"\x0F\x0F"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x09"
				"\x02"
				"\x0F\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x09\x09"
				"\x02"
				"\x0F\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x09\x09"
				"\x02"
				"\x0F\x0F"
				"\x00\x0A"
				"\x00\x0A"
				"\x09\x0E"
			));
		}

		virtual std::string initialstate() const
		{
			return STRING_WITH_NULLS(
				"\x08\x08\x00"
				"\x02"
				"\x0F\x0F"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x09"
				"\x02"
				"\x0F\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x09\x09"
				"\x02"
				"\x0F\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x09\x09"
				"\x02"
				"\x0F\x0F"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x09\x0E"
			);
		}
};

IMPLEMENT_TESTS(img_sw93beta_planar);
