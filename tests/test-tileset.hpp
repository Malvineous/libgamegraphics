/**
 * @file  test-tileset.hpp
 * @brief Generic test code for Tileset class descendents.
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

#include <boost/test/unit_test.hpp>
#include <camoto/stream_string.hpp>
#include <camoto/stream_seg.hpp>
#include <camoto/gamegraphics.hpp>

#include "tests.hpp"

// Define if there's no way to guarantee a given data set is in this format.
// It makes the c00 isinstance check match against PossiblyYes as opposed
// to DefinitelyYes.
//#define TILESET_DETECTION_UNCERTAIN

using namespace camoto;
using namespace camoto::gamegraphics;

// Defines to allow code reuse
#define COMBINE_CLASSNAME_EXP(c, n)  c ## _ ## n
#define COMBINE_CLASSNAME(c, n)  COMBINE_CLASSNAME_EXP(c, n)

#define TEST_VAR(n)        COMBINE_CLASSNAME(TILESET_CLASS, n)
#define TEST_NAME(n)       TEST_VAR(n)
#define TEST_RESULT(n)     test_tileset_ ## n

#define FIXTURE_NAME       TEST_VAR(sample)
#define EMPTY_FIXTURE_NAME TEST_VAR(sample_empty)
#define SUITE_NAME         TEST_VAR(suite)
#define EMPTY_SUITE_NAME   TEST_VAR(suite_empty)

#include "../src/tileset-fat.hpp"

struct FIXTURE_NAME: public default_sample {

	stream::string_sptr base;
	TilesetPtr pTileset;
	SuppData suppData;

	FIXTURE_NAME() :
		base(new stream::string())
	{
		this->base << makeString(test_tileset_initialstate);
		#ifdef HAS_FAT
		{
			stream::string_sptr supp(new stream::string());
			supp << makeString(TEST_RESULT(FAT_initialstate));
			this->suppData[FAT] = supp;
		}
		#endif

		BOOST_REQUIRE_NO_THROW(
			ManagerPtr pManager(gamegraphics::getManager());
			TilesetTypePtr pTestType(pManager->getTilesetTypeByCode(TILESET_TYPE));
			BOOST_REQUIRE_MESSAGE(pTestType, "Could not find tileset code " TILESET_TYPE);

			this->pTileset = pTestType->open(this->base, this->suppData);
			BOOST_REQUIRE_MESSAGE(this->pTileset, "Could not open initialstate as tileset");
		);
	}

	FIXTURE_NAME(int i) :
		base(new stream::string())
	{
	}

	boost::test_tools::predicate_result is_equal(const std::string& strExpected)
	{
		// Flush out any changes before we perform the check
		BOOST_CHECK_NO_THROW(
			this->pTileset->flush()
		);

		std::string baseStr = this->base->str();

		int len = strExpected.length();
		if (baseStr.length() > len) len = baseStr.length();

		boost::test_tools::predicate_result ret =
			this->default_sample::is_equal((uint8_t *)strExpected.c_str(),
				(uint8_t *)baseStr.c_str(), len, 8);
		BOOST_CHECK_EQUAL(strExpected.length(), baseStr.length());
		return ret;
	}

	boost::test_tools::predicate_result is_supp_equal(camoto::SuppItem::Type type, const std::string& strExpected)
	{
		// Flush out any changes to the main archive before we perform the check,
		// in case this function was called first.
		BOOST_CHECK_NO_THROW(
			this->pTileset->flush()
		);

		stream::string_sptr sstr = boost::dynamic_pointer_cast<stream::string>(this->suppData[type]);
		std::string baseStr = sstr->str();
		boost::test_tools::predicate_result ret =
			this->default_sample::is_equal((uint8_t *)strExpected.c_str(),
				(uint8_t *)baseStr.c_str(), baseStr.length(), 8);
		BOOST_CHECK_EQUAL(strExpected.length(), baseStr.length());
		return ret;
	}

// Make an image of the standard size with each pixel set to n, and the very
// last pixel set to 0x0e.
#define MAKE_MASK(var, n) \
	StdImageDataPtr var(new uint8_t[DATA_TILE_WIDTH * DATA_TILE_HEIGHT]); \
	memset(var.get(), n, \
		DATA_TILE_WIDTH * DATA_TILE_HEIGHT);

#define MAKE_IMAGE(var, n) \
	MAKE_MASK(var, n) \
	/* Last pixel is different */ \
	var[DATA_TILE_WIDTH * DATA_TILE_HEIGHT - 1] = 0x0e;

	void setTileData(Tileset::EntryPtr ep, int imgVal, int maskVal)
	{
		// Open new tile and populate with image data
		ImagePtr img(pTileset->openImage(ep));
		if (img->getCaps() & Image::CanSetDimensions) {
			img->setDimensions(DATA_TILE_WIDTH, DATA_TILE_HEIGHT);
		}

		MAKE_IMAGE(newImg, imgVal);
		MAKE_MASK(newMask, maskVal);
		img->fromStandard(newImg, newMask);

		return;
	}

};

