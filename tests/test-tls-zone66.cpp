/**
 * @file  test-tls-zone66.cpp
 * @brief Test code for Tileset_Zone66 class.
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

#include "test-tileset.hpp"

class test_tls_zone66: public test_tileset
{
	public:
		test_tls_zone66()
		{
			this->type = "tls-zone66";
			this->lenMaxFilename = -1;

			this->content[0] = this->tile1();
			this->content[1] = this->tile2();
			this->content[2] = this->tile3();
			this->content[3] = this->tile4();
			this->content0_overwritten = this->tile1_larger();
			this->content0_largeSize = 0x42 + 5;

			this->firstTileDims = {8, 8};
		}

		void addTests()
		{
			this->test_tileset::addTests();

			// c00: Initial state
			this->isInstance(ArchiveType::Certainty::DefinitelyYes, this->initialstate());

			// c01: Too short
			this->isInstance(ArchiveType::Certainty::DefinitelyNo, STRING_WITH_NULLS(
				"\x02\x00\x00"
			));

			// c02: Invalid empty tileset
			this->isInstance(ArchiveType::Certainty::DefinitelyNo, STRING_WITH_NULLS(
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
			));

			// c03: First tile always starts at offset 0
			this->isInstance(ArchiveType::Certainty::DefinitelyNo, STRING_WITH_NULLS(
				"\x02\x00\x00\x00"
				"\x01\x00\x00\x00"
				"\x54\x00\x00\x00"
				) +
				this->tile1() +
				this->tile2()
			);

			// c04: Make sure offsets are increasing
			this->isInstance(ArchiveType::Certainty::DefinitelyNo, STRING_WITH_NULLS(
				"\x02\x00\x00\x00"
				"\x54\x00\x00\x00"
				"\x00\x00\x00\x00"
				) +
				this->tile1() +
				this->tile2()
			);

			// c05: Tile past EOF
			this->isInstance(ArchiveType::Certainty::DefinitelyNo, STRING_WITH_NULLS(
				"\x02\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x54\x01\x00\x00"
				) +
				this->tile1() +
				this->tile2()
			);
		}

		virtual std::string tile1() const
		{
			return STRING_WITH_NULLS(
				"\x08\x00\x08\x00"
				"\x08\x0F\x00\x0F\x0F\x0F\x0F\x0F\x0F\xFE"
				"\x01\x0C\xFD\x06\x01\x0A\xFE" // original: "\x08\x0C\x00\x00\x00\x00\x00\x00\x0A\xFE"
				"\x01\x0C\xFD\x06\x01\x0A\xFE"
				"\x01\x0C\xFD\x06\x01\x0A\xFE"
				"\x01\x0C\xFD\x06\x01\x0A\xFE"
				"\x01\x0C\xFD\x06\x01\x0A\xFE"
				"\x01\x0C\xFD\x06\x01\x0A\xFE"
				"\x08\x0C\x09\x09\x09\x09\x09\x00\x0e"
				"\xFF"
			);
		}

		virtual std::string tile2() const
		{
			return STRING_WITH_NULLS(
				"\x08\x00\x08\x00"
				"\x08\x0F\x01\x0F\x0F\x0F\x0F\x0F\x0F\xFE"
				"\x01\x0C\xFD\x06\x01\x0A\xFE"
				"\x01\x0C\xFD\x06\x01\x0A\xFE"
				"\x01\x0C\xFD\x06\x01\x0A\xFE"
				"\x01\x0C\xFD\x06\x01\x0A\xFE"
				"\x01\x0C\xFD\x06\x01\x0A\xFE"
				"\x01\x0C\xFD\x06\x01\x0A\xFE"
				"\x08\x0C\x09\x09\x09\x09\x09\x01\x0e"
				"\xFF"
			);
		}

		virtual std::string tile3() const
		{
			return STRING_WITH_NULLS(
				"\x08\x00\x08\x00"
				"\x08\x0F\x02\x0F\x0F\x0F\x0F\x0F\x0F\xFE"
				"\x01\x0C\xFD\x06\x01\x0A\xFE"
				"\x01\x0C\xFD\x06\x01\x0A\xFE"
				"\x01\x0C\xFD\x06\x01\x0A\xFE"
				"\x01\x0C\xFD\x06\x01\x0A\xFE"
				"\x01\x0C\xFD\x06\x01\x0A\xFE"
				"\x01\x0C\xFD\x06\x01\x0A\xFE"
				"\x08\x0C\x09\x09\x09\x09\x09\x02\x0e"
				"\xFF"
			);
		}

		virtual std::string tile4() const
		{
			return STRING_WITH_NULLS(
				"\x08\x00\x08\x00"
				"\x08\x0F\x03\x0F\x0F\x0F\x0F\x0F\x0F\xFE"
				"\x01\x0C\xFD\x06\x01\x0A\xFE"
				"\x01\x0C\xFD\x06\x01\x0A\xFE"
				"\x01\x0C\xFD\x06\x01\x0A\xFE"
				"\x01\x0C\xFD\x06\x01\x0A\xFE"
				"\x01\x0C\xFD\x06\x01\x0A\xFE"
				"\x01\x0C\xFD\x06\x01\x0A\xFE"
				"\x08\x0C\x09\x09\x09\x09\x09\x03\x0e"
				"\xFF"
			);
		}

		virtual std::string tile1_larger_empty() const
		{
			return this->tile1() + STRING_WITH_NULLS("\x00\x00\x00\x00\x00");
		}

		virtual std::string tile1_larger() const
		{
			return STRING_WITH_NULLS(
				"\x09\x00\x09\x00"
				"\x08\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\xFE"
				"\x08\x0C\x00\x00\x00\x00\x00\x00\x00\x0A\xFE"
				"\x08\x0C\x00\x00\x00\x00\x00\x00\x00\x0A\xFE"
				"\x08\x0C\x00\x00\x00\x00\x00\x00\x00\x0A\xFE"
				"\x08\x0C\x00\x00\x00\x00\x00\x00\x00\x0A\xFE"
				"\x08\x0C\x00\x00\x00\x00\x00\x00\x00\x0A\xFE"
				"\x08\x0C\x00\x00\x00\x00\x00\x00\x00\x0A\xFE"
				"\x08\x0C\x00\x00\x00\x00\x00\x00\x00\x0A\xFE"
				"\x08\x0C\x09\x09\x09\x09\x09\x09\x09\x0e"
				"\xFF"
			);
		}

		virtual std::string tile1_smaller() const
		{
			return STRING_WITH_NULLS(
				"\x07\x00\x07\x00"
				"\x08\x0F\x0F\x0F\x0F\x0F\x0F\x0F\xFE"
				"\x08\x0C\x00\x00\x00\x00\x00\x0A\xFE"
				"\x08\x0C\x00\x00\x00\x00\x00\x0A\xFE"
				"\x08\x0C\x00\x00\x00\x00\x00\x0A\xFE"
				"\x08\x0C\x00\x00\x00\x00\x00\x0A\xFE"
				"\x08\x0C\x00\x00\x00\x00\x00\x0A\xFE"
				"\x08\x0C\x09\x09\x09\x09\x09\x0e"
				"\xFF"
			);
		}

		virtual std::string initialstate()
		{
			return
				STRING_WITH_NULLS(
					"\x02\x00\x00\x00"
					"\x00\x00\x00\x00"
					"\x42\x00\x00\x00"
				) +
				this->tile1() +
				this->tile2()
			;
		}

		virtual std::string rename()
		{
			throw stream::error("This tileset does not have any tilenames.");
		}

		virtual std::string insert_end()
		{
			return
				STRING_WITH_NULLS(
					"\x03\x00\x00\x00"
					"\x00\x00\x00\x00"
					"\x42\x00\x00\x00"
					"\x84\x00\x00\x00"
				) +
				this->tile1() +
				this->tile2() +
				this->tile3()
			;
		}

		virtual std::string insert_mid()
		{
			return
				STRING_WITH_NULLS(
					"\x03\x00\x00\x00"
					"\x00\x00\x00\x00"
					"\x42\x00\x00\x00"
					"\x84\x00\x00\x00"
				) +
				this->tile1() +
				this->tile3() +
				this->tile2()
			;
		}

		virtual std::string insert2()
		{
			return
				STRING_WITH_NULLS(
					"\x04\x00\x00\x00"
					"\x00\x00\x00\x00"
					"\x42\x00\x00\x00"
					"\x84\x00\x00\x00"
					"\xC6\x00\x00\x00"
				) +
				this->tile1() +
				this->tile3() +
				this->tile4() +
				this->tile2()
			;
		}

		virtual std::string remove()
		{
			return
				STRING_WITH_NULLS(
					"\x01\x00\x00\x00"
					"\x00\x00\x00\x00"
				) +
				this->tile2()
			;
		}

		virtual std::string remove2()
		{
			return STRING_WITH_NULLS(
				"\x00\x00\x00\x00"
			);
		}

		virtual std::string insert_remove()
		{
			return
				STRING_WITH_NULLS(
					"\x02\x00\x00\x00"
					"\x00\x00\x00\x00"
					"\x42\x00\x00\x00"
				) +
				this->tile3() +
				this->tile2()
			;
		}

		virtual std::string move()
		{
			return
				STRING_WITH_NULLS(
					"\x02\x00\x00\x00"
					"\x00\x00\x00\x00"
					"\x42\x00\x00\x00"
				) +
				this->tile2() +
				this->tile1()
			;
		}

		virtual std::string resize_larger()
		{
			return
				STRING_WITH_NULLS(
					"\x02\x00\x00\x00"
					"\x00\x00\x00\x00"
					"\x47\x00\x00\x00"
				) +
				this->tile1() + STRING_WITH_NULLS("\x00\x00\x00\x00\x00") +
				this->tile2()
			;
		}

		virtual std::string resize_smaller()
		{
			return
				STRING_WITH_NULLS(
					"\x02\x00\x00\x00"
					"\x00\x00\x00\x00"
					"\x0A\x00\x00\x00"
				) +
				this->tile1().substr(0, 10) +
				this->tile2()
			;
		}

		virtual std::string resize_write()
		{
			return
				STRING_WITH_NULLS(
					"\x02\x00\x00\x00"
					"\x00\x00\x00\x00"
					"\x67\x00\x00\x00"
				) +
				this->tile1_larger() +
				this->tile2()
			;
		}
};

class test_tls_zone66_fullscreen: public test_tileset
{
	public:
		test_tls_zone66_fullscreen()
		{
			this->type = "tls-zone66";
			this->lenMaxFilename = -1;

			this->content[0] = this->tile1();
			this->content[1] = this->tile2();
			this->content[2] = this->tile3();
			this->content[3] = this->tile4();

			// The filesize isn't really fixed, but as soon as the tiles are resized
			// we'll have to change the format which is a pain.  Plus once the image
			// is opened as an Image_VGARaw it can't be resized anyway.
			this->lenFilesizeFixed = 320 * 200;

			this->firstTileDims = {320, 200};
		}

		void addTests()
		{
			this->test_tileset::addTests();

			// c00: Initial state
			this->isInstance(ArchiveType::Certainty::DefinitelyYes, this->initialstate());
		}

		virtual std::string tile1() const
		{
			std::string content;

			content += STRING_WITH_NULLS("\x0F\x00") + std::string(318, '\x0F');
			for (int y = 1; y < 199; y++) {
				content += '\x0C';
				content += std::string(318, '\x00');
				content += '\x0A';
			}
			content += '\x0C';
			content += std::string(317, '\x09');
			content += STRING_WITH_NULLS("\x00\x0E");

			return content;
		}

		virtual std::string tile2() const
		{
			std::string content;

			content += STRING_WITH_NULLS("\x0F\x01") + std::string(318, '\x0F');
			for (int y = 1; y < 199; y++) {
				content += '\x0C';
				content += std::string(318, '\x00');
				content += '\x0A';
			}
			content += '\x0C';
			content += std::string(317, '\x09');
			content += STRING_WITH_NULLS("\x01\x0E");

			return content;
		}

		virtual std::string tile3() const
		{
			std::string content;

			content += STRING_WITH_NULLS("\x0F\x02") + std::string(318, '\x0F');
			for (int y = 1; y < 199; y++) {
				content += '\x0C';
				content += std::string(318, '\x00');
				content += '\x0A';
			}
			content += '\x0C';
			content += std::string(317, '\x09');
			content += STRING_WITH_NULLS("\x02\x0E");

			return content;
		}

		virtual std::string tile4() const
		{
			std::string content;

			content += STRING_WITH_NULLS("\x0F\x03") + std::string(318, '\x0F');
			for (int y = 1; y < 199; y++) {
				content += '\x0C';
				content += std::string(318, '\x00');
				content += '\x0A';
			}
			content += '\x0C';
			content += std::string(317, '\x09');
			content += STRING_WITH_NULLS("\x03\x0E");

			return content;
		}

		virtual std::string initialstate()
		{
			return
				STRING_WITH_NULLS(
					"\x02\x00\x00\x00"
					"\x00\x00\x00\x00"
					"\x00\xFA\x00\x00"
				) +
				this->tile1() +
				this->tile2()
			;
		}

		virtual std::string rename()
		{
			throw stream::error("This tileset does not have any tilenames.");
		}

		virtual std::string insert_end()
		{
			return
				STRING_WITH_NULLS(
					"\x03\x00\x00\x00"
					"\x00\x00\x00\x00"
					"\x00\xFA\x00\x00"
					"\x00\xF4\x01\x00"
				) +
				this->tile1() +
				this->tile2() +
				this->tile3()
			;
		}

		virtual std::string insert_mid()
		{
			return
				STRING_WITH_NULLS(
					"\x03\x00\x00\x00"
					"\x00\x00\x00\x00"
					"\x00\xFA\x00\x00"
					"\x00\xF4\x01\x00"
				) +
				this->tile1() +
				this->tile3() +
				this->tile2()
			;
		}

		virtual std::string insert2()
		{
			return
				STRING_WITH_NULLS(
					"\x04\x00\x00\x00"
					"\x00\x00\x00\x00"
					"\x00\xFA\x00\x00"
					"\x00\xF4\x01\x00"
					"\x00\xEE\x02\x00"
				) +
				this->tile1() +
				this->tile3() +
				this->tile4() +
				this->tile2()
			;
		}

		virtual std::string remove()
		{
			return
				STRING_WITH_NULLS(
					"\x01\x00\x00\x00"
					"\x00\x00\x00\x00"
				) +
				this->tile2()
			;
		}

		virtual std::string remove2()
		{
			return STRING_WITH_NULLS(
				"\x00\x00\x00\x00"
			);
		}

		virtual std::string insert_remove()
		{
			return
				STRING_WITH_NULLS(
					"\x02\x00\x00\x00"
					"\x00\x00\x00\x00"
					"\x00\xFA\x00\x00"
				) +
				this->tile3() +
				this->tile2()
			;
		}

		virtual std::string move()
		{
			return
				STRING_WITH_NULLS(
					"\x02\x00\x00\x00"
					"\x00\x00\x00\x00"
					"\x00\xFA\x00\x00"
				) +
				this->tile2() +
				this->tile1()
			;
		}

		virtual std::string resize_larger()
		{
			throw stream::error("Test not implemented.");
		}

		virtual std::string resize_smaller()
		{
			throw stream::error("Test not implemented.");
		}

		virtual std::string resize_write()
		{
			throw stream::error("Test not implemented.");
		}
};

IMPLEMENT_TESTS(tls_zone66);
IMPLEMENT_TESTS(tls_zone66_fullscreen);
