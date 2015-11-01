/**
 * @file   test-tls-ccomic2.cpp
 * @brief  Test code for Captain Comic II tilesets.
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
#include "../src/filter-ccomic2.hpp"

std::string cc2_rle(const std::string& src)
{
	auto i = std::make_unique<stream::input_string>(src);
	stream::output_string o;
	stream::input_filtered f(
		std::move(i),
		std::make_shared<filter_ccomic2_rle>(0)
	);
	stream::copy(o, f);
	return o.data;
}

class test_tls_ccomic2: public test_tileset
{
	public:
		test_tls_ccomic2()
		{
			this->type = "tls-ccomic2";
			this->lenMaxFilename = -1;
			this->lenFilesizeFixed = 128;

			this->header = STRING_WITH_NULLS("\x01\x00\xFF\xFF\x03\x00");

			this->content[0] = this->tile1();
			this->content[1] = this->tile2();
			this->content[2] = this->tile3();
			this->content[3] = this->tile4();

			this->firstTileDims = {16, 16};

			Attribute aa;
			aa.type = Attribute::Type::Integer;
			aa.integerValue = 1;
			this->attributes.push_back(aa);

			Attribute ab;
			ab.type = Attribute::Type::Integer;
			ab.integerValue = -1;
			this->attributes.push_back(ab);

			Attribute ac;
			ac.type = Attribute::Type::Integer;
			ac.integerValue = 3;
			this->attributes.push_back(ac);
		}

		void addTests()
		{
			this->test_tileset::addTests();

			// c00: Initial state
			this->isInstance(ArchiveType::Unsure, this->initialstate());

			// c01: File too large
			this->isInstance(ArchiveType::DefinitelyNo,
				this->initialstate()
				+ std::string(65535 - this->initialstate().length() + 1, '\x00')
			);

			// c02: Too many tiles
			this->isInstance(ArchiveType::DefinitelyNo,
				STRING_WITH_NULLS("\x01\x50\xFF\xFF\x03\x00") +
				cc2_rle(
					this->tile1() +
					this->tile2()
				)
			);
		}

		virtual std::string tile1() const
		{
			return STRING_WITH_NULLS(
				"\xBF\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x7F\xFC"
				"\xBF\xFF\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01" "\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01"
				"\xBF\xFF\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00" "\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x01"
				"\xBF\xFF\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01" "\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\xFF\xFD"
			);
		}

		virtual std::string tile2() const
		{
			return STRING_WITH_NULLS(
				"\xFF\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x7F\xFE"
				"\xBF\xFF\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01" "\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01"
				"\xBF\xFF\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00" "\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x01"
				"\xBF\xFF\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01" "\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\xFF\xFD"
			);
		}

		virtual std::string tile3() const
		{
			return STRING_WITH_NULLS(
				"\xBF\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x7F\xFC"
				"\xFF\xFF\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01" "\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x03"
				"\xBF\xFF\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00" "\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x01"
				"\xBF\xFF\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01" "\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\xFF\xFD"
			);
		}

		virtual std::string tile4() const
		{
			return STRING_WITH_NULLS(
				"\xFF\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x7F\xFE"
				"\xFF\xFF\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01" "\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x03"
				"\xBF\xFF\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00" "\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x01"
				"\xBF\xFF\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01" "\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\xFF\xFD"
			);
		}

		virtual std::string initialstate()
		{
			return this->header +
				cc2_rle(
					this->tile1() +
					this->tile2()
				)
			;
		}

		virtual std::string rename()
		{
			throw stream::error("This tileset does not have any tilenames.");
		}

		virtual std::string insert_end()
		{
			return this->header +
				cc2_rle(
					this->tile1() +
					this->tile2() +
					this->tile3()
				)
			;
		}

		virtual std::string insert_mid()
		{
			return this->header +
				cc2_rle(
					this->tile1() +
					this->tile3() +
					this->tile2()
				)
			;
		}

		virtual std::string insert2()
		{
			return this->header +
				cc2_rle(
					this->tile1() +
					this->tile3() +
					this->tile4() +
					this->tile2()
				)
			;
		}

		virtual std::string remove()
		{
			return this->header +
				cc2_rle(
					this->tile2()
				)
			;
		}

		virtual std::string remove2()
		{
			return this->header;
		}

		virtual std::string insert_remove()
		{
			return this->header +
				cc2_rle(
					this->tile3() +
					this->tile2()
				)
			;
		}

		virtual std::string move()
		{
			return this->header +
				cc2_rle(
					this->tile2() +
					this->tile1()
				)
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

	protected:
		std::string header;
};

IMPLEMENT_TESTS(tls_ccomic2);
