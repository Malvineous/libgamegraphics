/**
 * @file   test-subimage.cpp
 * @brief  Test code for Image_Sub class.
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

#include "../src/img-vga-raw.hpp"
#include "../src/image-sub.hpp"
#include "test-image.hpp"

class test_subimage: public test_image
{
	public:
		test_subimage()
		{
			this->type = "subimage";
			this->hasMask = false;
			this->hasHitmask = false;
			this->fixedDimensions = true;
			this->dimensions = {4, 4};
		}

		void addTests()
		{
			this->test_image::addTests();

			this->sizedContent({4, 4}, ImageType::DefinitelyYes,
				this->initialstate());
		}

		virtual std::string initialstate() const
		{
			return STRING_WITH_NULLS(
				"\x00\x00\x00\x00\x00\x00\x00\x00"
				"\x00\x00\x00\x00\x00\x00\x00\x00"
				"\x00\x00\x0F\x0F\x0F\x0F\x00\x00"
				"\x00\x00\x0C\x00\x00\x0A\x00\x00"
				"\x00\x00\x0C\x00\x00\x0A\x00\x00"
				"\x00\x00\x0C\x09\x09\x0E\x00\x00"
				"\x00\x00\x00\x00\x00\x00\x00\x00"
				"\x00\x00\x00\x00\x00\x00\x00\x00"
			);
		}

		virtual std::unique_ptr<Image> openImage(const Point& dims,
			std::unique_ptr<stream::inout> content, ImageType::Certainty result,
			bool create)
		{
			if (create) {
				// Create an empty image, 8x8 pixels in size
				content->truncate(8 * 8);
			}
			auto imgMain = std::make_shared<Image_VGARaw>(std::move(content),
				Point{8, 8}, nullptr);
			auto imgMainPix = std::make_shared<Pixels>(imgMain->convert());
			auto imgMainMsk = std::make_shared<Pixels>(imgMain->convert_mask());
			return std::make_unique<Image_Sub>(
				imgMainPix,
				imgMainMsk,
				imgMain->dimensions(),
				Rect{2, 2, 4, 4},
				ColourDepth::VGA,
				nullptr,
				[imgMain, imgMainPix, imgMainMsk](){
					imgMain->convert(*imgMainPix, *imgMainMsk);
				});
		}
};

IMPLEMENT_TESTS(subimage);
