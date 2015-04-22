/**
 * @file  test-tls-zone66-map.cpp
 * @brief Test code for Tileset_Zone66Map class.
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

#include "test-image.hpp" // createPixelData
#include "test-tileset.hpp"

class test_tls_zone66_map: public test_tileset
{
	public:
		test_tls_zone66_map()
		{
			this->type = "tls-zone66-map";
			this->lenMaxFilename = -1;
			this->lenFilesizeFixed = 32 * 32;

			this->content[0] = this->tile1();
			this->content[1] = this->tile2();
			this->content[2] = this->tile3();
			this->content[3] = this->tile4();

			this->firstTileDims = {32, 32};
		}

		void addTests()
		{
			this->test_tileset::addTests();

			// c00: Initial state
			this->isInstance(ArchiveType::PossiblyYes, this->initialstate());

			// c01: Not a multiple of the tile size
			this->isInstance(ArchiveType::DefinitelyNo,
				this->initialstate().substr(0, 32 * 32 - 1)
			);
		}

		virtual std::string tile1() const
		{
			auto c = createPixelData({32, 32}, false);
			c[1] = c[32*32-2] = 0x00;
			return std::string((const char *)c.data(), c.size());
		}

		virtual std::string tile2() const
		{
			auto c = createPixelData({32, 32}, false);
			c[1] = c[32*32-2] = 0x01;
			return std::string((const char *)c.data(), c.size());
		}

		virtual std::string tile3() const
		{
			auto c = createPixelData({32, 32}, false);
			c[1] = c[32*32-2] = 0x02;
			return std::string((const char *)c.data(), c.size());
		}

		virtual std::string tile4() const
		{
			auto c = createPixelData({32, 32}, false);
			c[1] = c[32*32-2] = 0x03;
			return std::string((const char *)c.data(), c.size());
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

IMPLEMENT_TESTS(tls_zone66_map);
