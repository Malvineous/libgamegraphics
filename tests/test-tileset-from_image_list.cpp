/**
 * @file   test-tileset-from_image_list.cpp
 * @brief  Test code for creating images from assembling tiles.
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
#include <camoto/gamegraphics/tileset-from_image_list.hpp>
#include "../src/img-vga-raw.hpp" // convenient format to load tiles from

class test_tileset_from_image_list: public test_tileset
{
	public:
		test_tileset_from_image_list()
		{
			this->type = "tileset-from_image_list";
			this->lenMaxFilename = -1;
			this->create = false;
			this->staticFiles = true;
			this->virtualFiles = true;
//			this->lenFilesizeFixed = 256;

			this->content[0] = this->tile1();
			this->content[1] = this->tile2();
			this->content[2] = this->tile3();
			this->content[3] = this->tile4();

			this->firstTileDims = {4, 4};
		}

		void addTests()
		{
			this->test_tileset::addTests();
		}

		virtual std::string tile1() const
		{
			return STRING_WITH_NULLS(
				"\x0F\x00\x0F\x0F"
				"\x0C\x00\x00\x0A"
				"\x0C\x00\x00\x0A"
				"\x0C\x09\x00\x0E"
			);
		}

		virtual std::string tile2() const
		{
			return STRING_WITH_NULLS(
				"\x0F\x01\x0F\x0F"
				"\x0C\x00\x00\x0A"
				"\x0C\x00\x00\x0A"
				"\x0C\x09\x01\x0E"
			);
		}

		virtual std::string tile3() const
		{
			return STRING_WITH_NULLS(
				"\x0F\x02\x0F\x0F"
				"\x0C\x00\x00\x0A"
				"\x0C\x00\x00\x0A"
				"\x0C\x09\x02\x0E"
			);
		}

		virtual std::string tile4() const
		{
			return STRING_WITH_NULLS(
				"\x0F\x03\x0F\x0F"
				"\x0C\x00\x00\x0A"
				"\x0C\x00\x00\x0A"
				"\x0C\x09\x03\x0E"
			);
		}

		virtual std::string initialstate()
		{
			return STRING_WITH_NULLS(
				"\x0F\x00\x0F\x0F" "\x0F\x01\x0F\x0F" "\x0F\x02\x0F\x0F" "\x0F\x03\x0F\x0F"
				"\x0C\x00\x00\x0A" "\x0C\x00\x00\x0A" "\x0C\x00\x00\x0A" "\x0C\x00\x00\x0A"
				"\x0C\x00\x00\x0A" "\x0C\x00\x00\x0A" "\x0C\x00\x00\x0A" "\x0C\x00\x00\x0A"
				"\x0C\x09\x00\x0E" "\x0C\x09\x01\x0E" "\x0C\x09\x02\x0E" "\x0C\x09\x03\x0E"
			);
		}

		virtual std::string rename()
		{
			throw stream::error("This tileset does not have any tilenames.");
		}

		virtual std::string insert_end()
		{
			throw stream::error("Tiles in this format cannot be rearranged.");
		}

		virtual std::string insert_mid()
		{
			throw stream::error("Tiles in this format cannot be rearranged.");
		}

		virtual std::string insert2()
		{
			throw stream::error("Tiles in this format cannot be rearranged.");
		}

		virtual std::string remove()
		{
			throw stream::error("Tiles in this format cannot be rearranged.");
		}

		virtual std::string remove2()
		{
			throw stream::error("Tiles in this format cannot be rearranged.");
		}

		virtual std::string insert_remove()
		{
			throw stream::error("Tiles in this format cannot be rearranged.");
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

		virtual std::shared_ptr<Tileset> openTileset(
			std::unique_ptr<stream::inout> content,	bool create)
		{
			if (create) {
				throw stream::error("Cannot create empty tileset from image.");
			} else {
				*content << this->initialstate();
			}

			// Make sure the stream contains enough data
			BOOST_REQUIRE_EQUAL(content->size(), 16 * 4);

			auto img = std::make_shared<Image_VGARaw>(
				std::move(content),
				Point{4*4, 4},
				nullptr
			);

			// This does nothing, we just want to check it can be converted ok before
			// we continue
			auto imgTest = img->convert();
			auto mskTest = img->convert_mask();

			return make_Tileset_FromImageList(
				{
					{
						img,
						Tileset_FromImageList::Item::AttachmentType::Append,
						Tileset_FromImageList::Item::SplitType::UniformTiles,
						{4, 4}, // tile size
						{0, 0, 4*4, 4}, // image size
						{}
					}
				},
				1
			);
		}
};

IMPLEMENT_TESTS(tileset_from_image_list);
