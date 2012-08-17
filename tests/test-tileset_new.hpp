/**
 * @tile   test-tileset_new.hpp
 * @brief  Generic test code for creating new tilesets from scratch.
 *
 * Copyright (C) 2010-2011 Adam Nielsen <malvineous@shikadi.net>
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

struct EMPTY_FIXTURE_NAME: public FIXTURE_NAME {
	EMPTY_FIXTURE_NAME()
		:	FIXTURE_NAME(0)
	{
		#ifdef HAS_FAT
		{
			stream::string_sptr supp(new stream::string());
			supp << makeString(TEST_RESULT(FAT_initialstate));
			this->suppData[FAT] = supp;
		}
		#endif

		ManagerPtr pManager(gamegraphics::getManager());
		TilesetTypePtr pTestType(pManager->getTilesetTypeByCode(TILESET_TYPE));

		BOOST_TEST_CHECKPOINT("About to call newTileset()");

		// This should really use BOOST_REQUIRE_NO_THROW but the message is more
		// informative without it.
		//BOOST_REQUIRE_NO_THROW(
		this->pTileset = pTestType->create(this->base, this->suppData);
		//);
		BOOST_REQUIRE_MESSAGE(this->pTileset, "Could not create new tileset");

		BOOST_TEST_CHECKPOINT("New tileset resized successfully");

		if (this->pTileset->getCaps() & Tileset::ChangeDimensions) {
			this->pTileset->setTilesetDimensions(DATA_TILE_WIDTH, DATA_TILE_HEIGHT);
			BOOST_TEST_CHECKPOINT("New tileset created successfully");
		}
	}
};

BOOST_FIXTURE_TEST_SUITE(EMPTY_SUITE_NAME, EMPTY_FIXTURE_NAME)

// Make sure a newly created tileset is confirmed as a valid instance of that
// tileset format.
BOOST_AUTO_TEST_CASE(TEST_NAME(new_isinstance))
{
	BOOST_TEST_MESSAGE("Checking new tileset is valid instance of itself");

	pTileset->flush();

	boost::shared_ptr<Manager> pManager(getManager());
	TilesetTypePtr pTestType(pManager->getTilesetTypeByCode(TILESET_TYPE));

	BOOST_REQUIRE_MESSAGE(pTestType->isInstance(this->base),
		"Newly created tileset was not recognised as a valid instance");

	BOOST_TEST_CHECKPOINT("New tileset reported valid, trying to open");

	// This should really use BOOST_REQUIRE_NO_THROW but the message is more
	// informative without it.
	//BOOST_REQUIRE_NO_THROW(
		TilesetPtr pTileset(pTestType->open(this->base, suppData));
	//);

	// Make sure there are now no tiles in the tileset
	const Tileset::VC_ENTRYPTR& tiles = pTileset->getItems();
	BOOST_REQUIRE_EQUAL(tiles.size(), 0);
}

BOOST_AUTO_TEST_CASE(TEST_NAME(new_to_initialstate))
{
	BOOST_TEST_MESSAGE("Creating tileset from scratch");

	const Tileset::VC_ENTRYPTR& tiles = pTileset->getItems();
	BOOST_REQUIRE_EQUAL(tiles.size(), 0);

	// Add the tiles to the new tileset
	Tileset::EntryPtr ep1 =
		pTileset->insert(Tileset::EntryPtr(), Tileset::Default);
	BOOST_REQUIRE_MESSAGE(ep1->isValid(),
		"Couldn't insert new tile in empty tileset");
	setTileData(ep1, 1, 0);

	Tileset::EntryPtr ep2 =
		pTileset->insert(Tileset::EntryPtr(), Tileset::Default);

	BOOST_REQUIRE_MESSAGE(ep2->isValid(),
		"Couldn't insert second new tile in empty tileset");
	setTileData(ep2, 2, 0);

#ifdef testdata_get_metadata_description
	// If this format has metadata, set it to the same value used when comparing
	// against the initialstate, so that this new tileset will hopefully match
	// the initialstate itself.
	pTileset->setMetadata(camoto::Metadata::Description, TEST_RESULT(get_metadata_description));
#endif

	BOOST_CHECK_MESSAGE(
		is_equal(makeString(TEST_RESULT(initialstate))),
		"Error inserting tiles in new/empty tileset"
	);

	// Make sure there are now the correct number of tiles in the tileset
	BOOST_REQUIRE_EQUAL(tiles.size(), 2);

#ifdef HAS_FAT
	BOOST_CHECK_MESSAGE(
		is_supp_equal(EST_FAT, makeString(TEST_RESULT(FAT_initialstate))),
		"Error inserting tiles in new/empty tileset"
	);
#endif
}

// The function shifting tiles can get confused if a zero-length tile is
// inserted, incorrectly moving it because of the zero size.
BOOST_AUTO_TEST_CASE(TEST_NAME(manipulate_zero_length_tiles))
{
	BOOST_TEST_MESSAGE("Inserting empty tiles into tileset, then resizing them");

#ifdef testdata_get_metadata_description
	// If this format has metadata, set it to the same value used when comparing
	// against the initialstate, so that this new tileset will hopefully match
	// the initialstate itself.
	pTileset->setMetadata(camoto::Metadata::Description, TEST_RESULT(get_metadata_description));
#endif

	// Insert the tile
	Tileset::EntryPtr ep3 =
		pTileset->insert(Tileset::EntryPtr(), Tileset::Default);
	BOOST_REQUIRE_MESSAGE(ep3->isValid(),
		"Couldn't insert new tile in empty tileset");
	ImagePtr img3(pTileset->openImage(ep3));
	// Get offsets of each tile for later testing
	FATTileset::FATEntryPtr fat3 =
		boost::dynamic_pointer_cast<FATTileset::FATEntry>(ep3);
	int off3;
	if (fat3) off3 = fat3->offset;

	Tileset::EntryPtr ep1 =
		pTileset->insert(ep3, Tileset::Default);
	BOOST_REQUIRE_MESSAGE(ep1->isValid(),
		"Couldn't insert new tile in empty tileset");
	ImagePtr img1(pTileset->openImage(ep1));

	Tileset::EntryPtr ep2 =
		pTileset->insert(ep3, Tileset::Default);
	BOOST_REQUIRE_MESSAGE(ep2->isValid(),
		"Couldn't insert new tile in empty tileset");
	ImagePtr img2(pTileset->openImage(ep2));

	// Get offsets of first tile for later testing
	FATTileset::FATEntryPtr fat1 =
		boost::dynamic_pointer_cast<FATTileset::FATEntry>(ep1);
	int off1;
	if (fat1) off1 = fat1->offset;

	// This will resize the second tile.  Since all three tiles are zero-length,
	// they currently all share the same offset.  This should result in tile1
	// keeping its original offset (same as tile2) and tile3's offset being
	// increased.
	setTileData(ep2, 2, 0);

	// Make sure the first tile hasn't moved.  We can't test the other tiles as
	// sometimes they will move, sometimes they won't (e.g. in the case of
	// tilesets with fixed image sizes, where the insert puts data at the final
	// offset.)
	if (fat1) BOOST_REQUIRE_EQUAL(fat1->offset, off1);

	setTileData(ep1, 1, 0);

	// Make sure the first tile still hasn't moved
	if (fat1) BOOST_REQUIRE_EQUAL(fat1->offset, off1);

	setTileData(ep3, 3, 0);

	BOOST_CHECK_MESSAGE(
		is_equal(makeString(TEST_RESULT(insert_end))),
		"Error manipulating zero-length tiles"
	);

#ifdef HAS_FAT
	BOOST_CHECK_MESSAGE(
		is_supp_equal(EST_FAT, makeString(TEST_RESULT(FAT_insert_end))),
		"Error manipulating zero-length tiles"
	);
#endif
}

BOOST_AUTO_TEST_SUITE_END()
