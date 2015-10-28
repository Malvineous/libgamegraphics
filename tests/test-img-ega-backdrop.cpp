/**
 * @file  test-img-ega-backdrop.cpp
 * @brief Test code for standard VGA full-screen image handler.
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

class test_img_ega_backdrop: public test_image
{
	public:
		test_img_ega_backdrop()
		{
			this->type = "img-cosmo-backdrop";
			this->hasMask = false;
			this->hasHitmask = false;
		}

		void addTests()
		{
			this->test_image::addTests();

			this->sizedContent({320, 144}, ImageType::PossiblyYes,
				this->initialstate());

			// c00: Initial state
			this->isInstance(ImageType::PossiblyYes, this->initialstate());

			// c01: Too short
			this->isInstance(ImageType::DefinitelyNo,
				this->initialstate().substr(1));
		}

		virtual std::string initialstate() const
		{
			std::string content;

			// Top-left
			content += STRING_WITH_NULLS(
				"\xFF\xFF\xFF\xFF"
				"\x00\x00\x80\x80"
				"\x00\x00\x80\x80"
				"\x00\x00\x80\x80"
				"\x00\x00\x80\x80"
				"\x00\x00\x80\x80"
				"\x00\x00\x80\x80"
				"\x00\x00\x80\x80"
			);

			// Top-middle
			for (int x = 1; x < 39; x++) {
				content += STRING_WITH_NULLS(
					"\xFF\xFF\xFF\xFF"
					"\x00\x00\x00\x00"
					"\x00\x00\x00\x00"
					"\x00\x00\x00\x00"
					"\x00\x00\x00\x00"
					"\x00\x00\x00\x00"
					"\x00\x00\x00\x00"
					"\x00\x00\x00\x00"
				);
			}

			// Top-right
			content += STRING_WITH_NULLS(
				"\xFF\xFF\xFF\xFF"
				"\x00\x01\x00\x01"
				"\x00\x01\x00\x01"
				"\x00\x01\x00\x01"
				"\x00\x01\x00\x01"
				"\x00\x01\x00\x01"
				"\x00\x01\x00\x01"
				"\x00\x01\x00\x01"
			);

			for (int y = 1; y < 17; y++) {
				// Mid-left
				content += STRING_WITH_NULLS(
					"\x00\x00\x80\x80"
					"\x00\x00\x80\x80"
					"\x00\x00\x80\x80"
					"\x00\x00\x80\x80"
					"\x00\x00\x80\x80"
					"\x00\x00\x80\x80"
					"\x00\x00\x80\x80"
					"\x00\x00\x80\x80"
				);

				// Mid-middle
				for (int x = 1; x < 39; x++) {
					content += STRING_WITH_NULLS(
						"\x00\x00\x00\x00"
						"\x00\x00\x00\x00"
						"\x00\x00\x00\x00"
						"\x00\x00\x00\x00"
						"\x00\x00\x00\x00"
						"\x00\x00\x00\x00"
						"\x00\x00\x00\x00"
						"\x00\x00\x00\x00"
					);
				}

				// Mid-right
				content += STRING_WITH_NULLS(
					"\x00\x01\x00\x01"
					"\x00\x01\x00\x01"
					"\x00\x01\x00\x01"
					"\x00\x01\x00\x01"
					"\x00\x01\x00\x01"
					"\x00\x01\x00\x01"
					"\x00\x01\x00\x01"
					"\x00\x01\x00\x01"
				);
			}

			// Bottom-left
			content += STRING_WITH_NULLS(
				"\x00\x00\x80\x80"
				"\x00\x00\x80\x80"
				"\x00\x00\x80\x80"
				"\x00\x00\x80\x80"
				"\x00\x00\x80\x80"
				"\x00\x00\x80\x80"
				"\x00\x00\x80\x80"
				"\x7F\x00\x80\xFF"
			);

			// Bottom-middle
			for (int x = 1; x < 39; x++) {
				content += STRING_WITH_NULLS(
					"\x00\x00\x00\x00"
					"\x00\x00\x00\x00"
					"\x00\x00\x00\x00"
					"\x00\x00\x00\x00"
					"\x00\x00\x00\x00"
					"\x00\x00\x00\x00"
					"\x00\x00\x00\x00"
					"\xFF\x00\x00\xFF"
				);
			}

			// Bottom-right
			content += STRING_WITH_NULLS(
				"\x00\x01\x00\x01"
				"\x00\x01\x00\x01"
				"\x00\x01\x00\x01"
				"\x00\x01\x00\x01"
				"\x00\x01\x00\x01"
				"\x00\x01\x00\x01"
				"\x00\x01\x00\x01"
				"\xFE\x01\x01\xFF"
			);

			return content;
		}
};

IMPLEMENT_TESTS(img_ega_backdrop);
