/**
 * @file   test-img-zone66_tile.cpp
 * @brief  Test code for conversion to and from Zone 66 tiles.
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

class test_img_zone66_tile: public test_image
{
	public:
		test_img_zone66_tile()
		{
			this->type = "img-zone66_tile";
			this->hasMask = false;
			this->hasHitmask = false;
		}

		void addTests()
		{
			this->test_image::addTests();

			this->sizedContent({8, 8}, ImageType::DefinitelyYes, this->initialstate());

			this->sizedContent({16, 16}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x10\x00\x10\x00"
				"\x10\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\xFE"
				"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x0e"                                       "\x01\x0A\xFE"
				"\x10\x0C\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x0E"
				"\xFF"
			));

			this->sizedContent({9, 9}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x09\x00\x09\x00"
				"\x09\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\xFE"
				"\x01\x0C" "\xFD\x07"           "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x07"           "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x07"           "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x07"           "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x07"           "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x07"           "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x07"           "\x01\x0A\xFE"
				"\x09\x0C\x09\x09\x09\x09\x09\x09\x09\x0E"
				"\xFF"
			));

			this->sizedContent({8, 4}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x08\x00\x04\x00" \
				"\x08\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x08\x0C\x09\x09\x09\x09\x09\x09\x0E"
				"\xFF"
			));

			// Compression check
			this->sizedContent({8, 3}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x08\x00\x03\x00"
				"\x04\x00\x01\x01\x01" "\xFE" /* initial black pixel */
				"\xFE" /* empty line */
				"\x04\x02\x00\x00\x03" "\xFD\x03" "\x01\x04" /* two and three black pixels */
				"\xFF"
			), {}, STRING_WITH_NULLS(
				"\x00\x01\x01\x01\x00\x00\x00\x00"
				"\x00\x00\x00\x00\x00\x00\x00\x00"
				"\x02\x00\x00\x03\x00\x00\x00\x04"
			));

			// Make sure it works with a max-size format (no 'compression' at all)
			this->sizedContent({4, 3}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x04\x00\x03\x00"
				"\x04\x00\x01\x00\x01" "\xFE"
				"\x04\x01\x00\x00\x01" "\xFE"
				"\x04\x00\x01\x00\x01"
				"\xFF"
			), {}, STRING_WITH_NULLS(
				"\x00\x01\x00\x01"
				"\x01\x00\x00\x01"
				"\x00\x01\x00\x01"
			));

			// Make sure it works with a trailing blank image
			this->sizedContent({4, 5}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x04\x00\x05\x00"
				"\x04\x01\x01\x01\x01" "\xFE"
				"\x04\x01\x01\x01\x01"
				"\xFF"
			), {}, STRING_WITH_NULLS(
				"\x01\x01\x01\x01"
				"\x01\x01\x01\x01"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
			));

			// Make sure it works with an entirely blank image
			this->sizedContent({4, 7}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x04\x00\x07\x00"
				"\xFF"
			), {}, STRING_WITH_NULLS(
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
			));

			// c00: Initial state
			this->isInstance(ImageType::DefinitelyYes, this->initialstate());

			// c01: Too short
			this->isInstance(ImageType::DefinitelyNo, STRING_WITH_NULLS(
				"\x08\x00\x08"
			));

			// c02: Image too large
			this->isInstance(ImageType::DefinitelyNo, STRING_WITH_NULLS(
				"\x08\xF0\x08\x00"
				"\x08\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x08\x0C\x09\x09\x09\x09\x09\x09\x0E"
				"\xFF"
			));

			// c03: Cut off in middle of 0xFD code
			this->isInstance(ImageType::DefinitelyNo, STRING_WITH_NULLS(
				"\x08\x00\x08\x00"
				"\x08\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x07\x0C\x09\x09\x09\x09\x09\x09\xFD"
			));

			// c04: Invalid code
			this->isInstance(ImageType::DefinitelyNo, STRING_WITH_NULLS(
				"\x08\x00\x08\x00"
				"\x08\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x07\x0C\x09\x09\x09\x09\x09\x09\x00"
				"\xFF"
			));

			// c05: EOF marker with trailing data
			this->isInstance(ImageType::DefinitelyNo, STRING_WITH_NULLS(
				"\x08\x00\x08\x00"
				"\x08\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x08\x0C\x09\x09\x09\x09\x09\x09\x0E"
				"\xFF" "test"
			));

			// c06: Read all data but didn't have 0xFF at EOF
			this->isInstance(ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x08\x00\x08\x00"
				"\x08\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x08\x0C\x09\x09\x09\x09\x09\x09\x0E"
			));

			// c07: Ended mid-data
			this->isInstance(ImageType::DefinitelyNo, STRING_WITH_NULLS(
				"\x08\x00\x08\x00"
				"\x08\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
			));
		}

		virtual std::string initialstate() const
		{
			return STRING_WITH_NULLS(
				"\x08\x00\x08\x00"
				"\x08\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x01\x0C" "\xFD\x06"       "\x01\x0A\xFE"
				"\x08\x0C\x09\x09\x09\x09\x09\x09\x0E"
				"\xFF"
			);
		}
};

IMPLEMENT_TESTS(img_zone66_tile);
