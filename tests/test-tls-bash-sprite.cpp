/**
 * @file   test-tls-bash-sprite.cpp
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

#include "test-tileset.hpp"

class test_tls_bash_sprite: public test_tileset
{
	public:
		test_tls_bash_sprite()
		{
			this->type = "tls-bash-sprite";
			this->lenMaxFilename = -1;
			this->hasMask = true;
			this->hasHotspot = true;
			this->ptHotspot[0] = {1, 0};
			this->ptHotspot[1] = {2, 0};
			this->ptHotspot[2] = {3, 0};
			this->ptHotspot[3] = {4, 0};
			this->hasHitrect = true;
			this->ptHitrect[0] = {0, 1};
			this->ptHitrect[1] = {0, 2};
			this->ptHitrect[2] = {0, 3};
			this->ptHitrect[3] = {0, 4};

			this->header = STRING_WITH_NULLS("\xFF");

			this->content[0] = this->tile1();
			this->content[1] = this->tile2();
			this->content[2] = this->tile3();
			this->content[3] = this->tile4();

			this->content0_overwritten = STRING_WITH_NULLS(
				"\x00\x09\x09\x00"
				"\xFB\xFF\x00\x00\x00\x00\x05\x00"
				"\x02\x00\x7F\x00\x7F\x00\x7F\x00\x7F\x00\x7F\x00\x7F\x00\x7F\x00\x7F\x00\x7F"
				"\x01\xFF\x80\xFF\x80\xFF\x80\xFF\x80\xFF\x80\xFF\x80\xFF\x80\xFF\x80\xFF\x00"
				"\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x80"
				"\x04\xFF\x80\xFF\x80\xFF\x80\xFF\x80\xFF\x80\xFF\x80\xFF\x80\xFF\x80\xFF\x80"
				"\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x80"
				"\x00"
			);
			this->content0_largeSize = 0x3A + 5;

			this->firstTileDims = {8, 8};
		}

		void addTests()
		{
			this->test_tileset::addTests();

			// c00: Initial state
			this->isInstance(ArchiveType::DefinitelyYes, this->initialstate());

			// c01: Minimum file length
			this->isInstance(ArchiveType::DefinitelyYes, STRING_WITH_NULLS(
				"\xFF"
			));

			// c02: Correct fields, but less than minimum file length
			this->isInstance(ArchiveType::DefinitelyNo, STRING_WITH_NULLS(
				"\xFF"
				"\x00\x0B"
				"\x00\x08\x08\x00"
				"\x00\x00\x00\x00\x08\x00\x08"
			));

			// c03: Bad signature byte
			this->isInstance(ArchiveType::DefinitelyNo,
				STRING_WITH_NULLS("\xFE") +
				STRING_WITH_NULLS("\x3A\x00") +
				this->tile1() +
				STRING_WITH_NULLS("\x3A\x00") +
				this->tile2()
			);

			// c04: Correct fields, but less than minimum image length
			this->isInstance(ArchiveType::DefinitelyNo,
				STRING_WITH_NULLS("\xFF") +
				STRING_WITH_NULLS("\x3A\x00") +
				this->tile1() +
				STRING_WITH_NULLS(
					"\x00\x0B"
					"\x00\x08\x08\x00"
					"\x00\x00\x00\x00\x08\x00\x08"
				)
			);

			// c05: Image goes past EOF
			this->isInstance(ArchiveType::DefinitelyNo,
				this->header +
				STRING_WITH_NULLS("\x3A\x00") +
				this->tile1() +
				STRING_WITH_NULLS("\xFF\x00") +
				this->tile2()
			);

			std::string c06 =
				this->header +
				STRING_WITH_NULLS(
					"\x3A\x00"
					"\x00\x08\x08\x00"
					"\x00\x00\x00\x00\x08\x00\x08\x00"
					"\x01\x00\x00\x00\x00\x00\x00\x00\x00"
					"\x01\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFE"
					"\x02\x00\x00\x00\x00\x00\x00\x00\x01"
					"\x04\x00\x00\x00\x00\x00\x00\x00\x01"
					"\x08\x00\x00\x00\x00\x00\x00\x00\x01"
				);

			// c06a: Confirm c06 data is good for next test
			this->isInstance(ArchiveType::DefinitelyYes,
				c06 + STRING_WITH_NULLS("\x00")
			);

			// c06b: Last plane ID byte is non-zero
			this->isInstance(ArchiveType::DefinitelyNo,
				c06 + STRING_WITH_NULLS("\x01")
			);
		}

		virtual std::string tile1() const
		{
			return STRING_WITH_NULLS(
				"\x00\x08\x08\x00"
				"\xFF\xFF\x00\x00\x00\x00\x01\x00"
				"\x01\x00\x7E\x7E\x7E\x7E\x7E\x7E\x00"
				"\x01\xBF\x00\x00\x00\x00\x00\x00\x7C"
				"\x02\xBF\x01\x01\x01\x01\x01\x01\x01"
				"\x04\xBF\x80\x80\x80\x80\x80\x80\x81"
				"\x08\xBF\x81\x81\x81\x81\x81\x81\xFD"
				"\x00"
			);
		}

		virtual std::string tile2() const
		{
			return STRING_WITH_NULLS(
				"\x00\x08\x08\x00"
				"\xFE\xFF\x00\x00\x00\x00\x02\x00"
				"\x01\x00\x7E\x7E\x7E\x7E\x7E\x7E\x00"
				"\x01\xFF\x00\x00\x00\x00\x00\x00\x7E"
				"\x02\xBF\x01\x01\x01\x01\x01\x01\x01"
				"\x04\xBF\x80\x80\x80\x80\x80\x80\x81"
				"\x08\xBF\x81\x81\x81\x81\x81\x81\xFD"
				"\x00"
			);
		}

		virtual std::string tile3() const
		{
			return STRING_WITH_NULLS(
				"\x00\x08\x08\x00"
				"\xFD\xFF\x00\x00\x00\x00\x03\x00"
				"\x01\x00\x7E\x7E\x7E\x7E\x7E\x7E\x00"
				"\x01\xBF\x00\x00\x00\x00\x00\x00\x7C"
				"\x02\xFF\x01\x01\x01\x01\x01\x01\x03"
				"\x04\xBF\x80\x80\x80\x80\x80\x80\x81"
				"\x08\xBF\x81\x81\x81\x81\x81\x81\xFD"
				"\x00"
			);
		}

		virtual std::string tile4() const
		{
			return STRING_WITH_NULLS(
				"\x00\x08\x08\x00"
				"\xFC\xFF\x00\x00\x00\x00\x04\x00"
				"\x01\x00\x7E\x7E\x7E\x7E\x7E\x7E\x00"
				"\x01\xFF\x00\x00\x00\x00\x00\x00\x7E"
				"\x02\xFF\x01\x01\x01\x01\x01\x01\x03"
				"\x04\xBF\x80\x80\x80\x80\x80\x80\x81"
				"\x08\xBF\x81\x81\x81\x81\x81\x81\xFD"
				"\x00"
			);
		}

		virtual std::string initialstate()
		{
			return
				this->header +
				STRING_WITH_NULLS("\x3A\x00") +
				this->tile1() +
				STRING_WITH_NULLS("\x3A\x00") +
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
				this->header +
				STRING_WITH_NULLS("\x3A\x00") +
				this->tile1() +
				STRING_WITH_NULLS("\x3A\x00") +
				this->tile2() +
				STRING_WITH_NULLS("\x3A\x00") +
				this->tile3()
			;
		}

		virtual std::string insert_mid()
		{
			return
				this->header +
				STRING_WITH_NULLS("\x3A\x00") +
				this->tile1() +
				STRING_WITH_NULLS("\x3A\x00") +
				this->tile3() +
				STRING_WITH_NULLS("\x3A\x00") +
				this->tile2()
			;
		}

		virtual std::string insert2()
		{
			return
				this->header +
				STRING_WITH_NULLS("\x3A\x00") +
				this->tile1() +
				STRING_WITH_NULLS("\x3A\x00") +
				this->tile3() +
				STRING_WITH_NULLS("\x3A\x00") +
				this->tile4() +
				STRING_WITH_NULLS("\x3A\x00") +
				this->tile2()
			;
		}

		virtual std::string remove()
		{
			return
				this->header +
				STRING_WITH_NULLS("\x3A\x00") +
				this->tile2()
			;
		}

		virtual std::string remove2()
		{
			return this->header;
		}

		virtual std::string insert_remove()
		{
			return
				this->header +
				STRING_WITH_NULLS("\x3A\x00") +
				this->tile3() +
				STRING_WITH_NULLS("\x3A\x00") +
				this->tile2()
			;
		}

		virtual std::string move()
		{
			return
				this->header +
				STRING_WITH_NULLS("\x3A\x00") +
				this->tile2() +
				STRING_WITH_NULLS("\x3A\x00") +
				this->tile1()
			;
		}

		virtual std::string resize_larger()
		{
			return
				this->header +
				STRING_WITH_NULLS("\x3F\x00") +
				this->tile1() + STRING_WITH_NULLS("\x00\x00\x00\x00\x00") +
				STRING_WITH_NULLS("\x3A\x00") +
				this->tile2()
			;
		}

		virtual std::string resize_smaller()
		{
			return
				this->header +
				STRING_WITH_NULLS("\x0A\x00") +
				this->tile1().substr(0, 10) +
				STRING_WITH_NULLS("\x3A\x00") +
				this->tile2()
			;
		}

		virtual std::string resize_write()
		{
			return
				this->header +
				STRING_WITH_NULLS("\x6C\x00") +
				this->content0_overwritten +
				STRING_WITH_NULLS("\x3A\x00") +
				this->tile2()
			;
		}

	protected:
		std::string header;
};

IMPLEMENT_TESTS(tls_bash_sprite);
