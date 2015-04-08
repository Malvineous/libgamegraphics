/**
 * @file   test-img-ega-byteplanar.cpp
 * @brief  Test code for conversion to and from byte-planar EGA data.
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

#include "../src/img-ega-byteplanar.hpp"
#include "test-image.hpp"

class test_img_ega_byteplanar: public test_image
{
	public:
		test_img_ega_byteplanar()
		{
			this->type = "img-ega-byteplanar";
			this->hasMask = true;
			this->hasHitmask = true;
		}

		void addTests()
		{
			this->test_image::addTests();

			this->sizedContent({8, 8}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\xFF\xFF\xFF\xFF\xFF\x00"
				"\x81\x00\x01\x80\x81\xFF"
				"\x81\x00\x01\x80\x81\x00"
				"\x81\x00\x01\x80\x81\x00"
				"\x81\x00\x01\x80\x81\x00"
				"\x81\x00\x01\x80\x81\x00"
				"\x81\x00\x01\x80\x81\x00"
				"\xFF\x7E\x01\x81\xFF\x00"
			));

			this->sizedContent({16, 16}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\xFF\xFF\xFF\xFF\xFF\x00" "\xFF\xFF\xFF\xFF\xFF\x00"
				"\x80\x00\x00\x80\x80\xFF" "\x01\x00\x01\x00\x01\xFF"
				"\x80\x00\x00\x80\x80\x00" "\x01\x00\x01\x00\x01\x00"
				"\x80\x00\x00\x80\x80\x00" "\x01\x00\x01\x00\x01\x00"
				"\x80\x00\x00\x80\x80\x00" "\x01\x00\x01\x00\x01\x00"
				"\x80\x00\x00\x80\x80\x00" "\x01\x00\x01\x00\x01\x00"
				"\x80\x00\x00\x80\x80\x00" "\x01\x00\x01\x00\x01\x00"
				"\x80\x00\x00\x80\x80\x00" "\x01\x00\x01\x00\x01\x00"
				"\x80\x00\x00\x80\x80\x00" "\x01\x00\x01\x00\x01\x00"
				"\x80\x00\x00\x80\x80\x00" "\x01\x00\x01\x00\x01\x00"
				"\x80\x00\x00\x80\x80\x00" "\x01\x00\x01\x00\x01\x00"
				"\x80\x00\x00\x80\x80\x00" "\x01\x00\x01\x00\x01\x00"
				"\x80\x00\x00\x80\x80\x00" "\x01\x00\x01\x00\x01\x00"
				"\x80\x00\x00\x80\x80\x00" "\x01\x00\x01\x00\x01\x00"
				"\x80\x00\x00\x80\x80\x00" "\x01\x00\x01\x00\x01\x00"
				"\xFF\x7F\x00\x80\xFF\x00" "\xFF\xFE\x01\x01\xFF\x00"
			));

			this->sizedContent({9, 9}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\xFF\xFF\xFF\xFF\xFF\x00" "\x80\x80\x80\x80\x80\x00"
				"\x80\x00\x00\x80\x80\xFF" "\x80\x00\x80\x00\x80\x80"
				"\x80\x00\x00\x80\x80\x00" "\x80\x00\x80\x00\x80\x00"
				"\x80\x00\x00\x80\x80\x00" "\x80\x00\x80\x00\x80\x00"
				"\x80\x00\x00\x80\x80\x00" "\x80\x00\x80\x00\x80\x00"
				"\x80\x00\x00\x80\x80\x00" "\x80\x00\x80\x00\x80\x00"
				"\x80\x00\x00\x80\x80\x00" "\x80\x00\x80\x00\x80\x00"
				"\x80\x00\x00\x80\x80\x00" "\x80\x00\x80\x00\x80\x00"
				"\xFF\x7F\x00\x80\xFF\x00" "\x80\x00\x80\x80\x80\x00"
			));

			this->sizedContent({8, 4}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\xFF\xFF\xFF\xFF\xFF\x00"
				"\x81\x00\x01\x80\x81\xFF"
				"\x81\x00\x01\x80\x81\x00"
				"\xFF\x7E\x01\x81\xFF\x00"
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
				EGAPlanePurpose::Opaque1, // swaps
				EGAPlanePurpose::Blue1,
				EGAPlanePurpose::Green1,
				EGAPlanePurpose::Red1,
				EGAPlanePurpose::Intensity1,
				EGAPlanePurpose::Hit1,
			};

			return std::make_unique<Image_EGA_BytePlanar>(
				std::move(content), 0, dims, planes, nullptr
			);
		}
};

IMPLEMENT_TESTS(img_ega_byteplanar);
