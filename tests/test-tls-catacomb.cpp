/**
 * @file  test-tls-catacomb.cpp
 * @brief Test code for Catacomb I/II tilesets.
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

class test_tls_catacomb_cga: public test_tileset
{
	public:
		test_tls_catacomb_cga()
		{
			this->type = "tls-catacomb-cga";
			this->lenMaxFilename = -1;
			this->lenFilesizeFixed = 16;
			this->cga = true;

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

			// c01: Known tile size
			this->isInstance(ArchiveType::Certainty::DefinitelyYes,
				std::string(1462 * 16, '\x00')
			);

			// c02: Not a multiple of the tile size
			this->isInstance(ArchiveType::Certainty::DefinitelyNo,
				std::string(1462 * 16 - 1, '\x00')
			);
		}

		virtual std::string tile1() const
		{
			return STRING_WITH_NULLS(
				"\xCF\xFF"
				"\x40\x02"
				"\x40\x02"
				"\x40\x02"
				"\x40\x02"
				"\x40\x02"
				"\x40\x02"
				"\x6A\xA1"
			);
		}

		virtual std::string tile2() const
		{
			return STRING_WITH_NULLS(
				"\xDF\xFF"
				"\x40\x02"
				"\x40\x02"
				"\x40\x02"
				"\x40\x02"
				"\x40\x02"
				"\x40\x02"
				"\x6A\xA5"
			);
		}

		virtual std::string tile3() const
		{
			return STRING_WITH_NULLS(
				"\xEF\xFF"
				"\x40\x02"
				"\x40\x02"
				"\x40\x02"
				"\x40\x02"
				"\x40\x02"
				"\x40\x02"
				"\x6A\xA9"
			);
		}

		virtual std::string tile4() const
		{
			return STRING_WITH_NULLS(
				"\xFF\xFF"
				"\x40\x02"
				"\x40\x02"
				"\x40\x02"
				"\x40\x02"
				"\x40\x02"
				"\x40\x02"
				"\x6A\xAD"
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

class test_tls_catacomb_ega: public test_tileset
{
	public:
		test_tls_catacomb_ega()
		{
			this->type = "tls-catacomb-ega";
			this->lenMaxFilename = -1;
			this->lenFilesizeFixed = 32;

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

			// c01: Known tile size
			this->isInstance(ArchiveType::Certainty::DefinitelyYes,
				std::string(1462 * 32, '\x00')
			);

			// c02: Not a multiple of the tile size
			this->isInstance(ArchiveType::Certainty::DefinitelyNo,
				std::string(1462 * 32 - 1, '\x00')
			);
		}

		virtual std::string tile1() const
		{
			return STRING_WITH_NULLS(
				"\xBF\x00\x00\x00\x00\x00\x00\x7C"
				"\xBF\x01\x01\x01\x01\x01\x01\x01"
				"\xBF\x80\x80\x80\x80\x80\x80\x81"
				"\xBF\x81\x81\x81\x81\x81\x81\xFD"
			);
		}

		virtual std::string tile2() const
		{
			return STRING_WITH_NULLS(
				"\xFF\x00\x00\x00\x00\x00\x00\x7E"
				"\xBF\x01\x01\x01\x01\x01\x01\x01"
				"\xBF\x80\x80\x80\x80\x80\x80\x81"
				"\xBF\x81\x81\x81\x81\x81\x81\xFD"
			);
		}

		virtual std::string tile3() const
		{
			return STRING_WITH_NULLS(
				"\xBF\x00\x00\x00\x00\x00\x00\x7C"
				"\xFF\x01\x01\x01\x01\x01\x01\x03"
				"\xBF\x80\x80\x80\x80\x80\x80\x81"
				"\xBF\x81\x81\x81\x81\x81\x81\xFD"
			);
		}

		virtual std::string tile4() const
		{
			return STRING_WITH_NULLS(
				"\xFF\x00\x00\x00\x00\x00\x00\x7E"
				"\xFF\x01\x01\x01\x01\x01\x01\x03"
				"\xBF\x80\x80\x80\x80\x80\x80\x81"
				"\xBF\x81\x81\x81\x81\x81\x81\xFD"
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

IMPLEMENT_TESTS(tls_catacomb_cga);
IMPLEMENT_TESTS(tls_catacomb_ega);
