/**
 * @file   test-img-bash-sprite.cpp
 * @brief  Test code for Monster Bash sprites.
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

#include <camoto/iostream_helpers.hpp>
#include <camoto/util.hpp> // make_unique
#include "test-image.hpp"
#include "../src/img-bash-sprite.hpp"

class test_img_bash_sprite: public test_image
{
	public:
		test_img_bash_sprite()
		{
			this->type = "img-bash-sprite";
			this->hasMask = true;
			this->hasHitmask = false;
			this->hotspot = {1, -1};
			this->hitrect = {0, 1};
		}

		void addTests()
		{
			this->test_image::addTests();

			this->sizedContent({8, 8}, ImageType::DefinitelyYes,
				this->initialstate());

			this->sizedContent({16, 16}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x00\x10\x10\x00"
				"\xFF\xFF\x01\x00\x00\x00\x01\x00"
				"\x02\x00\x00\x7F\xFE\x7F\xFE\x7F\xFE\x7F\xFE\x7F\xFE\x7F\xFE\x7F\xFE\x7F\xFE\x7F\xFE\x7F\xFE\x7F\xFE\x7F\xFE\x7F\xFE\x7F\xFE\x00\x00"
				"\x01\xFF\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x7F\xFE"
				"\x02\xFF\xFF\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01"
				"\x04\xFF\xFF\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x01"
				"\x08\xFF\xFF\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\xFF\xFF"
				"\x00"
			));

			this->sizedContent({9, 9}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x00\x09\x09\x00"
				"\xFF\xFF\x01\x00\x00\x00\x01\x00"
				"\x02\x00\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x00\x7F"
				"\x01\xFF\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x7F\x00"
				"\x02\xFF\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80"
				"\x04\xFF\x80\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x80"
				"\x08\xFF\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\xFF\x80"
				"\x00"
			));

			this->sizedContent({8, 4}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x00\x04\x08\x00"
				"\xFF\xFF\x01\x00\x00\x00\x01\x00"
				"\x01\x00\x7E\x7E\x00"
				"\x01\xFF\x00\x00\x7E"
				"\x02\xFF\x01\x01\x01"
				"\x04\xFF\x80\x80\x81"
				"\x08\xFF\x81\x81\xFF"
				"\x00"
			));
		}

		virtual std::string initialstate() const
		{
			return STRING_WITH_NULLS(
				"\x00\x08\x08\x00"
				"\xFF\xFF\x01\x00\x00\x00\x01\x00"
				"\x01\x00\x7E\x7E\x7E\x7E\x7E\x7E\x00"
				"\x01\xFF\x00\x00\x00\x00\x00\x00\x7E"
				"\x02\xFF\x01\x01\x01\x01\x01\x01\x01"
				"\x04\xFF\x80\x80\x80\x80\x80\x80\x81"
				"\x08\xFF\x81\x81\x81\x81\x81\x81\xFF"
				"\x00"
			);
		}

		virtual std::unique_ptr<Image> openImage(const Point& dims,
			std::unique_ptr<stream::inout> content, ImageType::Certainty result,
			bool create)
		{
			if (create) {
				// Create an empty image, 0x0 pixels in size
				content->truncate(13);
				*content
					<< u8(0)
					<< u8(0)
					<< u8(0)
					<< u8(0)
					<< s16le(0)
					<< s16le(0)
					<< u16le(0)
					<< u16le(0)
					<< u8(0) // EOF plane flag
				;
			}
			return std::make_unique<Image_BashSprite>(std::move(content));
		}
};

IMPLEMENT_TESTS(img_bash_sprite);
