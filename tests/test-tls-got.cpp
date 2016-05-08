/**
 * @file  test-tls-got.cpp
 * @brief Test code for God of Thunder tilesets.
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

class test_tls_got: public test_tileset
{
	public:
		test_tls_got()
		{
			this->type = "tls-got";
			this->lenMaxFilename = -1;

			this->content[0] = this->tile1();
			this->content[1] = this->tile2();
			this->content[2] = this->tile3();
			this->content[3] = this->tile4();

			this->content0_overwritten = this->tile1_larger_overwritten();
			this->content0_largeSize = 6 + 8 * 8 + 8;
			this->content0_smallSize = 6 + 8 * 7;

			this->firstTileDims = {8, 8};
		}

		void addTests()
		{
			this->test_tileset::addTests();

			// c00: Initial state
			this->isInstance(ArchiveType::Certainty::DefinitelyYes, this->initialstate());

			// c01: Width out of range
			this->isInstance(ArchiveType::Certainty::DefinitelyNo, STRING_WITH_NULLS(
				"\xFF\x00\x08\x00\x00\x00"
				"\x0F\x0F"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x09"

				"\x00\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x09\x09"

				"\x0F\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x09\x00"

				"\x0F\x0F"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x09\x0E"
			) +
				this->tile2()
			);

			// c01: Height out of range
			this->isInstance(ArchiveType::Certainty::DefinitelyNo, STRING_WITH_NULLS(
				"\x02\x00\xFF\x00\x00\x00"
				"\x0F\x0F"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x09"

				"\x00\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x09\x09"

				"\x0F\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x09\x00"

				"\x0F\x0F"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x09\x0E"
			) +
				this->tile2()
			);

			// c03: Data truncated
			this->isInstance(ArchiveType::Certainty::DefinitelyNo, STRING_WITH_NULLS(
				"\x02\x00\x08\x00\x00\x00"
				"\x0F\x0F"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x09"

				"\x00\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x09\x09"

				"\x0F\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x09\x00"

				"\x0F\x0F"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x09"
			));

			// c04: Empty file
			this->isInstance(ArchiveType::Certainty::PossiblyYes, {});
		}

		virtual std::string tile1() const
		{
			return STRING_WITH_NULLS(
				"\x02\x00\x08\x00\x00\x00"
				"\x0F\x0F"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x09"

				"\x00\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x09\x09"

				"\x0F\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x09\x00"

				"\x0F\x0F"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x09\x0E"
			);
		}

		virtual std::string tile2() const
		{
			return STRING_WITH_NULLS(
				"\x02\x00\x08\x00\x00\x00"
				"\x0F\x0F"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x09"

				"\x01\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x09\x09"

				"\x0F\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x09\x01"

				"\x0F\x0F"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x09\x0E"
			);
		}

		virtual std::string tile3() const
		{
			return STRING_WITH_NULLS(
				"\x02\x00\x08\x00\x00\x00"
				"\x0F\x0F"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x09"

				"\x02\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x09\x09"

				"\x0F\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x09\x02"

				"\x0F\x0F"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x09\x0E"
			);
		}

		virtual std::string tile4() const
		{
			return STRING_WITH_NULLS(
				"\x02\x00\x08\x00\x00\x00"
				"\x0F\x0F"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x09"

				"\x03\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x09\x09"

				"\x0F\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x09\x03"

				"\x0F\x0F"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x09\x0E"
			);
		}

		virtual std::string tile1_larger_overwritten() const
		{
			return STRING_WITH_NULLS(
				"\x04\x00\x10\x00\x00\x00"
				"\x0F\x0F\x0F\x0F"
				"\x0C\x00\x00\x00"
				"\x0C\x00\x00\x00"
				"\x0C\x00\x00\x00"
				"\x0C\x00\x00\x00"
				"\x0C\x00\x00\x00"
				"\x0C\x00\x00\x00"
				"\x0C\x09\x09\x09"

				"\x00\x0F\x0F\x0F"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x09\x09\x09\x09"

				"\x0F\x0F\x0F\x0F"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x00\x00\x00\x00"
				"\x09\x09\x09\x00"

				"\x0F\x0F\x0F\x0F"
				"\x00\x00\x00\x0A"
				"\x00\x00\x00\x0A"
				"\x00\x00\x00\x0A"
				"\x00\x00\x00\x0A"
				"\x00\x00\x00\x0A"
				"\x00\x00\x00\x0A"
				"\x09\x09\x09\x0E"
			);
		}

		virtual std::string initialstate()
		{
			return
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
				this->tile1() +
				this->tile2() +
				this->tile3()
			;
		}

		virtual std::string insert_mid()
		{
			return
				this->tile1() +
				this->tile3() +
				this->tile2()
			;
		}

		virtual std::string insert2()
		{
			return
				this->tile1() +
				this->tile3() +
				this->tile4() +
				this->tile2()
			;
		}

		virtual std::string remove()
		{
			return
				this->tile2()
			;
		}

		virtual std::string remove2()
		{
			return {};
		}

		virtual std::string insert_remove()
		{
			return
				this->tile3() +
				this->tile2()
			;
		}

		virtual std::string move()
		{
			return
				this->tile2() +
				this->tile1()
			;
		}

		virtual std::string resize_larger()
		{
			return STRING_WITH_NULLS(
				"\x06\x00\x03\x00\x00\x00"
				"\x0F\x0F"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x09"

				"\x00\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x09\x09"

				"\x0F\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x09\x00"

				"\x0F\x0F"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
				"\x09\x0E"

				"\x00\x00\x00\x00\x00\x00\x00\x00"
			) +
				this->tile2()
			;
		}

		virtual std::string resize_smaller()
		{
			return STRING_WITH_NULLS(
				"\x07\x00\x02\x00\x00\x00"
				"\x0F\x0F"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x00"
				"\x0C\x09"

				"\x00\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x09\x09"

				"\x0F\x0F"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x00\x00"
				"\x09\x00"

				"\x0F\x0F"
				"\x00\x0A"
				"\x00\x0A"
				"\x00\x0A"
			) +
				this->tile2()
			;
		}

		virtual std::string resize_write()
		{
			return
				this->tile1_larger_overwritten() +
				this->tile2()
			;
		}
};

IMPLEMENT_TESTS(tls_got);
