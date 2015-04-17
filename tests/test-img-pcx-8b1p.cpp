/**
 * @file   test-img-pcx-8b1p.cpp
 * @brief  Test code for PCX file handler (with VGA linear image data.)
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
#include "test-image.hpp"
#include "../src/img-pcx.hpp"

class test_img_pcx_8b1p: public test_image
{
	public:
		test_img_pcx_8b1p()
		{
			this->type = "img-pcx-8b1p";
			this->hasMask = false;
			this->hasHitmask = false;

			this->skipInstDetect.push_back("img-pcx-8b1p-norle");

			this->pcxPal = STRING_WITH_NULLS(
				"\x00\x00\x00"
				"\x00\x00\xAA"
				"\x00\xAA\x00"
				"\x00\xAA\xAA"
				"\xAA\x00\x00"
				"\xAA\x00\xAA"
				"\xAA\x55\x00"
				"\xAA\xAA\xAA"
				"\x55\x55\x55"
				"\x55\x55\xFF"
				"\x55\xFF\x55"
				"\x55\xFF\xFF"
				"\xFF\x55\x55"
				"\xFF\x55\xFF"
				"\xFF\xFF\x55"
				"\xFF\xFF\xFF"
			);
		}

		void addTests()
		{
			this->test_image::addTests();

			ADD_IMAGE_TEST(false, &test_img_pcx_8b1p::test_truncated_image);
			ADD_IMAGE_TEST(false, &test_img_pcx_8b1p::test_truncated_rle);

			this->sizedContent({8, 8}, ImageType::DefinitelyYes, this->initialstate(),
				createPalette_DefaultEGA());

			this->sizedContent({16, 16}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x0A\x05\x01"
				"\x08"
				"\x00\x00" "\x00\x00" "\x0F\x00" "\x0F\x00"
				"\x4B\x00" "\x4B\x00"
			) + this->pcxPal + STRING_WITH_NULLS(
				"\x00"
				"\x01"
				"\x10\x00" "\x01\x00"
				"\x00\x00" "\x00\x00"
			) + std::string(54, '\x00') + STRING_WITH_NULLS(
				"\xD0\x0F"
				"\x0C\xCE\x00\x0A"
				"\x0C\xCE\x00\x0A"
				"\x0C\xCE\x00\x0A"
				"\x0C\xCE\x00\x0A"
				"\x0C\xCE\x00\x0A"
				"\x0C\xCE\x00\x0A"
				"\x0C\xCE\x00\x0A"
				"\x0C\xCE\x00\x0A"
				"\x0C\xCE\x00\x0A"
				"\x0C\xCE\x00\x0A"
				"\x0C\xCE\x00\x0A"
				"\x0C\xCE\x00\x0A"
				"\x0C\xCE\x00\x0A"
				"\x0C\xCE\x00\x0A"
				"\x0C\xCE\x09\x0E"
			), createPalette_DefaultEGA());

			this->sizedContent({9, 9}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x0A\x05\x01"
				"\x08"
				"\x00\x00" "\x00\x00" "\x08\x00" "\x08\x00"
				"\x4B\x00" "\x4B\x00"
			) + this->pcxPal + STRING_WITH_NULLS(
				"\x00"
				"\x01"
				"\x0A\x00" "\x01\x00"
				"\x00\x00" "\x00\x00"
			) + std::string(54, '\x00') + STRING_WITH_NULLS(
				"\xCA\x0F"
				"\x0C\xC7\x00\x0A\x0A" // last byte on each scanline is padding
				"\x0C\xC7\x00\x0A\x0A"
				"\x0C\xC7\x00\x0A\x0A"
				"\x0C\xC7\x00\x0A\x0A"
				"\x0C\xC7\x00\x0A\x0A"
				"\x0C\xC7\x00\x0A\x0A"
				"\x0C\xC7\x00\x0A\x0A"
				"\x0C\xC7\x09\x0E\x0E"
			), createPalette_DefaultEGA());

			this->sizedContent({8, 4}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x0A\x05\x01"
				"\x08"
				"\x00\x00" "\x00\x00" "\x07\x00" "\x03\x00"
				"\x4B\x00" "\x4B\x00"
			) + this->pcxPal + STRING_WITH_NULLS(
				"\x00"
				"\x01"
				"\x08\x00" "\x01\x00"
				"\x00\x00" "\x00\x00"
			) + std::string(54, '\x00') + STRING_WITH_NULLS(
				"\xC8\x0F"
				"\x0C\xC6\x00\x0A"
				"\x0C\xC6\x00\x0A"
				"\x0C\xC6\x09\x0E"
			), createPalette_DefaultEGA());

			// RLE check.  Ensure RLE codes run across scanlines.
			this->sizedContent({16, 3}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x0A\x05\x01"
				"\x08"
				"\x00\x00" "\x00\x00" "\x0F\x00" "\x02\x00"
				"\x4B\x00" "\x4B\x00"
			) + this->pcxPal + STRING_WITH_NULLS(
				"\x00"
				"\x01"
				"\x10\x00" "\x01\x00"
				"\x00\x00" "\x00\x00"
			) + std::string(54, '\x00') + STRING_WITH_NULLS(
				"\xD0\x0F"
				"\xD0\x0F"
				"\xD0\x0F"
			), createPalette_DefaultEGA(), std::string(16 * 3, '\x0F'));

			// Check RLE-across-scanlines when padding is involved due to the image
			// width not being a multiple of 2.
			this->sizedContent({11, 4}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x0A\x05\x01"
				"\x08"
				"\x00\x00" "\x00\x00" "\x0A\x00" "\x03\x00"
				"\x4B\x00" "\x4B\x00"
			) + this->pcxPal + STRING_WITH_NULLS(
				"\x00"
				"\x01"
				"\x0C\x00" "\x01\x00"
				"\x00\x00" "\x00\x00"
			) + std::string(54, '\x00') + STRING_WITH_NULLS(
				"\xCC\x0F"
				"\xCC\x0F"
				"\xCC\x0F"
				"\xCC\x0F"
			), createPalette_DefaultEGA(), STRING_WITH_NULLS(
				"\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F" // 0x0F follows (in source data) as padding
				"\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F" // 0x0F follows (in source data) as padding
				"\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F" // 0x0F follows (in source data) as padding
				"\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F" // 0x0F follows (in source data) as padding
			));

			// c00: Initial state
			this->isInstance(ImageType::DefinitelyYes, this->initialstate());

			// c01: Bad signature
			auto c01 = this->initialstate();
			c01[0] = 0xFF;
			this->isInstance(ImageType::DefinitelyNo, c01);

			// c02: Unsupported version
			auto c02 = this->initialstate();
			c02[1] = 0xFF;
			this->isInstance(ImageType::DefinitelyNo, c02);

			// c03: Wrong number of bits per plane
			auto c03 = this->initialstate();
			c03[3] = 0x01;
			this->isInstance(ImageType::DefinitelyNo, c03);

			// c04: Wrong number of planes
			auto c04 = this->initialstate();
			c04[65] = 0x04;
			this->isInstance(ImageType::DefinitelyNo, c04);

			// c05: Too short
			this->isInstance(ImageType::DefinitelyNo, this->initialstate().substr(0, 127));
		}

		virtual std::string initialstate() const
		{
			return STRING_WITH_NULLS(
				"\x0A\x05\x01"
				"\x08"
				"\x00\x00" "\x00\x00" "\x07\x00" "\x07\x00"
				"\x4B\x00" "\x4B\x00"
			) + this->pcxPal + STRING_WITH_NULLS(
				"\x00"
				"\x01"
				"\x08\x00" "\x01\x00"
				"\x00\x00" "\x00\x00"
			) + std::string(54, '\x00') + STRING_WITH_NULLS(
				"\xC8\x0F"
				"\x0C\xC6\x00\x0A"
				"\x0C\xC6\x00\x0A"
				"\x0C\xC6\x00\x0A"
				"\x0C\xC6\x00\x0A"
				"\x0C\xC6\x00\x0A"
				"\x0C\xC6\x00\x0A"
				"\x0C\xC6\x09\x0E"
			);
		}

		void test_truncated_image()
		{
			this->test_sizedContent_read_pix(Point{8, 8},
				ImageType::DefinitelyYes,
				STRING_WITH_NULLS(
					"\x0A\x05\x01"
					"\x08"
					"\x00\x00" "\x00\x00" "\x07\x00" "\x07\x00"
					"\x4B\x00" "\x4B\x00"
				) + this->pcxPal + STRING_WITH_NULLS(
					"\x00"
					"\x01"
					"\x08\x00" "\x01\x00"
					"\x00\x00" "\x00\x00"
				) + std::string(54, '\x00') + STRING_WITH_NULLS(
					"\xC8\x0F"
					"\x0C\xC6\x00\x0A"
					"\x0C\xC6\x00\x0A"
					"\x0C\xC6\x00\x0A"
					"\x0C\xC6\x00\x0A"
					"\x0C\xC6\x00\x0A"
					"\x0C\xC6\x00\x0A"
					"\x0C\xC6\x09"
				), createPalette_DefaultEGA(), STRING_WITH_NULLS(
				"\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x09\x09\x09\x09\x09\x09\x00"
			));
			return;
		}

		void test_truncated_rle()
		{
			this->test_sizedContent_read_pix(Point{8, 8},
				ImageType::DefinitelyYes,
				STRING_WITH_NULLS(
					"\x0A\x05\x01"
					"\x08"
					"\x00\x00" "\x00\x00" "\x07\x00" "\x07\x00"
					"\x4B\x00" "\x4B\x00"
				) + this->pcxPal + STRING_WITH_NULLS(
					"\x00"
					"\x01"
					"\x08\x00" "\x01\x00"
					"\x00\x00" "\x00\x00"
				) + std::string(54, '\x00') + STRING_WITH_NULLS(
					"\xC8\x0F"
					"\x0C\xC6\x00\x0A"
					"\x0C\xC6\x00\x0A"
					"\x0C\xC6\x00\x0A"
					"\x0C\xC6\x00\x0A"
					"\x0C\xC6\x00\x0A"
					"\x0C\xC6\x00\x0A"
					"\x0C\xC6\x09\xC2"
				), createPalette_DefaultEGA(), STRING_WITH_NULLS(
				"\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x09\x09\x09\x09\x09\x09\x00"
			));
			return;
		}

	private:
		std::string pcxPal;
};

class test_img_pcx_8b1p_norle: public test_image
{
	public:
		test_img_pcx_8b1p_norle()
		{
			this->type = "img-pcx-8b1p-norle";
			this->hasMask = false;
			this->hasHitmask = false;

			this->skipInstDetect.push_back("img-pcx-8b1p");

			this->pcxPal = STRING_WITH_NULLS(
				"\x00\x00\x00"
				"\x00\x00\xAA"
				"\x00\xAA\x00"
				"\x00\xAA\xAA"
				"\xAA\x00\x00"
				"\xAA\x00\xAA"
				"\xAA\x55\x00"
				"\xAA\xAA\xAA"
				"\x55\x55\x55"
				"\x55\x55\xFF"
				"\x55\xFF\x55"
				"\x55\xFF\xFF"
				"\xFF\x55\x55"
				"\xFF\x55\xFF"
				"\xFF\xFF\x55"
				"\xFF\xFF\xFF"
			);
		}

		void addTests()
		{
			this->test_image::addTests();

			ADD_IMAGE_TEST(false, &test_img_pcx_8b1p_norle::test_truncated_image);

			this->sizedContent({8, 8}, ImageType::DefinitelyYes, this->initialstate(),
				createPalette_DefaultEGA());

			this->sizedContent({16, 16}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x0A\x05\x00"
				"\x08"
				"\x00\x00" "\x00\x00" "\x0F\x00" "\x0F\x00"
				"\x4B\x00" "\x4B\x00"
			) + this->pcxPal + STRING_WITH_NULLS(
				"\x00"
				"\x01"
				"\x10\x00" "\x01\x00"
				"\x00\x00" "\x00\x00"
			) + std::string(54, '\x00') + STRING_WITH_NULLS(
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
			), createPalette_DefaultEGA());

			this->sizedContent({9, 9}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x0A\x05\x00"
				"\x08"
				"\x00\x00" "\x00\x00" "\x08\x00" "\x08\x00"
				"\x4B\x00" "\x4B\x00"
			) + this->pcxPal + STRING_WITH_NULLS(
				"\x00"
				"\x01"
				"\x0A\x00" "\x01\x00"
				"\x00\x00" "\x00\x00"
			) + std::string(54, '\x00') + STRING_WITH_NULLS(
				"\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F" // last byte on each scanline is padding
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x0A\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x0A\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x0A\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x0A\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x0A\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x0A\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x00\x0A\x0A"
				"\x0C\x09\x09\x09\x09\x09\x09\x09\x0E\x0E"
			), createPalette_DefaultEGA());

			this->sizedContent({8, 4}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x0A\x05\x00"
				"\x08"
				"\x00\x00" "\x00\x00" "\x07\x00" "\x03\x00"
				"\x4B\x00" "\x4B\x00"
			) + this->pcxPal + STRING_WITH_NULLS(
				"\x00"
				"\x01"
				"\x08\x00" "\x01\x00"
				"\x00\x00" "\x00\x00"
			) + std::string(54, '\x00') + STRING_WITH_NULLS(
				"\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x09\x09\x09\x09\x09\x09\x0E"
			), createPalette_DefaultEGA());

			// c00: Initial state
			this->isInstance(ImageType::DefinitelyYes, this->initialstate());
		}

		virtual std::string initialstate() const
		{
			return STRING_WITH_NULLS(
				"\x0A\x05\x00"
				"\x08"
				"\x00\x00" "\x00\x00" "\x07\x00" "\x07\x00"
				"\x4B\x00" "\x4B\x00"
			) + this->pcxPal + STRING_WITH_NULLS(
				"\x00"
				"\x01"
				"\x08\x00" "\x01\x00"
				"\x00\x00" "\x00\x00"
			) + std::string(54, '\x00') + STRING_WITH_NULLS(
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

		void test_truncated_image()
		{
			this->test_sizedContent_read_pix(Point{8, 8},
				ImageType::DefinitelyYes,
				STRING_WITH_NULLS(
					"\x0A\x05\x00"
					"\x08"
					"\x00\x00" "\x00\x00" "\x07\x00" "\x07\x00"
					"\x4B\x00" "\x4B\x00"
				) + this->pcxPal + STRING_WITH_NULLS(
					"\x00"
					"\x01"
					"\x08\x00" "\x01\x00"
					"\x00\x00" "\x00\x00"
				) + std::string(54, '\x00') + STRING_WITH_NULLS(
					"\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F"
					"\x0C\x00\x00\x00\x00\x00\x00\x0A"
					"\x0C\x00\x00\x00\x00\x00\x00\x0A"
					"\x0C\x00\x00\x00\x00\x00\x00\x0A"
					"\x0C\x00\x00\x00\x00\x00\x00\x0A"
					"\x0C\x00\x00\x00\x00\x00\x00\x0A"
					"\x0C\x00\x00\x00\x00\x00\x00\x0A"
					"\x0C\x09\x09\x09\x09\x09\x09"
				), createPalette_DefaultEGA(), STRING_WITH_NULLS(
				"\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x09\x09\x09\x09\x09\x09\x00"
			));
			return;
		}

	private:
		std::string pcxPal;
};

IMPLEMENT_TESTS(img_pcx_8b1p);
IMPLEMENT_TESTS(img_pcx_8b1p_norle);
