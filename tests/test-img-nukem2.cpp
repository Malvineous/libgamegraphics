/**
 * @file  test-img-nukem2.cpp
 * @brief Test code for Duke Nukem II full-screen images.
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

class test_img_nukem2: public test_image
{
	public:
		test_img_nukem2()
		{
			this->type = "img-nukem2";
			this->hasMask = false;
			this->hasHitmask = false;

			this->palette = std::make_shared<Palette>();
			this->palette->push_back({  0,   0,   0});
			this->palette->push_back({ 32,  32,  32});
			this->palette->push_back({ 65,  65,  65});
			this->palette->push_back({ 97,  97,  97});
			this->palette->push_back({130, 130, 130});
			this->palette->push_back({162, 162, 162});
			this->palette->push_back({195, 195, 195});
			this->palette->push_back({227, 227, 227});
			this->palette->push_back({255, 255, 255});
			this->palette->push_back({  0,   0, 255});
			this->palette->push_back({  0, 255,   0});
			this->palette->push_back({  0, 255, 255});
			this->palette->push_back({255,   0,   0});
			this->palette->push_back({255,   0, 255});
			this->palette->push_back({255, 255,   0});
			this->palette->push_back({255, 255, 255});
		}

		void addTests()
		{
			this->test_image::addTests();

			this->sizedContent({320, 200}, ImageType::DefinitelyYes,
				this->initialstate());

			// c00: Initial state
			this->isInstance(ImageType::DefinitelyYes, this->initialstate());

			// c01: Wrong size
			this->isInstance(ImageType::DefinitelyNo, this->initialstate() + "test");

			// c02: Palette entry out of range
			auto c02 = this->initialstate();
			c02[32001] = 0x41;
			this->isInstance(ImageType::DefinitelyNo, c02);
		}

		virtual std::string initialstate() const
		{
			std::string content;

			// Blue
			content += std::string(40, '\xFF');
			for (int y = 1; y < 199; y++) {
				content += '\x00';
				content += std::string(38, '\x00');
				content += '\x00';
			}
			content += '\x7F';
			content += std::string(38, '\xFF');
			content += '\xFE';

			// Green
			content += std::string(40, '\xFF');
			for (int y = 1; y < 199; y++) {
				content += '\x00';
				content += std::string(38, '\x00');
				content += '\x01';
			}
			content += '\x00';
			content += std::string(38, '\x00');
			content += '\x01';

			// Red
			content += std::string(40, '\xFF');
			for (int y = 1; y < 199; y++) {
				content += '\x80';
				content += std::string(38, '\x00');
				content += '\x00';
			}
			content += '\x80';
			content += std::string(38, '\x00');
			content += '\x01';

			// Intensity
			content += std::string(40, '\xFF');
			for (int y = 1; y < 199; y++) {
				content += '\x80';
				content += std::string(38, '\x00');
				content += '\x01';
			}
			content += '\xFF';
			content += std::string(38, '\xFF');
			content += '\xFF';

			// Add palette, make it different to the default ones so we can detect
			// if we end up with one of those by accident.
			content += STRING_WITH_NULLS(
				"\x00\x00\x00"
				"\x08\x08\x08"
				"\x10\x10\x10"
				"\x18\x18\x18"
				"\x20\x20\x20"
				"\x28\x28\x28"
				"\x30\x30\x30"
				"\x38\x38\x38"
				"\x3F\x3F\x3F"
				"\x00\x00\x3F"
				"\x00\x3F\x00"
				"\x00\x3F\x3F"
				"\x3F\x00\x00"
				"\x3F\x00\x3F"
				"\x3F\x3F\x00"
				"\x3F\x3F\x3F"
			);
			return content;
		}
};

IMPLEMENT_TESTS(img_nukem2);