BOOST_FIXTURE_TEST_SUITE(SUITE_NAME, FIXTURE_NAME)

// Define an ISINSTANCE_TEST macro which we use to confirm the initial state
// is a valid instance of this format.  This is defined as a macro so the
// format-specific code can reuse it later to test various invalid formats.
#define ISINSTANCE_TEST(c, d, r) \
	BOOST_AUTO_TEST_CASE(TEST_NAME(isinstance_ ## c)) \
	{ \
		BOOST_TEST_MESSAGE("isInstance check (" TILESET_TYPE "; " #c ")"); \
		\
		ManagerPtr pManager(getManager()); \
		TilesetTypePtr pTestType(pManager->getTilesetTypeByCode(TILESET_TYPE)); \
		BOOST_REQUIRE_MESSAGE(pTestType, "Could not find tileset code " TILESET_TYPE); \
		\
		stream::string_sptr test(new stream::string()); \
		test << makeString(d); \
		\
		BOOST_CHECK_EQUAL(pTestType->isInstance(test), TilesetType::r); \
	}

#ifdef TILESET_DETECTION_UNCERTAIN
ISINSTANCE_TEST(c00, test_tileset_initialstate, PossiblyYes);
#else
ISINSTANCE_TEST(c00, test_tileset_initialstate, DefinitelyYes);
#endif


// Define an INVALIDDATA_TEST macro which we use to confirm the reader correctly
// rejects a file with invalid data.  This is defined as a macro so the
// format-specific code can reuse it later to test various invalid formats.
#ifdef HAS_FAT
#	define INVALIDDATA_FATCODE(d) \
	{ \
		boost::shared_ptr<std::stringstream> suppSS(new std::stringstream); \
		suppSS->exceptions(std::ios::badbit | std::ios::failbit | std::ios::eofbit); \
		(*suppSS) << makeString(d); \
		camoto::stream::inout_sptr suppStream(suppSS); \
		camoto::SuppItem si; \
		si.stream = suppStream; \
		si.fnTruncate = boost::bind<void>(stringStreamTruncate, suppSS.get(), _1); \
		suppData[EST_FAT] = si; \
	}
#else
#	define INVALIDDATA_FATCODE(d)
#endif

#define INVALIDDATA_TEST(c, d) \
	INVALIDDATA_TEST_FULL(c, d, 0)

#define INVALIDDATA_TEST_FAT(c, d, f) \
	INVALIDDATA_TEST_FULL(c, d, f)

#define INVALIDDATA_TEST_FULL(c, d, f) \
	/* Run an isInstance test first to make sure the data is accepted */ \
	ISINSTANCE_TEST(invaliddata_ ## c, d, EC_DEFINITELY_YES); \
	\
	BOOST_AUTO_TEST_CASE(TEST_NAME(invaliddata_ ## c)) \
	{ \
		BOOST_TEST_MESSAGE("invalidData check (" TILESET_TYPE "; " #c ")"); \
		\
		boost::shared_ptr<Manager> pManager(getManager()); \
		TilesetTypePtr pTestType(pManager->getTilesetTypeByCode(TILESET_TYPE)); \
		\
		/* Prepare an invalid tileset */ \
		boost::shared_ptr<std::stringstream> psstrBase(new std::stringstream); \
		(*psstrBase) << makeString(d); \
		camoto::stream::inout_sptr psBase(psstrBase); \
		\
		camoto::SuppData suppData; \
		INVALIDDATA_FATCODE(f) \
		\
		BOOST_CHECK_THROW( \
			TilesetPtr pTileset(pTestType->open(psBase, suppData)), \
			stream::error \
		); \
	}


BOOST_AUTO_TEST_CASE(TEST_NAME(open))
{
	BOOST_TEST_MESSAGE("Opening image in tileset");

	const Tileset::VC_ENTRYPTR& tiles = pTileset->getItems();

	ImagePtr img(pTileset->openImage(tiles[0]));
	StdImageDataPtr output = img->toStandard();
	MAKE_IMAGE(rawImage, 1);

	BOOST_CHECK_MESSAGE(
		default_sample::is_equal(
			rawImage.get(),
			output.get(),
			DATA_TILE_WIDTH * DATA_TILE_HEIGHT,
			DATA_TILE_WIDTH
		),
		"Error opening tile or wrong tile opened (or image format conversion "
		"failed)"
	);
}

BOOST_AUTO_TEST_CASE(TEST_NAME(insert_end))
{
	BOOST_TEST_MESSAGE("Inserting image into tileset");

	// Insert tile
	Tileset::EntryPtr epNew =
		pTileset->insert(Tileset::EntryPtr(), Tileset::None);

	// Make sure it went in ok
	BOOST_REQUIRE_MESSAGE(epNew->isValid, "Couldn't insert new tile");

	// Open new tile and populate with image data
	setTileData(epNew, 3, 0);

	BOOST_CHECK_MESSAGE(
		is_equal(makeString(TEST_RESULT(insert_end))),
		"Error inserting tile at end of tileset"
	);

#ifdef EXTERNAL_FAT
	BOOST_CHECK_MESSAGE(
		is_supp_equal(EST_FAT, makeString(TEST_RESULT(FAT_insert_end))),
		"Error inserting tile at end of tileset"
	);
#endif
}

BOOST_AUTO_TEST_CASE(TEST_NAME(insert_mid))
{
	BOOST_TEST_MESSAGE("Inserting tile into middle of tileset");

	// Find the tile we're going to insert before
	const Tileset::VC_ENTRYPTR& tiles = pTileset->getItems();
	Tileset::EntryPtr epBefore = tiles[1]; // quick hack

	// Make sure we found it
	BOOST_REQUIRE_MESSAGE(epBefore->isValid,
		"Couldn't find second tile in sample tileset");

	// Insert the tile
	Tileset::EntryPtr epNew =
		pTileset->insert(epBefore, Tileset::None);

	// Make sure it went in ok
	BOOST_REQUIRE_MESSAGE(epNew->isValid, "Couldn't insert new tile");

	// Populate the tile with image data
	setTileData(epNew, 3, 0);

	BOOST_CHECK_MESSAGE(
		is_equal(makeString(TEST_RESULT(insert_mid))),
		"Error inserting tile in middle of tileset"
	);

#ifdef HAS_FAT
	BOOST_CHECK_MESSAGE(
		is_supp_equal(EST_FAT, makeString(TEST_RESULT(FAT_insert_mid))),
		"Error inserting tile in middle of tileset"
	);
#endif
}

BOOST_AUTO_TEST_CASE(TEST_NAME(insert2))
{
	BOOST_TEST_MESSAGE("Inserting multiple tiles");

	// Find the tile we're going to insert before
	const Tileset::VC_ENTRYPTR& tiles = pTileset->getItems();
	Tileset::EntryPtr epBefore = tiles[1]; // quick hack

	// Make sure we found it
	BOOST_REQUIRE_MESSAGE(epBefore->isValid,
		"Couldn't find second tile in sample tileset");

	// Insert the tile
	Tileset::EntryPtr ep1 = pTileset->insert(epBefore, Tileset::None);

	// Make sure it went in ok
	BOOST_REQUIRE_MESSAGE(ep1->isValid,
		"Couldn't insert first new tile in sample tileset");

	// Populate the tile with image data
	setTileData(ep1, 3, 0);


	// Find the tile we're going to insert before
	epBefore = tiles[2]; // quick hack

	// Make sure we found it
	BOOST_REQUIRE_MESSAGE(epBefore->isValid,
		"Couldn't find second tile in sample tileset");

	// Insert the tile
	Tileset::EntryPtr ep2 = pTileset->insert(epBefore, Tileset::None);

	// Make sure it went in ok
	BOOST_REQUIRE_MESSAGE(ep2->isValid,
		"Couldn't insert second new tile in sample tileset");

	// Populate the tile with image data
	setTileData(ep2, 4, 0);

	BOOST_CHECK_MESSAGE(
		is_equal(makeString(TEST_RESULT(insert2))),
		"Error inserting two tiles"
	);

#ifdef HAS_FAT
	BOOST_CHECK_MESSAGE(
		is_supp_equal(EST_FAT, makeString(TEST_RESULT(FAT_insert2))),
		"Error inserting two tiles"
	);
#endif
}

BOOST_AUTO_TEST_CASE(TEST_NAME(remove))
{
	BOOST_TEST_MESSAGE("Removing tile from tileset");

	// Find the tile we're going to insert before
	const Tileset::VC_ENTRYPTR& tiles = pTileset->getItems();
	Tileset::EntryPtr ep = tiles[0]; // quick hack

	// Make sure we found it
	BOOST_REQUIRE_MESSAGE(ep->isValid,
		"Couldn't find first tile in sample tileset");

	// Remove it
	pTileset->remove(ep);

	BOOST_CHECK_MESSAGE(
		is_equal(makeString(TEST_RESULT(remove))),
		"Error removing tile"
	);

#ifdef HAS_FAT
	BOOST_CHECK_MESSAGE(
		is_supp_equal(EST_FAT, makeString(TEST_RESULT(FAT_remove))),
		"Error removing tile"
	);
#endif
}

BOOST_AUTO_TEST_CASE(TEST_NAME(remove2))
{
	BOOST_TEST_MESSAGE("Removing multiple tiles from tileset");

	// Find the tile we're going to insert before
	const Tileset::VC_ENTRYPTR& tiles = pTileset->getItems();
	Tileset::EntryPtr ep1 = tiles[0]; // quick hack
	Tileset::EntryPtr ep2 = tiles[1]; // quick hack

	// Make sure we found it
	BOOST_REQUIRE_MESSAGE(ep1->isValid,
		"Couldn't find first tile in sample tileset");
	BOOST_REQUIRE_MESSAGE(ep2->isValid,
		"Couldn't find second tile in sample tileset");

	// Remove it
	pTileset->remove(ep1);
	pTileset->remove(ep2);

	BOOST_CHECK_MESSAGE(
		is_equal(makeString(TEST_RESULT(remove2))),
		"Error removing multiple tiles"
	);

#ifdef HAS_FAT
	BOOST_CHECK_MESSAGE(
		is_supp_equal(EST_FAT, makeString(TEST_RESULT(FAT_remove2))),
		"Error removing multiple tiles"
	);
#endif
}

BOOST_AUTO_TEST_CASE(TEST_NAME(insert_remove))
{
	BOOST_TEST_MESSAGE("Insert then remove tile from tileset");

	// Find the tile we're going to insert before
	const Tileset::VC_ENTRYPTR& tiles = pTileset->getItems();
	Tileset::EntryPtr epBefore = tiles[0]; // quick hack

	// Make sure we found it
	BOOST_REQUIRE_MESSAGE(epBefore->isValid,
		"Couldn't find second tile in sample tileset");

	// Insert the tile
	Tileset::EntryPtr ep1 = pTileset->insert(epBefore, Tileset::None);

	// Make sure it went in ok
	BOOST_REQUIRE_MESSAGE(ep1->isValid,
		"Couldn't insert new tile in sample tileset");

	// Populate the tile with image data
	setTileData(ep1, 3, 0);

	Tileset::EntryPtr ep2 = tiles[1]; // quick hack

	// Make sure we found it
	BOOST_REQUIRE_MESSAGE(ep2->isValid,
		"Couldn't find first (now second) tile in sample tileset");

	// Remove it
	pTileset->remove(ep2);

	BOOST_CHECK_MESSAGE(
		is_equal(makeString(TEST_RESULT(insert_remove))),
		"Error inserting then removing tile"
	);

#ifdef HAS_FAT
	BOOST_CHECK_MESSAGE(
		is_supp_equal(EST_FAT, makeString(TEST_RESULT(FAT_insert_remove))),
		"Error inserting then removing tile"
	);
#endif
}

BOOST_AUTO_TEST_CASE(TEST_NAME(remove_insert))
{
	BOOST_TEST_MESSAGE("Remove then insert tile from tileset");

	// Find the tile we're going to insert before
	const Tileset::VC_ENTRYPTR& tiles = pTileset->getItems();
	Tileset::EntryPtr ep1 = tiles[0]; // quick hack

	// Make sure we found it
	BOOST_REQUIRE_MESSAGE(ep1->isValid,
		"Couldn't find first tile in sample tileset");

	// Remove it
	pTileset->remove(ep1);

	// At this point, tile2 is the first (and only) tile in the tileset
	ep1 = tiles[0]; // quick hack

	// Make sure we found it
	BOOST_REQUIRE_MESSAGE(ep1->isValid,
		"Couldn't find first tile in sample tileset");

	// Insert the tile
	Tileset::EntryPtr ep2 = pTileset->insert(ep1, Tileset::None);

	// Make sure it went in ok
	BOOST_REQUIRE_MESSAGE(ep2->isValid,
		"Couldn't insert new tile in sample tileset");

	// Populate the tile with image data
	setTileData(ep2, 3, 0);

	BOOST_CHECK_MESSAGE(
		// This test checks against the insert_remove result instead, as the end
		// result should be the same as that test.
		is_equal(makeString(TEST_RESULT(insert_remove))),
		"Error removing then inserting tile"
	);

#ifdef HAS_FAT
	BOOST_CHECK_MESSAGE(
		// Again, use insert_remove result instead
		is_supp_equal(EST_FAT, makeString(TEST_RESULT(FAT_insert_remove))),
		"Error removing then inserting tile"
	);
#endif
}

// Remove all the tiles from the tileset, then add them back in again.  This
// differs from the insert/remove tests above as it takes the tileset to the
// point where it has no tiles at all.
BOOST_AUTO_TEST_CASE(TEST_NAME(remove_all_re_add))
{
	BOOST_TEST_MESSAGE("Remove all tiles then add them again");

	const Tileset::VC_ENTRYPTR& tiles = pTileset->getItems();
	Tileset::EntryPtr ep1 = tiles[0]; // quick hack

	// Make sure we found it
	BOOST_REQUIRE_MESSAGE(ep1->isValid,
		"Couldn't find first tile in sample tileset");

	// Remove it
	pTileset->remove(ep1);


	Tileset::EntryPtr ep2 = tiles[0]; // quick hack

	// Make sure we found it
	BOOST_REQUIRE_MESSAGE(ep2->isValid,
		"Couldn't find first tile in sample tileset");

	// Remove it
	pTileset->remove(ep2);

	BOOST_REQUIRE_EQUAL(tiles.size(), 0);


	// Insert the tile
	ep1 = pTileset->insert(Tileset::EntryPtr(), Tileset::None);

	// Make sure it went in ok
	BOOST_REQUIRE_MESSAGE(ep1->isValid, "Couldn't insert new tile after removing all tiles");

	// Populate the tile with image data
	setTileData(ep1, 1, 0);


	// Insert the tile
	ep2 = pTileset->insert(Tileset::EntryPtr(), Tileset::None);

	// Make sure it went in ok
	BOOST_REQUIRE_MESSAGE(ep2->isValid, "Couldn't insert new tile after removing all tiles");

	// Populate the tile with image data
	setTileData(ep2, 2, 0);


	BOOST_CHECK_MESSAGE(
		is_equal(makeString(TEST_RESULT(initialstate))),
		"Error removing all tiles then reinserting them again"
	);

#ifdef HAS_FAT
	BOOST_CHECK_MESSAGE(
		is_supp_equal(EST_FAT, makeString(TEST_RESULT(FAT_initialstate))),
		"Error removing all tiles then reinserting them again"
	);
#endif
}

//
// Metadata tests
//
/*
#ifdef testdata_get_metadata_description
BOOST_AUTO_TEST_CASE(TEST_NAME(get_metadata_description))
{
	BOOST_TEST_MESSAGE("get 'description' metadata field");

	// Make sure this format reports having a 'description' metadata field
	camoto::Metadata::MetadataTypes items = pTileset->getMetadataList();
	bool bFound = false;
	for (camoto::Metadata::MetadataTypes::iterator i = items.begin(); i != items.end(); i++) {
		if (*i == camoto::Metadata::Description) {
			bFound = true;
			break;
		}
	}
	BOOST_REQUIRE_EQUAL(bFound, true);

	// Change the field's value
	std::string value = pTileset->getMetadata(camoto::Metadata::Description);

	// Make sure we didn't read in extra data (e.g. 400MB with a broken length)
	BOOST_REQUIRE_EQUAL(value.length(), sizeof(TEST_RESULT(get_metadata_description)) - 1);

	// Put it in a stringstream to allow use of the standard checking mechanism
	std::ostringstream out;
	out << value;

	BOOST_CHECK_MESSAGE(
		default_sample::is_equal(makeString(
			TEST_RESULT(get_metadata_description)
		), out.str()),
		"Error getting 'description' metadata field"
	);

}

BOOST_AUTO_TEST_CASE(TEST_NAME(set_metadata_description_larger))
{
	BOOST_TEST_MESSAGE("Set 'description' metadata field to larger value");

	// We assume the format supports this metadata type, as this is checked in
	// get_metadata_description above.

	// Change the field's value
	pTileset->setMetadata(camoto::Metadata::Description, TEST_RESULT(set_metadata_description_target_larger));

	BOOST_CHECK_MESSAGE(
		is_equal(makeString(TEST_RESULT(set_metadata_description_larger))),
		"Error setting 'description' metadata field"
	);

#ifdef HAS_FAT
	BOOST_CHECK_MESSAGE(
		is_supp_equal(EST_FAT, makeString(TEST_RESULT(FAT_set_metadata_description_larger))),
		"Error setting 'description' metadata field"
	);
#endif
}

BOOST_AUTO_TEST_CASE(TEST_NAME(set_metadata_description_smaller))
{
	BOOST_TEST_MESSAGE("Set 'description' metadata field to smaller value");

	// We assume the format supports this metadata type, as this is checked in
	// get_metadata_description above.

	// Change the field's value
	pTileset->setMetadata(camoto::Metadata::Description, TEST_RESULT(set_metadata_description_target_smaller));

	BOOST_CHECK_MESSAGE(
		is_equal(makeString(TEST_RESULT(set_metadata_description_smaller))),
		"Error setting 'description' metadata field"
	);

#ifdef HAS_FAT
	BOOST_CHECK_MESSAGE(
		is_supp_equal(EST_FAT, makeString(TEST_RESULT(FAT_set_metadata_description_smaller))),
		"Error setting 'description' metadata field"
	);
#endif
}
#endif // testdata_get_metadata_description
*/
BOOST_AUTO_TEST_SUITE_END()

#include "test-tileset_new.hpp"
