/**
 * @file  test-img-ega-linear.cpp
 * @brief Test code for conversion to and from linear EGA data.
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

#include "../src/img-ega-linear.hpp"
#include "test-image.hpp"

class test_img_ega_linear_le: public test_image
{
	public:
		test_img_ega_linear_le()
		{
			this->type = "img-ega-linear";
			this->hasMask = false;
			this->hasHitmask = false;
		}

		void addTests()
		{
			this->test_image::addTests();

			this->sizedContent({8, 8}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\xFF\xFF\xFF\xFF"
				"\x0C\x00\x00\xA0"
				"\x0C\x00\x00\xA0"
				"\x0C\x00\x00\xA0"
				"\x0C\x00\x00\xA0"
				"\x0C\x00\x00\xA0"
				"\x0C\x00\x00\xA0"
				"\x9C\x99\x99\xE9"
			));

			this->sizedContent({16, 16}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
				"\x0C\x00\x00\x00\x00\x00\x00\xA0"
				"\x0C\x00\x00\x00\x00\x00\x00\xA0"
				"\x0C\x00\x00\x00\x00\x00\x00\xA0"
				"\x0C\x00\x00\x00\x00\x00\x00\xA0"
				"\x0C\x00\x00\x00\x00\x00\x00\xA0"
				"\x0C\x00\x00\x00\x00\x00\x00\xA0"
				"\x0C\x00\x00\x00\x00\x00\x00\xA0"
				"\x0C\x00\x00\x00\x00\x00\x00\xA0"
				"\x0C\x00\x00\x00\x00\x00\x00\xA0"
				"\x0C\x00\x00\x00\x00\x00\x00\xA0"
				"\x0C\x00\x00\x00\x00\x00\x00\xA0"
				"\x0C\x00\x00\x00\x00\x00\x00\xA0"
				"\x0C\x00\x00\x00\x00\x00\x00\xA0"
				"\x0C\x00\x00\x00\x00\x00\x00\xA0"
				"\x9C\x99\x99\x99\x99\x99\x99\xE9"
			));

			this->sizedContent({9, 9}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\xFF\xFF\xFF\xFF\x0F"
				"\x0C\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x0A"
				"\x9C\x99\x99\x99\x0E"
			));

			this->sizedContent({8, 4}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\xFF\xFF\xFF\xFF"
				"\x0C\x00\x00\xA0"
				"\x0C\x00\x00\xA0"
				"\x9C\x99\x99\xE9"
			));
		}

		virtual std::string initialstate() const
		{
			// No instance-related tests for this format.
			return {};
		}

		virtual std::unique_ptr<Image> openImage(const Point& dims,
			std::unique_ptr<stream::inout> content, ImageType::Certainty result,
			bool create)
		{
			EGAPlaneLayout planes = {
				EGAPlanePurpose::Blue1,
				EGAPlanePurpose::Green1,
				EGAPlanePurpose::Red1,
				EGAPlanePurpose::Intensity1,
				EGAPlanePurpose::Unused,
				EGAPlanePurpose::Unused
			};

			return std::make_unique<Image_EGA_Linear>(
				std::move(content), 0, dims, planes, bitstream::endian::littleEndian, nullptr
			);
		}
};

class test_img_ega_linear_be: public test_image
{
	public:
		test_img_ega_linear_be()
		{
			this->type = "img-ega-linear";
			this->hasMask = false;
			this->hasHitmask = false;
		}

		void addTests()
		{
			this->test_image::addTests();

			this->sizedContent({8, 8}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\xFF\xFF\xFF\xFF"
				"\xC0\x00\x00\x0A"
				"\xC0\x00\x00\x0A"
				"\xC0\x00\x00\x0A"
				"\xC0\x00\x00\x0A"
				"\xC0\x00\x00\x0A"
				"\xC0\x00\x00\x0A"
				"\xC9\x99\x99\x9E"
			));

			this->sizedContent({16, 16}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
				"\xC0\x00\x00\x00\x00\x00\x00\x0A"
				"\xC0\x00\x00\x00\x00\x00\x00\x0A"
				"\xC0\x00\x00\x00\x00\x00\x00\x0A"
				"\xC0\x00\x00\x00\x00\x00\x00\x0A"
				"\xC0\x00\x00\x00\x00\x00\x00\x0A"
				"\xC0\x00\x00\x00\x00\x00\x00\x0A"
				"\xC0\x00\x00\x00\x00\x00\x00\x0A"
				"\xC0\x00\x00\x00\x00\x00\x00\x0A"
				"\xC0\x00\x00\x00\x00\x00\x00\x0A"
				"\xC0\x00\x00\x00\x00\x00\x00\x0A"
				"\xC0\x00\x00\x00\x00\x00\x00\x0A"
				"\xC0\x00\x00\x00\x00\x00\x00\x0A"
				"\xC0\x00\x00\x00\x00\x00\x00\x0A"
				"\xC0\x00\x00\x00\x00\x00\x00\x0A"
				"\xC9\x99\x99\x99\x99\x99\x99\x9E"
			));

			this->sizedContent({9, 9}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\xFF\xFF\xFF\xFF\xF0"
				"\xC0\x00\x00\x00\xA0"
				"\xC0\x00\x00\x00\xA0"
				"\xC0\x00\x00\x00\xA0"
				"\xC0\x00\x00\x00\xA0"
				"\xC0\x00\x00\x00\xA0"
				"\xC0\x00\x00\x00\xA0"
				"\xC0\x00\x00\x00\xA0"
				"\xC9\x99\x99\x99\xE0"
			));

			this->sizedContent({8, 4}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\xFF\xFF\xFF\xFF"
				"\xC0\x00\x00\x0A"
				"\xC0\x00\x00\x0A"
				"\xC9\x99\x99\x9E"
			));
		}

		virtual std::string initialstate() const
		{
			// No instance-related tests for this format.
			return {};
		}

		virtual std::unique_ptr<Image> openImage(const Point& dims,
			std::unique_ptr<stream::inout> content, ImageType::Certainty result,
			bool create)
		{
			EGAPlaneLayout planes = {
				EGAPlanePurpose::Intensity1,
				EGAPlanePurpose::Red1,
				EGAPlanePurpose::Green1,
				EGAPlanePurpose::Blue1,
				EGAPlanePurpose::Unused,
				EGAPlanePurpose::Unused
			};

			return std::make_unique<Image_EGA_Linear>(
				std::move(content), 0, dims, planes, bitstream::endian::bigEndian, nullptr
			);
		}
};

IMPLEMENT_TESTS(img_ega_linear_le);
IMPLEMENT_TESTS(img_ega_linear_be);
