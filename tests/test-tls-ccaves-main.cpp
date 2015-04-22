/**
 * @file   test-tls-ccaves-main.cpp
 * @brief  Test code for Tileset_CCavesMain class.
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

class test_tls_ccaves_main: public test_tileset
{
	public:
		test_tls_ccaves_main()
		{
			this->type = "tls-ccaves-main";
			this->lenMaxFilename = -1;

			this->content[0] = this->tileset1();
			this->content[1] = this->tileset2();
			this->content[2] = this->tileset3();
			this->content[3] = this->tileset4();

			this->content0_overwritten = this->tileset1_larger_replaced();
			this->content0_largeSize = 3 + 8 * 5 * 2;
			this->content0_smallSize = 3;

			this->firstTileDims = {8, 8};
		}

		void addTests()
		{
			this->test_tileset::addTests();

			// c00: Initial state
			this->isInstance(ArchiveType::PossiblyYes, this->initialstate());

			// c01: Too short
			this->isInstance(ArchiveType::DefinitelyNo, STRING_WITH_NULLS(
				"\x00" "\x01"
			));

			// c02: Length doesn't match header
			this->isInstance(ArchiveType::DefinitelyNo,
				STRING_WITH_NULLS(
					"\x03" "\x01" "\x08"
				) +
				this->tileset1() +
				this->tileset2()
			);
		}

		virtual std::string tileset1() const
		{
			return STRING_WITH_NULLS(
				"\x01\x01\x08"
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

		virtual std::string tileset2() const
		{
			return STRING_WITH_NULLS(
				"\x01\x01\x08"
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

		virtual std::string tileset3() const
		{
			return STRING_WITH_NULLS(
				"\x01\x01\x08"
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

		virtual std::string tileset4() const
		{
			return STRING_WITH_NULLS(
				"\x01\x01\x08"
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

		virtual std::string tileset1_larger_empty() const
		{
			return STRING_WITH_NULLS(
				"\x02\x01\x08"
				"\xFF\xBF\xBF\xBF\xBF"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\xFF\x7C\x01\x81\xFD"

				"\x00\x00\x00\x00\x00"
				"\x00\x00\x00\x00\x00"
				"\x00\x00\x00\x00\x00"
				"\x00\x00\x00\x00\x00"
				"\x00\x00\x00\x00\x00"
				"\x00\x00\x00\x00\x00"
				"\x00\x00\x00\x00\x00"
				"\x00\x00\x00\x00\x00"
			);
		}

		virtual std::string tileset1_smaller() const
		{
			return STRING_WITH_NULLS(
				"\x00\x01\x08"
			);
		}

		virtual std::string tileset1_larger_replaced() const
		{
			return STRING_WITH_NULLS(
				"\x02\x01\x08"
				"\xFF\xBF\xBF\xBF\xBF"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\x81\x00\x01\x80\x81"
				"\xFF\x7C\x01\x81\xFD"

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

		virtual std::string initialstate()
		{
			return
				this->tileset1() +
				this->tileset2()
			;
		}

		virtual std::string rename()
		{
			throw stream::error("This tileset does not have any tilenames.");
		}

		virtual std::string insert_end()
		{
			return
				this->tileset1() +
				this->tileset2() +
				this->tileset3()
			;
		}

		virtual std::string insert_mid()
		{
			return
				this->tileset1() +
				this->tileset3() +
				this->tileset2()
			;
		}

		virtual std::string insert2()
		{
			return
				this->tileset1() +
				this->tileset3() +
				this->tileset4() +
				this->tileset2()
			;
		}

		virtual std::string remove()
		{
			return
				this->tileset2()
			;
		}

		virtual std::string remove2()
		{
			return {};
		}

		virtual std::string insert_remove()
		{
			return
				this->tileset3() +
				this->tileset2()
			;
		}

		virtual std::string move()
		{
			return
				this->tileset2() +
				this->tileset1()
			;
		}

		virtual std::string resize_larger()
		{
			return
				this->tileset1_larger_empty() +
				this->tileset2()
			;
		}

		virtual std::string resize_smaller()
		{
			return
				this->tileset1_smaller() +
				this->tileset2()
			;
		}

		virtual std::string resize_write()
		{
			return
				this->tileset1_larger_replaced() +
				this->tileset2()
			;
		}

};

IMPLEMENT_TESTS(tls_ccaves_main);
