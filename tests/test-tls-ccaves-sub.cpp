/**
 * @file   test-tls-ccaves-sub.cpp
 * @brief  Test code for Tileset_CCavesSub class.
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

class test_tls_ccaves_sub: public test_tileset
{
	public:
		test_tls_ccaves_sub()
		{
			this->type = "tls-ccaves-sub";
			this->lenMaxFilename = -1;
			this->lenFilesizeFixed = 1 * 8 * 5;

			this->content[0] = this->tile1();
			this->content[1] = this->tile2();
			this->content[2] = this->tile3();
			this->content[3] = this->tile4();

			this->firstTileDims = {8, 8};
		}

		void addTests()
		{
			this->test_tileset::addTests();

			// c00: Initial state
			this->isInstance(ArchiveType::Certainty::PossiblyYes, this->initialstate());

			// c01: Too short
			this->isInstance(ArchiveType::Certainty::DefinitelyNo, STRING_WITH_NULLS(
				"\x00" "\x01"
			));

			// c02: Length doesn't match header
			this->isInstance(ArchiveType::Certainty::DefinitelyNo,
				STRING_WITH_NULLS(
					"\x03" "\x01" "\x08"
				) +
				this->tile1() +
				this->tile2()
			);
		}

		virtual std::string tile1() const
		{
			return STRING_WITH_NULLS(
				"\xFF\xBF\xBF\xBF\xBF"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\xFF\x7C\x01\x81\xFD"
			);
		}

		virtual std::string tile2() const
		{
			return STRING_WITH_NULLS(
				"\xFF\xFF\xBF\xBF\xBF"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\xFF\x7E\x01\x81\xFD"
			);
		}

		virtual std::string tile3() const
		{
			return STRING_WITH_NULLS(
				"\xFF\xBF\xFF\xBF\xBF"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\xFF\x7C\x03\x81\xFD"
			);
		}

		virtual std::string tile4() const
		{
			return STRING_WITH_NULLS(
				"\xFF\xFF\xFF\xBF\xBF"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\xFF\x7E\x03\x81\xFD"
			);
		}

		virtual std::string initialstate()
		{
			return
				STRING_WITH_NULLS(
					"\x02" "\x01" "\x08"
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
					"\x03" "\x01" "\x08"
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
					"\x03" "\x01" "\x08"
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
					"\x04" "\x01" "\x08"
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
					"\x01" "\x01" "\x08"
				) +
				this->tile2()
			;
		}

		virtual std::string remove2()
		{
			return STRING_WITH_NULLS(
				"\x00" "\x01" "\x08"
			);
		}

		virtual std::string insert_remove()
		{
			return
				STRING_WITH_NULLS(
					"\x02" "\x01" "\x08"
				) +
				this->tile3() +
				this->tile2()
			;
		}

		virtual std::string move()
		{
			return
				STRING_WITH_NULLS(
					"\x02" "\x01" "\x08"
				) +
				this->tile2() +
				this->tile1()
			;
		}

		virtual std::string resize_larger()
		{
			throw stream::error("Tiles in this format are a fixed size.");
		}

		virtual std::string resize_smaller()
		{
			throw stream::error("Tiles in this format are a fixed size.");
		}

		virtual std::string resize_write()
		{
			throw stream::error("Tiles in this format are a fixed size.");
		}

};

IMPLEMENT_TESTS(tls_ccaves_sub);
