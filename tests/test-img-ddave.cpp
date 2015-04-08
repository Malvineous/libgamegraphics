/**
 * @file  test-img-ega-rowplanar.cpp
 * @brief Test code for conversion to and from row-planar EGA data.
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
#include "../src/img-ddave.hpp"
#include "test-image.hpp"

class test_img_img_ddave_cga: public test_image
{
	public:
		test_img_img_ddave_cga()
		{
			this->type = "img-ddave-cga";
			this->hasMask = false;
			this->hasHitmask = false;
			this->cga = true;
		}

		void addTests()
		{
			this->test_image::addTests();

			this->sizedContent({8, 8}, ImageType::DefinitelyYes, this->initialstate(),
				createPalette_CGA(CGAPaletteType::CyanMagentaBright));

			this->sizedContent({16, 16}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x10\x00" "\x10\x00"
				"\xFF\xFF\xFF\xFF"
				"\x40\x00\x00\x02"
				"\x40\x00\x00\x02"
				"\x40\x00\x00\x02"
				"\x40\x00\x00\x02"
				"\x40\x00\x00\x02"
				"\x40\x00\x00\x02"
				"\x40\x00\x00\x02"
				"\x40\x00\x00\x02"
				"\x40\x00\x00\x02"
				"\x40\x00\x00\x02"
				"\x40\x00\x00\x02"
				"\x40\x00\x00\x02"
				"\x40\x00\x00\x02"
				"\x40\x00\x00\x02"
				"\x6A\xAA\xAA\xA9"
			));

			this->sizedContent({9, 9}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x09\x00" "\x09\x00"
				"\xFF\xFF\xC0"
				"\x40\x00\x80"
				"\x40\x00\x80"
				"\x40\x00\x80"
				"\x40\x00\x80"
				"\x40\x00\x80"
				"\x40\x00\x80"
				"\x40\x00\x80"
				"\x6A\xAA\x40"
			));

			this->sizedContent({8, 4}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x08\x00" "\x04\x00"
				"\xFF\xFF"
				"\x40\x02"
				"\x40\x02"
				"\x6A\xA9"
			));
		}

		virtual std::string initialstate() const
		{
			// No instance-related tests for this format.
			return STRING_WITH_NULLS(
				"\x08\x00" "\x08\x00"
				"\xFF\xFF"
				"\x40\x02"
				"\x40\x02"
				"\x40\x02"
				"\x40\x02"
				"\x40\x02"
				"\x40\x02"
				"\x6A\xA9"
			);
		}

		virtual std::unique_ptr<Image> openImage(const Point& dims,
			std::unique_ptr<stream::inout> content, ImageType::Certainty result,
			bool create)
		{
			if (create) {
				// Create an empty image, 0x0 pixels in size
				content->truncate(4);
				*content << u16le(0) << u16le(0);
			}
			return std::make_unique<Image_DDaveCGA>(std::move(content), false);
		}
};

class test_img_img_ddave_ega: public test_image
{
	public:
		test_img_img_ddave_ega()
		{
			this->type = "img-ddave-ega";
			this->hasMask = false;
			this->hasHitmask = false;
		}

		void addTests()
		{
			this->test_image::addTests();

			this->sizedContent({8, 8}, ImageType::DefinitelyYes, this->initialstate());

			this->sizedContent({16, 16}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x10\x00" "\x10\x00"
				"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
				"\x80\x01\x80\x00\x00\x01\x00\x00"
				"\x80\x01\x80\x00\x00\x01\x00\x00"
				"\x80\x01\x80\x00\x00\x01\x00\x00"
				"\x80\x01\x80\x00\x00\x01\x00\x00"
				"\x80\x01\x80\x00\x00\x01\x00\x00"
				"\x80\x01\x80\x00\x00\x01\x00\x00"
				"\x80\x01\x80\x00\x00\x01\x00\x00"
				"\x80\x01\x80\x00\x00\x01\x00\x00"
				"\x80\x01\x80\x00\x00\x01\x00\x00"
				"\x80\x01\x80\x00\x00\x01\x00\x00"
				"\x80\x01\x80\x00\x00\x01\x00\x00"
				"\x80\x01\x80\x00\x00\x01\x00\x00"
				"\x80\x01\x80\x00\x00\x01\x00\x00"
				"\x80\x01\x80\x00\x00\x01\x00\x00"
				"\xFF\xFF\x80\x01\x00\x01\x7F\xFE"
			));

			this->sizedContent({9, 9}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x09\x00" "\x09\x00"
				"\xFF\x80\xFF\x80\xFF\x80\xFF\x80"
				"\x80\x80\x80\x00\x00\x80\x00\x00"
				"\x80\x80\x80\x00\x00\x80\x00\x00"
				"\x80\x80\x80\x00\x00\x80\x00\x00"
				"\x80\x80\x80\x00\x00\x80\x00\x00"
				"\x80\x80\x80\x00\x00\x80\x00\x00"
				"\x80\x80\x80\x00\x00\x80\x00\x00"
				"\x80\x80\x80\x00\x00\x80\x00\x00"
				"\xFF\x80\x80\x80\x00\x80\x7F\x00"
			));

			this->sizedContent({8, 4}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x08\x00" "\x04\x00"
				"\xFF\xFF\xFF\xFF"
				"\x81\x80\x01\x00"
				"\x81\x80\x01\x00"
				"\xFF\x81\x01\x7E"
			));
		}

		virtual std::string initialstate() const
		{
			// No instance-related tests for this format.
			return STRING_WITH_NULLS(
				"\x08\x00" "\x08\x00"
				"\xFF\xFF\xFF\xFF"
				"\x81\x80\x01\x00"
				"\x81\x80\x01\x00"
				"\x81\x80\x01\x00"
				"\x81\x80\x01\x00"
				"\x81\x80\x01\x00"
				"\x81\x80\x01\x00"
				"\xFF\x81\x01\x7E"
			);
		}

		virtual std::unique_ptr<Image> openImage(const Point& dims,
			std::unique_ptr<stream::inout> content, ImageType::Certainty result,
			bool create)
		{
			if (create) {
				// Create an empty image, 0x0 pixels in size
				content->truncate(4);
				*content << u16le(0) << u16le(0);
			}
			return std::make_unique<Image_DDaveEGA>(std::move(content), false);
		}
};

class test_img_img_ddave_vga: public test_image
{
	public:
		test_img_img_ddave_vga()
		{
			this->type = "img-ddave-vga";
			this->hasMask = false;
			this->hasHitmask = false;
		}

		void addTests()
		{
			this->test_image::addTests();

			this->sizedContent({8, 8}, ImageType::DefinitelyYes, this->initialstate());

			this->sizedContent({16, 16}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x10\x00" "\x10\x00"
				"\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F"
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x0E"
			));

			this->sizedContent({9, 9}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x09\x00" "\x09\x00"
				"\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F"
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x09\x09\x09\x09\x09\x09\x09\x0E"
			));

			this->sizedContent({8, 4}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x08\x00" "\x04\x00"
				"\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x09\x09\x09\x09\x09\x09\x0E"
			));
		}

		virtual std::string initialstate() const
		{
			// No instance-related tests for this format.
			return STRING_WITH_NULLS(
				"\x08\x00" "\x08\x00"
				"\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x09\x09\x09\x09\x09\x09\x0E"
			);
		}

		virtual std::unique_ptr<Image> openImage(const Point& dims,
			std::unique_ptr<stream::inout> content, ImageType::Certainty result,
			bool create)
		{
			if (create) {
				// Create an empty image, 0x0 pixels in size
				content->truncate(4);
				*content << u16le(0) << u16le(0);
			}
			return std::make_unique<Image_DDaveVGA>(std::move(content), false, createPalette_DefaultVGA());
		}
};

IMPLEMENT_TESTS(img_img_ddave_cga);
IMPLEMENT_TESTS(img_img_ddave_ega);
IMPLEMENT_TESTS(img_img_ddave_vga);
