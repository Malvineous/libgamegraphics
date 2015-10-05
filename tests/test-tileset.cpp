/**
 * @file   test-tileset.cpp
 * @brief  Generic test code for Tileset class descendents.
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

#include <iomanip>
#include <functional>
#include <camoto/util.hpp>
#include <camoto/gamegraphics/tileset-from_image_list.hpp> // Tileset_FromImageList::ImageListEntry
#include "test-tileset.hpp"
#include "test-image.hpp"

using namespace camoto;
using namespace camoto::gamearchive;

/// Create a standard pattern in the given size.
Pixels createTileData(const Point& dims, bool cga, unsigned int index)
{
	auto pix = createPixelData(dims, cga);
	if ((dims.x > 0) && (dims.y > 0)) {
		// Write index at second and second-last pixel, over the top of the standard data
		pix[1] = index;
		pix[dims.x * dims.y - 2] = index;
	}
	return pix;
}

test_tileset::test_tileset()
{
	this->firstTileDims = {9999, 9999};
	this->hasMask = false;
	this->hasHitmask = false;
	this->cga = false;
}

void test_tileset::addTests()
{
	this->test_archive::addTests();
	ADD_TILESET_TEST(false, &test_tileset::test_open_image);
	return;
}

void test_tileset::prepareTest(bool emptyArchive)
{
	// Make this->suppData valid
	this->resetSuppData(emptyArchive);
	this->populateSuppData();

	this->base = std::make_unique<stream::string>();

	this->pArchive = this->openTileset(stream_wrap(this->base), emptyArchive);
	BOOST_REQUIRE_MESSAGE(this->pArchive, "Could not create tileset class");

	if (this->lenMaxFilename < 0) {
		// This format has no filenames, so set them to obviously incorrect values.
		this->filename[0] = "dummy";
		this->filename[1] = "dummy";
		this->filename[2] = "dummy";
		this->filename[3] = "dummy";
	}
	return;
}

Archive::FileHandle test_tileset::getFileAt(const Archive::FileVector& files,
	unsigned int index)
{
	bool foundType = false;
	for (const auto& i : files) {
		auto pEntry = dynamic_cast<const Tileset_FromImageList::ImageListEntry *>(&*i);
		if (pEntry) {
			foundType = true;
			if (pEntry->index == index) return i;
		}
	}

	if (!foundType) {
		// Fall back to parent implementation
		return this->test_archive::getFileAt(files, index);
	} else {
		// Found the type in the list above, but didn't match the index, so return
		// an empty pointer here.  This gives a "could not find file at index X"
		// error rather than an assertion failure.
		return nullptr;
	}
}

std::shared_ptr<Tileset> test_tileset::openTileset(
	std::unique_ptr<stream::inout> content,	bool create)
{
	auto pTilesetType = TilesetManager::byCode(this->type);
	BOOST_REQUIRE_MESSAGE(pTilesetType, "Could not find tileset type " + this->type);

	if (create) {
		BOOST_TEST_CHECKPOINT("About to create new empty instance of "
			+ this->basename);
		// This should really use BOOST_REQUIRE_NO_THROW but the message is more
		// informative without it.
		//BOOST_REQUIRE_NO_THROW(
			return pTilesetType->create(std::move(content), this->suppData);
		//);
	} else {
		*content << this->initialstate();
		BOOST_TEST_CHECKPOINT("About to open " + this->basename
			+ " initialstate as an archive");
		// This should really use BOOST_REQUIRE_NO_THROW but the message is more
		// informative without it.
		//BOOST_REQUIRE_NO_THROW(
			return pTilesetType->open(std::move(content), this->suppData);
		//);
	}
}

void test_tileset::test_isInstance(ArchiveType::Certainty result,
	const std::string& content, unsigned int testNumber)
{
	BOOST_TEST_MESSAGE(createString("isInstance check (" << this->basename
		<< "; " << std::setfill('0') << std::setw(2) << testNumber << ")"));

	auto pTestType = TilesetManager::byCode(this->type);
	BOOST_REQUIRE_MESSAGE(pTestType,
		createString("Could not find tileset type " << this->type));

	stream::string ss;
	ss << content;

	BOOST_CHECK_EQUAL(pTestType->isInstance(ss), result);
	return;
}

void test_tileset::test_isinstance_others()
{
	// Check all file formats except this one to avoid any false positives
	BOOST_TEST_MESSAGE("isInstance check for other formats (not " << this->type
		<< ")");

	stream::string content;
	content << this->initialstate();

	for (const auto& pTestType : TilesetManager::formats()) {
		// Don't check our own type, that's done by the other isinstance_* tests
		std::string otherType = pTestType->code();
		if (otherType.compare(this->type) == 0) continue;

		// Skip any formats known to produce false detections unavoidably
		if (
			std::find(
				this->skipInstDetect.begin(), this->skipInstDetect.end(), otherType
			) != this->skipInstDetect.end()) continue;

		BOOST_TEST_CHECKPOINT("Checking " << this->type
			<< " content against isInstance() for " << otherType);

		// Put this outside the BOOST_CHECK_MESSAGE macro so if an exception is
		// thrown we can see the above BOOST_TEST_CHECKPOINT message telling us which
		// handler is to blame.
		auto isInstanceResult = pTestType->isInstance(content);

		BOOST_CHECK_MESSAGE(isInstanceResult < TilesetType::DefinitelyYes,
			"isInstance() for " << otherType << " incorrectly recognises content for "
			<< this->type);
	}
	return;
}

void test_tileset::test_new_isinstance()
{
	BOOST_TEST_MESSAGE("Checking new tileset is valid instance of itself");

	this->pArchive->flush();

	auto pTestType = TilesetManager::byCode(this->type);
	BOOST_REQUIRE_MESSAGE(pTestType,
		createString("Could not find tileset type " << this->type));

	BOOST_REQUIRE_MESSAGE(pTestType->isInstance(*this->base),
		"Newly created archive was not recognised as a valid instance");

	BOOST_TEST_CHECKPOINT("New tileset reported valid, trying to open");

	// Make this->suppData valid again, reusing previous data
	this->populateSuppData();

	auto base2 = stream_wrap(this->base);

	// This should really use BOOST_REQUIRE_NO_THROW but the message is more
	// informative without it.
	//BOOST_REQUIRE_NO_THROW(
		auto pArchive = pTestType->open(std::move(base2), this->suppData);
	//);

	// Make sure there are now no files in the archive
	auto& files = pArchive->files();
	BOOST_REQUIRE_EQUAL(files.size(), 0);
}

void test_tileset::test_new_to_initialstate()
{
	// May to prepare the tileset
	auto tileset = std::dynamic_pointer_cast<Tileset>(this->pArchive);
	BOOST_REQUIRE(tileset);
	if (tileset->caps() & Tileset::Caps::SetDimensions) {
		// This tileset can have its dimensions changed, so we'll need to
		// set them to values the tests expect.
		tileset->dimensions(this->firstTileDims);
	}

	this->test_archive::test_new_to_initialstate();
}

void test_tileset::test_open_image()
{
	BOOST_TEST_MESSAGE("Opening image in tileset");

	auto tileset = std::dynamic_pointer_cast<Tileset>(this->pArchive);
	BOOST_REQUIRE(tileset);

	BOOST_TEST_CHECKPOINT("Checking global tileset dimensions");
	if (tileset->caps() & Tileset::Caps::HasDimensions) {
		auto dimsReported = tileset->dimensions();
		BOOST_CHECK_EQUAL(this->firstTileDims.x, dimsReported.x);
		BOOST_CHECK_EQUAL(this->firstTileDims.y, dimsReported.y);
	}

	auto ep = this->findFile(0);

	auto targetTileset = tileset;
	if (ep->fAttr & Archive::File::Attribute::Folder) {
		BOOST_TEST_CHECKPOINT("Opening first sub-tileset as tileset");
		targetTileset = tileset->openTileset(ep);
		ep = targetTileset->files().at(0);
	}
	BOOST_TEST_CHECKPOINT("Opening first tile as image");
	auto img = targetTileset->openImage(ep);
	BOOST_REQUIRE(img);

	BOOST_TEST_CHECKPOINT("Retrieving dimensions of first tile");
	auto dimsReported = img->dimensions();
	BOOST_CHECK_EQUAL(this->firstTileDims.x, dimsReported.x);
	BOOST_CHECK_EQUAL(this->firstTileDims.y, dimsReported.y);

	BOOST_TEST_CHECKPOINT("Converting first image to standard format");
	auto pixels = img->convert();
	auto pixelsExpected = createTileData(dimsReported, this->cga, 0);
	auto strPixels = std::string(pixels.begin(), pixels.end());
	auto strPixelsExpected = std::string(pixelsExpected.begin(), pixelsExpected.end());
	BOOST_REQUIRE_MESSAGE(
		this->is_equal(strPixelsExpected, strPixels),
		"First tile in tileset was not standard test image"
	);

	if (this->hasMask) {
		BOOST_TEST_CHECKPOINT("Convert first tile to standard mask data");
		auto mask = img->convert_mask();
		auto maskExpected = createMaskData(dimsReported, this->hasHitmask);
		auto strMask = std::string(mask.begin(), mask.end());
		auto strMaskExpected = std::string(maskExpected.begin(), maskExpected.end());
		BOOST_REQUIRE_MESSAGE(
			this->is_equal(strMaskExpected, strMask),
			"Converting first tile to standard mask data produced incorrect result"
		);
	}

	ep = this->findFile(1);

	targetTileset = tileset;
	if (ep->fAttr & Archive::File::Attribute::Folder) {
		BOOST_TEST_CHECKPOINT("Opening second sub-tileset as tileset");
		targetTileset = tileset->openTileset(ep);
		ep = targetTileset->files().at(0);
	}

	BOOST_TEST_CHECKPOINT("Opening second tile as image");
	img = targetTileset->openImage(ep);
	BOOST_REQUIRE(img);

	BOOST_TEST_CHECKPOINT("Retrieving dimensions of second tile");
	dimsReported = img->dimensions();
	BOOST_CHECK_EQUAL(this->firstTileDims.x, dimsReported.x);
	BOOST_CHECK_EQUAL(this->firstTileDims.y, dimsReported.y);

	BOOST_TEST_CHECKPOINT("Converting second image to standard format");
	pixels = img->convert();
	pixelsExpected = createTileData(dimsReported, this->cga, 1);
	strPixels = std::string(pixels.begin(), pixels.end());
	strPixelsExpected = std::string(pixelsExpected.begin(), pixelsExpected.end());
	BOOST_REQUIRE_MESSAGE(
		this->is_equal(strPixelsExpected, strPixels),
		"Second tile in tileset was not standard test image"
	);

	if (this->hasMask) {
		BOOST_TEST_CHECKPOINT("Converting second tile to standard mask data");
		auto mask = img->convert_mask();
		auto maskExpected = createMaskData(dimsReported, this->hasHitmask);
		auto strMask = std::string(mask.begin(), mask.end());
		auto strMaskExpected = std::string(maskExpected.begin(), maskExpected.end());
		BOOST_REQUIRE_MESSAGE(
			this->is_equal(strMaskExpected, strMask),
			"Converting second tile to standard mask data produced incorrect result"
		);
	}
}
