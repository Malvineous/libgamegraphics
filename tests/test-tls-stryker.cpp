/**
 * @file  test-tls-stryker.cpp
 * @brief Test code for Major Stryker's tilesets.
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

class test_tls_stryker: public test_tileset
{
	public:
		test_tls_stryker()
		{
			this->type = "tls-stryker";
			this->lenMaxFilename = -1;
			this->lenFilesizeFixed = 16*16*4/8;

			// We can't run isInstance() checks on new tilesets because they don't
			// match any of the known file sizes.
			this->newIsInstance = false;

			this->content[0] = this->tile1();
			this->content[1] = this->tile2();
			this->content[2] = this->tile3();
			this->content[3] = this->tile4();

			this->firstTileDims = {16, 16};
		}

		void addTests()
		{
			this->test_tileset::addTests();

			// c00: Initial state
			std::string standard;
			for (int i = 0; i < 20 * 12; i++) standard.append(this->tile1());
			this->isInstance(ArchiveType::Certainty::PossiblyYes, standard);

			// c01: File too short
			this->isInstance(ArchiveType::Certainty::DefinitelyNo, STRING_WITH_NULLS(
				"\x00\x00"
			));
		}

		virtual std::string tile1() const
		{
			return STRING_WITH_NULLS(
				"\xBF\xBF\xBF\xBF" "\xFF\xFF\xFF\xFF"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x7F\x00\x80\xFF" "\xFC\x01\x01\xFD"
			);
		}

		virtual std::string tile2() const
		{
			return STRING_WITH_NULLS(
				"\xFF\xBF\xBF\xBF" "\xFF\xFF\xFF\xFF"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x7F\x00\x80\xFF" "\xFE\x01\x01\xFD"
			);
		}

		virtual std::string tile3() const
		{
			return STRING_WITH_NULLS(
				"\xBF\xFF\xBF\xBF" "\xFF\xFF\xFF\xFF"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x7F\x00\x80\xFF" "\xFC\x03\x01\xFD"
			);
		}

		virtual std::string tile4() const
		{
			return STRING_WITH_NULLS(
				"\xFF\xFF\xBF\xBF" "\xFF\xFF\xFF\xFF"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x00\x00\x80\x80" "\x00\x01\x00\x01"
				"\x7F\x00\x80\xFF" "\xFE\x03\x01\xFD"
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

class test_tls_stryker_masked: public test_tileset
{
	public:
		test_tls_stryker_masked()
		{
			this->type = "tls-stryker-masked";
			this->lenMaxFilename = -1;
			this->lenFilesizeFixed = 16*16*5/8;
			this->hasMask = true;

			// We can't run isInstance() checks on new tilesets because they don't
			// match any of the known file sizes.
			this->newIsInstance = false;

			this->content[0] = this->tile1();
			this->content[1] = this->tile2();
			this->content[2] = this->tile3();
			this->content[3] = this->tile4();

			this->firstTileDims = {16, 16};
		}

		void addTests()
		{
			this->test_tileset::addTests();

			// c00: Initial state
			std::string standard;
			for (int i = 0; i < 20 * 12; i++) standard.append(this->tile1());
			this->isInstance(ArchiveType::Certainty::PossiblyYes, standard);

			// c01: File too short
			this->isInstance(ArchiveType::Certainty::DefinitelyNo, STRING_WITH_NULLS(
				"\x00\x00"
			));
		}

		virtual std::string tile1() const
		{
			return STRING_WITH_NULLS(
				"\x00\xBF\xBF\xBF\xBF" "\x00\xFF\xFF\xFF\xFF"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x00\x7F\x00\x80\xFF" "\x00\xFC\x01\x01\xFD"
			);
		}

		virtual std::string tile2() const
		{
			return STRING_WITH_NULLS(
				"\x00\xFF\xBF\xBF\xBF" "\x00\xFF\xFF\xFF\xFF"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x00\x7F\x00\x80\xFF" "\x00\xFE\x01\x01\xFD"
			);
		}

		virtual std::string tile3() const
		{
			return STRING_WITH_NULLS(
				"\x00\xBF\xFF\xBF\xBF" "\x00\xFF\xFF\xFF\xFF"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x00\x7F\x00\x80\xFF" "\x00\xFC\x03\x01\xFD"
			);
		}

		virtual std::string tile4() const
		{
			return STRING_WITH_NULLS(
				"\x00\xFF\xFF\xBF\xBF" "\x00\xFF\xFF\xFF\xFF"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x7F\x00\x00\x80\x80" "\xFE\x00\x01\x00\x01"
				"\x00\x7F\x00\x80\xFF" "\x00\xFE\x03\x01\xFD"
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

IMPLEMENT_TESTS(tls_stryker);
IMPLEMENT_TESTS(tls_stryker_masked);
