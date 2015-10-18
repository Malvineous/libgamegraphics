/**
 * @file  test-tls-cosmo.cpp
 * @brief Test code for Cosmo's Cosmic Adventures tilesets.
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

class test_tls_cosmo: public test_tileset
{
	public:
		test_tls_cosmo()
		{
			this->type = "tls-cosmo";
			this->lenMaxFilename = -1;
			this->lenFilesizeFixed = 8*8*4/8;

			// We can't run isInstance() checks on new tilesets because they don't
			// match any of the known file sizes.
			this->newIsInstance = false;

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
			std::string standard;
			for (int i = 0; i < 40 * 50; i++) standard.append(this->tile1());
			this->isInstance(ArchiveType::PossiblyYes, standard);

			// c01: File too short
			this->isInstance(ArchiveType::DefinitelyNo, STRING_WITH_NULLS(
				"\x00\x00"
			));
		}

		virtual std::string tile1() const
		{
			return STRING_WITH_NULLS(
				"\xBF\xBF\xBF\xBF"
				"\x00\x01\x80\x81"
				"\x00\x01\x80\x81"
				"\x00\x01\x80\x81"
				"\x00\x01\x80\x81"
				"\x00\x01\x80\x81"
				"\x00\x01\x80\x81"
				"\x7C\x01\x81\xFD"
			);
		}

		virtual std::string tile2() const
		{
			return STRING_WITH_NULLS(
				"\xFF\xBF\xBF\xBF"
				"\x00\x01\x80\x81"
				"\x00\x01\x80\x81"
				"\x00\x01\x80\x81"
				"\x00\x01\x80\x81"
				"\x00\x01\x80\x81"
				"\x00\x01\x80\x81"
				"\x7E\x01\x81\xFD"
			);
		}

		virtual std::string tile3() const
		{
			return STRING_WITH_NULLS(
				"\xBF\xFF\xBF\xBF"
				"\x00\x01\x80\x81"
				"\x00\x01\x80\x81"
				"\x00\x01\x80\x81"
				"\x00\x01\x80\x81"
				"\x00\x01\x80\x81"
				"\x00\x01\x80\x81"
				"\x7C\x03\x81\xFD"
			);
		}

		virtual std::string tile4() const
		{
			return STRING_WITH_NULLS(
				"\xBF\xBF\xBF\xBF"
				"\x00\x01\x80\x81"
				"\x00\x01\x80\x81"
				"\x00\x01\x80\x81"
				"\x00\x01\x80\x81"
				"\x00\x01\x80\x81"
				"\x00\x01\x80\x81"
				"\x7E\x03\x81\xFD"
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

class test_tls_cosmo_masked: public test_tileset
{
	public:
		test_tls_cosmo_masked()
		{
			this->type = "tls-cosmo-masked";
			this->lenMaxFilename = -1;
			this->lenFilesizeFixed = 8*8*5/8;
			this->hasMask = true;

			// We can't run isInstance() checks on new tilesets because they don't
			// match any of the known file sizes.
			this->newIsInstance = false;

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
			std::string standard;
			for (int i = 0; i < 40 * 25; i++) standard.append(this->tile1());
			this->isInstance(ArchiveType::PossiblyYes, standard);

			// c01: File too short
			this->isInstance(ArchiveType::DefinitelyNo, STRING_WITH_NULLS(
				"\x00\x00"
			));
		}

		virtual std::string tile1() const
		{
			return STRING_WITH_NULLS(
				"\x00\xBF\xBF\xBF\xBF"
				"\x7E\x00\x01\x80\x81"
				"\x7E\x00\x01\x80\x81"
				"\x7E\x00\x01\x80\x81"
				"\x7E\x00\x01\x80\x81"
				"\x7E\x00\x01\x80\x81"
				"\x7E\x00\x01\x80\x81"
				"\x00\x7C\x01\x81\xFD"
			);
		}

		virtual std::string tile2() const
		{
			return STRING_WITH_NULLS(
				"\x00\xFF\xBF\xBF\xBF"
				"\x7E\x00\x01\x80\x81"
				"\x7E\x00\x01\x80\x81"
				"\x7E\x00\x01\x80\x81"
				"\x7E\x00\x01\x80\x81"
				"\x7E\x00\x01\x80\x81"
				"\x7E\x00\x01\x80\x81"
				"\x00\x7E\x01\x81\xFD"
			);
		}

		virtual std::string tile3() const
		{
			return STRING_WITH_NULLS(
				"\x00\xBF\xFF\xBF\xBF"
				"\x7E\x00\x01\x80\x81"
				"\x7E\x00\x01\x80\x81"
				"\x7E\x00\x01\x80\x81"
				"\x7E\x00\x01\x80\x81"
				"\x7E\x00\x01\x80\x81"
				"\x7E\x00\x01\x80\x81"
				"\x00\x7C\x03\x81\xFD"
			);
		}

		virtual std::string tile4() const
		{
			return STRING_WITH_NULLS(
				"\x00\xBF\xBF\xBF\xBF"
				"\x7E\x00\x01\x80\x81"
				"\x7E\x00\x01\x80\x81"
				"\x7E\x00\x01\x80\x81"
				"\x7E\x00\x01\x80\x81"
				"\x7E\x00\x01\x80\x81"
				"\x7E\x00\x01\x80\x81"
				"\x00\x7E\x03\x81\xFD"
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

IMPLEMENT_TESTS(tls_cosmo);
IMPLEMENT_TESTS(tls_cosmo_masked);
