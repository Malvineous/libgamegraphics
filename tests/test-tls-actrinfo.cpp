/**
 * @file   test-tls-actrinfo.cpp
 * @brief  Test code for Cosmo actor tiles.
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

class test_suppextra_tls_actrinfo: public test_tileset
{
	public:
		test_suppextra_tls_actrinfo()
		{
			this->type = "tls-actrinfo.extra1";
			this->lenMaxFilename = -1;
		}

		std::string tile1() const
		{
			return STRING_WITH_NULLS(
				"\x00\xBF\xBF\xBF\xBF"
				"\x7F\x00\x00\x80\x80"
				"\x7F\x00\x00\x80\x80"
				"\x7F\x00\x00\x80\x80"
				"\x7F\x00\x00\x80\x80"
				"\x7F\x00\x00\x80\x80"
				"\x7F\x00\x00\x80\x80"
				"\x00\x7F\x00\x80\xFF"

				"\x00\xFF\xFF\xFF\xFF"
				"\xFE\x00\x01\x00\x01"
				"\xFE\x00\x01\x00\x01"
				"\xFE\x00\x01\x00\x01"
				"\xFE\x00\x01\x00\x01"
				"\xFE\x00\x01\x00\x01"
				"\xFE\x00\x01\x00\x01"
				"\x00\xFC\x01\x01\xFD"
			);
		}

		std::string tile2() const
		{
			return STRING_WITH_NULLS(
				"\x00\xFF\xBF\xBF\xBF"
				"\x7F\x00\x00\x80\x80"
				"\x7F\x00\x00\x80\x80"
				"\x7F\x00\x00\x80\x80"
				"\x7F\x00\x00\x80\x80"
				"\x7F\x00\x00\x80\x80"
				"\x7F\x00\x00\x80\x80"
				"\x00\x7F\x00\x80\xFF"

				"\x00\xFF\xFF\xFF\xFF"
				"\xFE\x00\x01\x00\x01"
				"\xFE\x00\x01\x00\x01"
				"\xFE\x00\x01\x00\x01"
				"\xFE\x00\x01\x00\x01"
				"\xFE\x00\x01\x00\x01"
				"\xFE\x00\x01\x00\x01"
				"\x00\xFE\x01\x01\xFD"
			);
		}

		std::string tile3() const
		{
			return STRING_WITH_NULLS(
				"\x00\xBF\xFF\xBF\xBF"
				"\x7F\x00\x00\x80\x80"
				"\x7F\x00\x00\x80\x80"
				"\x7F\x00\x00\x80\x80"
				"\x7F\x00\x00\x80\x80"
				"\x7F\x00\x00\x80\x80"
				"\x7F\x00\x00\x80\x80"
				"\x00\x7F\x00\x80\xFF"

				"\x00\xFF\xFF\xFF\xFF"
				"\xFE\x00\x01\x00\x01"
				"\xFE\x00\x01\x00\x01"
				"\xFE\x00\x01\x00\x01"
				"\xFE\x00\x01\x00\x01"
				"\xFE\x00\x01\x00\x01"
				"\xFE\x00\x01\x00\x01"
				"\x00\xFC\x03\x01\xFD"
			);
		}

		std::string tile4() const
		{
			return STRING_WITH_NULLS(
				"\x00\xFF\xFF\xBF\xBF"
				"\x7F\x00\x00\x80\x80"
				"\x7F\x00\x00\x80\x80"
				"\x7F\x00\x00\x80\x80"
				"\x7F\x00\x00\x80\x80"
				"\x7F\x00\x00\x80\x80"
				"\x7F\x00\x00\x80\x80"
				"\x00\x7F\x00\x80\xFF"

				"\x00\xFF\xFF\xFF\xFF"
				"\xFE\x00\x01\x00\x01"
				"\xFE\x00\x01\x00\x01"
				"\xFE\x00\x01\x00\x01"
				"\xFE\x00\x01\x00\x01"
				"\xFE\x00\x01\x00\x01"
				"\xFE\x00\x01\x00\x01"
				"\x00\xFE\x03\x01\xFD"
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
			return
				this->tile1() + STRING_WITH_NULLS("\x00\x00\x00\x00\x00") +
				this->tile2()
			;
		}

		virtual std::string resize_smaller()
		{
			return
				this->tile1().substr(0, 10) +
				this->tile2()
			;
		}

		virtual std::string resize_write()
		{
			return
				this->content0_overwritten +
				this->tile2()
			;
		}
};

class test_tls_actrinfo: public test_tileset
{
	public:
		test_tls_actrinfo()
		{
			this->type = "tls-actrinfo";
			this->lenMaxFilename = -1;
			this->outputWidth = 8;
			this->foldersOnly = true;

			auto suppextra = std::make_unique<test_suppextra_tls_actrinfo>();
			this->content[0] = suppextra->tile1();
			this->content[1] = suppextra->tile2();
			this->content[2] = suppextra->tile3();
			this->content[3] = suppextra->tile4();

			// Since we need image sizes to handle resizing, skip the resize tests
			this->lenFilesizeFixed = suppextra->tile1().length();

			this->suppResult[SuppItem::Extra1] = std::move(suppextra);

			this->firstTileDims = {16, 8};
		}

		std::string tileset1()
		{
			return STRING_WITH_NULLS(
				"\x01\x00\x02\x00" "\x00\x00\x00\x00" // Frame 1, 2x1
			);
		}

		std::string tileset2()
		{
			return STRING_WITH_NULLS(
				"\x01\x00\x02\x00" "\x50\x00\x00\x00" // Frame 1, 2x1
			);
		}

		std::string tileset3()
		{
			return STRING_WITH_NULLS(
				"\x01\x00\x02\x00" "\xA0\x00\x00\x00" // Frame 1, 2x1
			);
		}

		std::string tileset4()
		{
			return STRING_WITH_NULLS(
				"\x01\x00\x02\x00" "\xF0\x00\x00\x00" // Frame 1, 2x1
			);
		}

		void addTests()
		{
			this->test_tileset::addTests();

			// c00: Initial state
			this->isInstance(ArchiveType::DefinitelyYes, this->initialstate());

			// c01: Could be a blank file
			this->isInstance(ArchiveType::Unsure, {});

			// c02: First offset truncated
			this->isInstance(ArchiveType::DefinitelyNo, STRING_WITH_NULLS(
				"\x00"
			));

			// c03: No actor data
			this->isInstance(ArchiveType::DefinitelyNo,
				STRING_WITH_NULLS(
					"\x02\xF0"
					"\x06\x00"
				)
				+ this->tileset1()
				+ this->tileset2()
			);

			// c04: First FAT offset points to FAT itself
			this->isInstance(ArchiveType::DefinitelyNo,
				STRING_WITH_NULLS(
					"\x00\x00"
					"\x06\x00"
				)
				+ this->tileset1()
				+ this->tileset2()
			);

			// c05: Actor frame too big (>320x200)
			this->isInstance(ArchiveType::DefinitelyNo,
				STRING_WITH_NULLS(
					"\x02\x00"
					"\x06\x00"
				)
				+ STRING_WITH_NULLS(
					"\x01\x00\x29\x00" "\x00\x00\x00\x00" // Frame 1, 328x1
				)
				+ this->tileset2()
			);

			// c06: Offset too big (>4MB)
			this->isInstance(ArchiveType::DefinitelyNo,
				STRING_WITH_NULLS(
					"\x02\x00"
					"\x06\x00"
				)
				+ STRING_WITH_NULLS(
					"\x01\x00\x02\x00" "\x01\x00\x40\x00" // Frame 1, 328x1
				)
				+ this->tileset2()
			);

			// c07: Offsets decrementing
			this->isInstance(ArchiveType::DefinitelyNo,
				STRING_WITH_NULLS(
					"\x02\x00"
					"\x01\x00"
				)
				+ this->tileset1()
				+ this->tileset2()
			);

			// c08: Truncated actor data
			this->isInstance(ArchiveType::DefinitelyNo,
				STRING_WITH_NULLS(
					"\x02\x00"
					"\x06\xF0"
				)
				+ this->tileset1()
				+ this->tileset2()
			);
		}

		virtual std::string initialstate()
		{
			return
				STRING_WITH_NULLS(
					"\x02\x00" // Actor 1 offset
					"\x06\x00" // Actor 2 offset
				)
				// Actor 1
				+ this->tileset1()
				// Actor 2
				+ this->tileset2()
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
					"\x03\x00" // Actor 1 offset
					"\x07\x00" // Actor 2 offset
					"\x0B\x00" // Actor 3 offset
				)
				// Actor 1
				+ this->tileset1()
				// Actor 2
				+ this->tileset2()
				// Actor 3
				+ this->tileset3()
			;
		}

		virtual std::string insert_mid()
		{
			return
				STRING_WITH_NULLS(
					"\x03\x00" // Actor 1 offset
					"\x07\x00" // Actor 2 offset
					"\x0B\x00" // Actor 3 offset
				)
				// Actor 1
				+ this->tileset1()
				// Actor 2
				+ this->tileset2()
				// Actor 3
				+ this->tileset3()
			;
		}

		virtual std::string insert2()
		{
			return
				STRING_WITH_NULLS(
					"\x04\x00" // Actor 1 offset
					"\x08\x00" // Actor 2 offset
					"\x0C\x00" // Actor 3 offset
					"\x10\x00" // Actor 4 offset
				)
				// Actor 1
				+ this->tileset1()
				// Actor 2
				+ this->tileset2()
				// Actor 3
				+ this->tileset3()
				// Actor 4
				+ this->tileset4()
			;
		}

		virtual std::string remove()
		{
			return
				STRING_WITH_NULLS(
					"\x01\x00" // Actor 1 offset
				)
				// Actor 1
				+ this->tileset1()
			;
		}

		virtual std::string remove2()
		{
			return {};
		}

		virtual std::string insert_remove()
		{
			return
				STRING_WITH_NULLS(
					"\x02\x00" // Actor 1 offset
					"\x06\x00" // Actor 2 offset
				)
				// Actor 1
				+ this->tileset1()
				// Actor 2
				+ this->tileset2()
			;
		}

		virtual std::string move()
		{
			return
				STRING_WITH_NULLS(
					"\x02\x00" // Actor 1 offset
					"\x06\x00" // Actor 2 offset
				)
				// Actor 1
				+ this->tileset1()
				// Actor 2
				+ this->tileset2()
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

/// Same tests again, but as tiles entirely within the first actor
class test_tls_actrinfo_sub: public test_tileset
{
	public:
		test_tls_actrinfo_sub()
		{
			this->type = "tls-actrinfo";
			this->lenMaxFilename = -1;
			this->outputWidth = 8;

			auto suppextra = std::make_unique<test_suppextra_tls_actrinfo>();
			this->content[0] = suppextra->tile1();
			this->content[1] = suppextra->tile2();
			this->content[2] = suppextra->tile3();
			this->content[3] = suppextra->tile4();

			// Since we need image sizes to handle resizing, skip the resize tests
			this->lenFilesizeFixed = suppextra->tile1().length();

			this->suppResult[SuppItem::Extra1] = std::move(suppextra);

			this->firstTileDims = {16, 8};
		}

		void prepareTest(bool empty)
		{
			this->test_tileset::prepareTest(empty);
			if (empty) {
				// Insert the first actor/tileset
				auto epActor = this->pArchive->insert(nullptr, this->filename[0],
					0, {}, Archive::File::Attribute::Folder);
				this->pArchive = this->pArchive->openFolder(epActor);
			} else {
				// Open the first actor/tileset so the rest of the tests will be done there
				auto actors = this->pArchive->files();
				this->pArchive = this->pArchive->openFolder(actors[0]);
			}
			return;
		}

		std::string tileset1()
		{
			return STRING_WITH_NULLS(
				"\x01\x00\x02\x00" "\x00\x00\x00\x00" // Frame 1, 2x1
			);
		}

		std::string tileset2()
		{
			return STRING_WITH_NULLS(
				"\x01\x00\x02\x00" "\x50\x00\x00\x00" // Frame 1, 2x1
			);
		}

		std::string tileset3()
		{
			return STRING_WITH_NULLS(
				"\x01\x00\x02\x00" "\xA0\x00\x00\x00" // Frame 1, 2x1
			);
		}

		std::string tileset4()
		{
			return STRING_WITH_NULLS(
				"\x01\x00\x02\x00" "\xF0\x00\x00\x00" // Frame 1, 2x1
			);
		}

		virtual std::string initialstate()
		{
			return
				STRING_WITH_NULLS(
					"\x01\x00" // Actor 1 offset
				)
				+ this->tileset1()
				+ this->tileset2()
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
					"\x01\x00" // Actor 1 offset
				)
				+ this->tileset1()
				+ this->tileset2()
				+ this->tileset3()
			;
		}

		virtual std::string insert_mid()
		{
			return
				STRING_WITH_NULLS(
					"\x01\x00" // Actor 1 offset
				)
				+ this->tileset1()
				+ this->tileset2()
				+ this->tileset3()
			;
		}

		virtual std::string insert2()
		{
			return
				STRING_WITH_NULLS(
					"\x01\x00" // Actor 1 offset
				)
				+ this->tileset1()
				+ this->tileset2()
				+ this->tileset3()
				+ this->tileset4()
			;
		}

		virtual std::string remove()
		{
			return
				STRING_WITH_NULLS(
					"\x01\x00" // Actor 1 offset
				)
				+ this->tileset1()
			;
		}

		virtual std::string remove2()
		{
			return
				STRING_WITH_NULLS(
					"\x01\x00" // Actor 1 offset (empty actor now)
				)
			;
		}

		virtual std::string insert_remove()
		{
			return
				STRING_WITH_NULLS(
					"\x01\x00" // Actor 1 offset
				)
				+ this->tileset1()
				+ this->tileset2()
			;
		}

		virtual std::string move()
		{
			return
				STRING_WITH_NULLS(
					"\x01\x00" // Actor 1 offset
				)
				+ this->tileset1()
				+ this->tileset2()
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

IMPLEMENT_TESTS(tls_actrinfo);
IMPLEMENT_TESTS(tls_actrinfo_sub);
