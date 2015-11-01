/**
 * @file   test-img-pcx-1b4p.cpp
 * @brief  Test code for PCX file handler (with EGA planar image data.)
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

class test_img_pcx_1b4p: public test_image
{
	public:
		test_img_pcx_1b4p()
		{
			this->type = "img-pcx-1b4p";
			this->hasMask = false;
			this->hasHitmask = false;

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

			ADD_IMAGE_TEST(false, &test_img_pcx_1b4p::test_truncated_image);
			ADD_IMAGE_TEST(false, &test_img_pcx_1b4p::test_truncated_rle);

			this->sizedContent({8, 8}, ImageType::DefinitelyYes, this->initialstate(),
				createPalette_DefaultEGA());

			this->sizedContent({16, 16}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x0A\x05\x01"
				"\x01"
				"\x00\x00" "\x00\x00" "\x0F\x00" "\x0F\x00"
				"\x4B\x00" "\x4B\x00"
			) + this->pcxPal + STRING_WITH_NULLS(
				"\x00"
				"\x04"
				"\x08\x00" "\x01\x00"
				"\x00\x00" "\x00\x00"
			) + std::string(54, '\x00') + STRING_WITH_NULLS(
				"\xC8\xFF"
				"\xC3\x00"     "\x01" "\x80\x00" "\x80\x01"
				"\xC3\x00"     "\x01" "\x80\x00" "\x80\x01"
				"\xC3\x00"     "\x01" "\x80\x00" "\x80\x01"
				"\xC3\x00"     "\x01" "\x80\x00" "\x80\x01"
				"\xC3\x00"     "\x01" "\x80\x00" "\x80\x01"
				"\xC3\x00"     "\x01" "\x80\x00" "\x80\x01"
				"\xC3\x00"     "\x01" "\x80\x00" "\x80\x01"
				"\xC3\x00"     "\x01" "\x80\x00" "\x80\x01"
				"\xC3\x00"     "\x01" "\x80\x00" "\x80\x01"
				"\xC3\x00"     "\x01" "\x80\x00" "\x80\x01"
				"\xC3\x00"     "\x01" "\x80\x00" "\x80\x01"
				"\xC3\x00"     "\x01" "\x80\x00" "\x80\x01"
				"\xC3\x00"     "\x01" "\x80\x00" "\x80\x01"
				"\xC3\x00"     "\x01" "\x80\x00" "\x80\x01"
				"\x7F\xC1\xFE" "\x00\x01" "\x80\x01" "\xC2\xFF"
			), createPalette_DefaultEGA());

			this->sizedContent({9, 9}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x0A\x05\x01"
				"\x01"
				"\x00\x00" "\x00\x00" "\x08\x00" "\x08\x00"
				"\x4B\x00" "\x4B\x00"
			) + this->pcxPal + STRING_WITH_NULLS(
				"\x00"
				"\x04"
				"\x08\x00" "\x01\x00"
				"\x00\x00" "\x00\x00"
			) + std::string(54, '\x00') + STRING_WITH_NULLS(
				"\xC8\xFF"
				"\xC3\x00"     "\x80" "\x80\x00" "\x80\x80"
				"\xC3\x00"     "\x80" "\x80\x00" "\x80\x80"
				"\xC3\x00"     "\x80" "\x80\x00" "\x80\x80"
				"\xC3\x00"     "\x80" "\x80\x00" "\x80\x80"
				"\xC3\x00"     "\x80" "\x80\x00" "\x80\x80"
				"\xC3\x00"     "\x80" "\x80\x00" "\x80\x80"
				"\xC3\x00"     "\x80" "\x80\x00" "\x80\x80"
				"\x7F\x7F" "\x00\xC3\x80"        "\xC2\xFF"
			), createPalette_DefaultEGA());

			this->sizedContent({8, 4}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x0A\x05\x01"
				"\x01"
				"\x00\x00" "\x00\x00" "\x07\x00" "\x03\x00"
				"\x4B\x00" "\x4B\x00"
			) + this->pcxPal + STRING_WITH_NULLS(
				"\x00"
				"\x04"
				"\x04\x00" "\x01\x00"
				"\x00\x00" "\x00\x00"
			) + std::string(54, '\x00') + STRING_WITH_NULLS(
				"\xC4\xFF" /* use same byte for padding (instead of 0x00) to help RLE */
				"\x00\x01\x80\x81"
				"\x00\x01\x80\x81"
				"\x7E\x01\x81\xC1\xFF"
			), createPalette_DefaultEGA());

			// RLE check.  Ensure RLE codes do not run across scanlines.
			this->sizedContent({16, 4}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x0A\x05\x01"
				"\x01"
				"\x00\x00" "\x00\x00" "\x0F\x00" "\x03\x00"
				"\x4B\x00" "\x4B\x00"
			) + this->pcxPal + STRING_WITH_NULLS(
				"\x00"
				"\x04"
				"\x08\x00" "\x01\x00"
				"\x00\x00" "\x00\x00"
			) + std::string(54, '\x00') + STRING_WITH_NULLS(
				"\xC8\xFF"
				"\xC8\xFF"
				"\xC8\xFF"
				"\xC8\xFF"
			), createPalette_DefaultEGA(), std::string(16 * 4, '\x0F'));

			// Check RLE-across-scanlines when padding is involved due to the image
			// width not being a multiple of 8.
			this->sizedContent({12, 4}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\x0A\x05\x01"
				"\x01"
				"\x00\x00" "\x00\x00" "\x0B\x00" "\x03\x00"
				"\x4B\x00" "\x4B\x00"
			) + this->pcxPal + STRING_WITH_NULLS(
				"\x00"
				"\x04"
				"\x08\x00" "\x01\x00"
				"\x00\x00" "\x00\x00"
			) + std::string(54, '\x00') + STRING_WITH_NULLS(
				"\xC8\xF0"
				"\xC8\xF0"
				"\xC8\xF0"
				"\xC8\xF0"
			), createPalette_DefaultEGA(), STRING_WITH_NULLS(
				"\x0F\x0F\x0F\x0F\x00\x00\x00\x00\x0F\x0F\x0F\x0F" // following 00 00 00 00 is in padding
				"\x0F\x0F\x0F\x0F\x00\x00\x00\x00\x0F\x0F\x0F\x0F"
				"\x0F\x0F\x0F\x0F\x00\x00\x00\x00\x0F\x0F\x0F\x0F"
				"\x0F\x0F\x0F\x0F\x00\x00\x00\x00\x0F\x0F\x0F\x0F"
			));

			// c00: Initial state
			this->isInstance(ImageType::DefinitelyYes, this->initialstate());

			// c01: Bad signature
			auto c01 = this->initialstate();
			c01[0] = 0xFFu;
			this->isInstance(ImageType::DefinitelyNo, c01);

			// c02: Unsupported version
			auto c02 = this->initialstate();
			c02[1] = 0xFFu;
			this->isInstance(ImageType::DefinitelyNo, c02);

			// c03: Wrong number of bits per plane
			auto c03 = this->initialstate();
			c03[3] = 0x08;
			this->isInstance(ImageType::DefinitelyNo, c03);

			// c04: Wrong number of planes
			auto c04 = this->initialstate();
			c04[65] = 0x01;
			this->isInstance(ImageType::DefinitelyNo, c04);

			// c05: Too short
			this->isInstance(ImageType::DefinitelyNo, this->initialstate().substr(0, 127));
		}

		virtual std::string initialstate() const
		{
			return STRING_WITH_NULLS(
				"\x0A\x05\x01"
				"\x01"
				"\x00\x00" "\x00\x00" "\x07\x00" "\x07\x00"
				"\x4B\x00" "\x4B\x00"
			) + this->pcxPal + STRING_WITH_NULLS(
				"\x00"
				"\x04"
				"\x04\x00" "\x01\x00"
				"\x00\x00" "\x00\x00"
			) + std::string(54, '\x00') + STRING_WITH_NULLS(
				// Every second byte is padding to bring each plane's scanline up to
				// two bytes.  The same byte is used again in case this helps RLE.
				"\xC4\xFF"
				"\x00\x01\x80\x81"
				"\x00\x01\x80\x81"
				"\x00\x01\x80\x81"
				"\x00\x01\x80\x81"
				"\x00\x01\x80\x81"
				"\x00\x01\x80\x81"
				"\x7E\x01\x81\xC1\xFF"
			);
		}

		void test_truncated_image()
		{
			this->test_sizedContent_read_pix(Point{8, 8},
				ImageType::DefinitelyYes,
				STRING_WITH_NULLS(
					"\x0A\x05\x01"
					"\x01"
					"\x00\x00" "\x00\x00" "\x07\x00" "\x07\x00"
					"\x4B\x00" "\x4B\x00"
				) + this->pcxPal + STRING_WITH_NULLS(
					"\x00"
					"\x04"
					"\x04\x00" "\x01\x00"
					"\x00\x00" "\x00\x00"
				) + std::string(54, '\x00') + STRING_WITH_NULLS(
					"\xC4\xFF"
					"\x00\x01\x80\x81"
					"\x00\x01\x80\x81"
					"\x00\x01\x80\x81"
					"\x00\x01\x80\x81"
					"\x00\x01\x80\x81"
					"\x00\x01\x80\x81"
					"\x7E\x01\x81"
				), createPalette_DefaultEGA(), STRING_WITH_NULLS(
				"\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x04\x01\x01\x01\x01\x01\x01\x06" // intensity plane now missing from this row
			));
			return;
		}

		void test_truncated_rle()
		{
			this->test_sizedContent_read_pix(Point{8, 8},
				ImageType::DefinitelyYes,
				STRING_WITH_NULLS(
					"\x0A\x05\x01"
					"\x01"
					"\x00\x00" "\x00\x00" "\x07\x00" "\x07\x00"
					"\x4B\x00" "\x4B\x00"
				) + this->pcxPal + STRING_WITH_NULLS(
					"\x00"
					"\x04"
					"\x04\x00" "\x01\x00"
					"\x00\x00" "\x00\x00"
				) + std::string(54, '\x00') + STRING_WITH_NULLS(
					"\xC4\xFF"
					"\x00\x01\x80\x81"
					"\x00\x01\x80\x81"
					"\x00\x01\x80\x81"
					"\x00\x01\x80\x81"
					"\x00\x01\x80\x81"
					"\x00\x01\x80\x81"
					"\x7E\x01\x81\xC2"
				), createPalette_DefaultEGA(), STRING_WITH_NULLS(
				"\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x0C\x00\x00\x00\x00\x00\x00\x0A"
				"\x04\x01\x01\x01\x01\x01\x01\x06" // intensity plane now missing from this row
			));
			return;
		}

	private:
		std::string pcxPal;
};

IMPLEMENT_TESTS(img_pcx_1b4p);
