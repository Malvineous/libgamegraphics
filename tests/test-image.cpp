/**
 * @file   test-image.cpp
 * @brief  Generic test code for Image class implementations.
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
#include "test-image.hpp"

using namespace camoto;
using namespace camoto::gamegraphics;

/// Check whether a supp item is present and if so that the content is correct.
#define CHECK_SUPP_ITEM(item, check_func, msg) \
	if (this->suppResult.find(camoto::SuppItem::item) != this->suppResult.end()) { \
		BOOST_CHECK_MESSAGE( \
			this->is_supp_equal(camoto::SuppItem::item, \
				this->suppResult[camoto::SuppItem::item]->check_func()), \
			"[SuppItem::" TOSTRING(item) "] " msg \
		); \
	}

/// Create a standard pattern in the given size.
Pixels createPixelData(const Point& dims)
{
	Pixels content(dims.x * dims.y, '\x00');

	// Write 0x0F across first row and 0x09 across last row
	for (unsigned int x = 0; x < dims.x; x++) {
		auto lastRow = dims.x * (dims.y - 1);
		// This is not the most efficient way to write this, but we want to make
		// use of the container's bounds checking as an additional test.
		content[x] = '\x0F';
		content[lastRow + x] = '\x09';
	}

	// Write 0x0C across first and 0x0A across last column, except first row
	for (unsigned int y = 1; y < dims.y; y++) {
		// This is not the most efficient way to write this, but we want to make
		// use of the container's bounds checking as an additional test.
		content[y * dims.x] = '\x0C';
		content[y * dims.x + (dims.x - 1)] = '\x0A';
	}

	// Bottom-right-most pixel is 0x0E
	content[dims.x * dims.y - 1] = 0x0E;

	return content;
}

/// Create a standard pattern in the given size.
Pixels createMaskData(const Point& dims, bool hit)
{
	Pixels content(dims.x * dims.y, '\x01');

	// Write 0x00 across first and last row
	for (unsigned int x = 0; x < dims.x; x++) {
		auto lastRow = dims.x * (dims.y - 1);
		// This is not the most efficient way to write this, but we want to make
		// use of the container's bounds checking as an additional test.
		content[x] = '\x00';
		content[lastRow + x] = '\x00';
	}

	// Write 0x00 across first and last column
	for (unsigned int y = 0; y < dims.y; y++) {
		// This is not the most efficient way to write this, but we want to make
		// use of the container's bounds checking as an additional test.
		content[y * dims.x] = '\x00';
		content[y * dims.x + (dims.x - 1)] = '\x00';
	}

	// Write 0x02 across second row if hitmask is enabled, on both transparent
	// and opaque pixels.
	for (unsigned int x = 0; x < dims.x; x++) {
		content[dims.x + x] |= '\x02';
	}
	return content;
}

test_image::test_image()
	:	init(false),
		numIsInstanceTests(0),
		numInvalidContentTests(1),
		numChangeMetadataTests(1)
{
	this->create = true;

	this->hasMask = true;
	this->hasHitmask = true;

	this->fixedDimensions = false;
	this->dimensions = {0, 0};

	this->hasMetadata[camoto::Metadata::Description] = false;
	this->hasMetadata[camoto::Metadata::Version] = false;

	this->metadataDesc = "Metadata description";
	this->metadataVer = "123";
}

void test_image::addTests()
{
	ADD_IMAGE_TEST(false, &test_image::test_isinstance_others);
	return;
}

void test_image::addBoundTest(bool empty, std::function<void()> fnTest,
	boost::unit_test::const_string name)
{
	std::function<void()> fnTestWrapper = std::bind(&test_image::runTest,
		this, empty, fnTest);
	this->ts->add(boost::unit_test::make_test_case(
		boost::unit_test::callback0<>(fnTestWrapper),
		createString(name << '[' << this->basename << ']')
	));
	return;
}

void test_image::runTest(bool empty, std::function<void()> fnTest)
{
	this->prepareTest(empty);
	fnTest();
	return;
}

void test_image::prepareTest(bool empty)
{
	// Make this->suppData valid
	this->resetSuppData(empty);
	this->populateSuppData();

	this->base = std::make_unique<stream::string>();
	return;
}

void test_image::resetSuppData(bool emptyImage)
{
	this->suppBase.clear();
	for (auto& i : this->suppResult) {
		auto& item = i.first;
		if (!i.second) {
			std::cout << "Warning: " << this->basename << " sets empty "
				<< suppToString(item) << " suppitem, ignoring.\n";
			continue;
		}
		auto suppSS = std::make_shared<stream::string>();
		if (!emptyImage) {
			// Populate the suppitem with its initial state
			*suppSS << i.second->initialstate();
		}
		this->suppBase[item] = suppSS;
	}
	return;
}

void test_image::populateSuppData()
{
	this->suppData.clear();
	for (auto& i : this->suppBase) {
		auto& item = i.first;
		auto& suppSS = i.second;
		// Wrap this in a substream to get a unique pointer, with an independent
		// seek position.
		this->suppData[item] = stream_wrap(suppSS);
	}
	return;
}

void test_image::isInstance(ImageType::Certainty result,
	const std::string& content)
{
	std::function<void()> fnTest = std::bind(&test_image::test_isInstance,
		this, result, content, this->numIsInstanceTests);
	this->ts->add(boost::unit_test::make_test_case(
			boost::unit_test::callback0<>(fnTest),
			createString("test_image[" << this->basename << "]::isinstance_c"
				<< std::setfill('0') << std::setw(2) << this->numIsInstanceTests)
		));
	this->numIsInstanceTests++;
	return;
}

void test_image::test_isInstance(ImageType::Certainty result,
	const std::string& content, unsigned int testNumber)
{
	BOOST_TEST_MESSAGE(createString("isInstance check (" << this->basename
		<< "; " << std::setfill('0') << std::setw(2) << testNumber << ")"));

	auto pTestType = ImageManager::byCode(this->type);
	BOOST_REQUIRE_MESSAGE(pTestType,
		createString("Could not find image type " << this->type));

	stream::string ss;
	ss << content;

	BOOST_CHECK_EQUAL(pTestType->isInstance(ss), result);
	return;
}

void test_image::invalidContent(const std::string& content)
{
	std::function<void()> fnTest = std::bind(&test_image::test_invalidContent,
		this, content, this->numInvalidContentTests);
	this->ts->add(boost::unit_test::make_test_case(
			boost::unit_test::callback0<>(fnTest),
			createString("test_image[" << this->basename << "]::invalidcontent_i"
				<< std::setfill('0') << std::setw(2) << this->numInvalidContentTests)
		));
	this->numInvalidContentTests++;
	return;
}

void test_image::test_invalidContent(const std::string& content,
	unsigned int testNumber)
{
	BOOST_TEST_MESSAGE(createString("invalidContent check (" << this->basename
		<< "; " << std::setfill('0') << std::setw(2) << testNumber << ")"));

	auto pTestType = ImageManager::byCode(this->type);
	BOOST_REQUIRE_MESSAGE(pTestType,
		createString("Could not find image type " << this->type));

	auto ss = std::make_unique<stream::string>();
	*ss << content;

	// Make sure isInstance reports this is valid
	BOOST_CHECK_EQUAL(pTestType->isInstance(*ss), ImageType::DefinitelyYes);

	// Make this->suppData valid again, reusing previous data
	this->populateSuppData();

	// But that we get an error when trying to open the file
	BOOST_CHECK_THROW(
		auto pImage = pTestType->open(std::move(ss), this->suppData),
		stream::error
	);

	return;
}

void test_image::changeMetadata(camoto::Metadata::MetadataType item,
	const std::string& newValue, const std::string& content)
{
	std::function<void()> fnTest = std::bind(&test_image::test_changeMetadata,
		this, item, newValue, content, this->numChangeMetadataTests);
	this->ts->add(boost::unit_test::make_test_case(
			boost::unit_test::callback0<>(fnTest),
			createString("test_image[" << this->basename << "]::changemetadata_c"
				<< std::setfill('0') << std::setw(2) << this->numIsInstanceTests)
		));
	this->numChangeMetadataTests++;
	return;
}

void test_image::test_changeMetadata(camoto::Metadata::MetadataType item,
	const std::string& newValue, const std::string& content,
	unsigned int testNumber)
{
	BOOST_REQUIRE_MESSAGE(false,
		"TODO: Implement metadata tests once first format needs them");
	return;
}

void test_image::sizedContent(const Point& dims, ImageType::Certainty result,
	const std::string& content)
{
	// Read pixels
	std::function<void()> fnTest = std::bind(&test_image::test_sizedContent_read_pix,
		this, dims, result, content);
	this->ts->add(boost::unit_test::make_test_case(
		boost::unit_test::callback0<>(fnTest),
		createString("test_image[" << this->basename << "]::sizedContent_read_pix["
			<< dims.x << "x" << dims.y << "]")
	));

	// Read mask (if present)
	if (this->hasMask || this->hasHitmask) {
		fnTest = std::bind(&test_image::test_sizedContent_read_mask,
			this, dims, result, content);
		this->ts->add(boost::unit_test::make_test_case(
			boost::unit_test::callback0<>(fnTest),
			createString("test_image[" << this->basename << "]::sizedContent_read_mask["
				<< dims.x << "x" << dims.y << "]")
		));
	}

	// Write pixels and mask
	fnTest = std::bind(&test_image::test_sizedContent_create,
		this, dims, result, content);
	this->ts->add(boost::unit_test::make_test_case(
		boost::unit_test::callback0<>(fnTest),
		createString("test_image[" << this->basename << "]::sizedContent_create["
			<< dims.x << "x" << dims.y << "]")
	));
	return;
}

void test_image::test_sizedContent_read_pix(const Point& dims,
	ImageType::Certainty result, const std::string& content)
{
	BOOST_TEST_MESSAGE(createString("sizedContent_read check (" << this->basename
		<< dims.x << "x" << dims.y << ")"));

	auto ss = std::make_unique<stream::string>(content);
	auto img = this->openImage(dims, std::move(ss), result, false);

	auto dimsReported = img->dimensions();
	BOOST_CHECK_EQUAL(dims.x, dimsReported.x);
	BOOST_CHECK_EQUAL(dims.y, dimsReported.y);

	auto pixels = img->convert();
	auto pixelsExpected = createPixelData(dimsReported);
	auto strPixels = std::string(pixels.begin(), pixels.end());
	auto strPixelsExpected = std::string(pixelsExpected.begin(), pixelsExpected.end());
	BOOST_REQUIRE_MESSAGE(
		this->is_equal(strPixelsExpected, strPixels),
		"Converting to standard pixel data produced incorrect result"
	);

	return;
}

void test_image::test_sizedContent_read_mask(const Point& dims,
	ImageType::Certainty result, const std::string& content)
{
	BOOST_TEST_MESSAGE(createString("sizedContent_read check (" << this->basename
		<< dims.x << "x" << dims.y << ")"));

	auto ss = std::make_unique<stream::string>(content);
	auto img = this->openImage(dims, std::move(ss), result, false);

	auto dimsReported = img->dimensions();
	BOOST_CHECK_EQUAL(dims.x, dimsReported.x);
	BOOST_CHECK_EQUAL(dims.y, dimsReported.y);

	assert(this->hasMask || this->hasHitmask);

	auto mask = img->convert_mask();
	auto maskExpected = createMaskData(dimsReported, this->hasHitmask);
	auto strMask = std::string(mask.begin(), mask.end());
	auto strMaskExpected = std::string(maskExpected.begin(), maskExpected.end());
	BOOST_REQUIRE_MESSAGE(
		this->is_equal(strMaskExpected, strMask),
		"Converting to standard mask data produced incorrect result"
	);

	return;
}

void test_image::test_sizedContent_create(const Point& dims,
	ImageType::Certainty result, const std::string& content)
{
	BOOST_TEST_CHECKPOINT("Init");
	BOOST_TEST_MESSAGE(createString("sizedContent_create check (" << this->basename
		<< dims.x << "x" << dims.y << ")"));

	this->resetSuppData(true); // true == empty
	this->populateSuppData();
	auto ss = std::make_unique<stream::string>();
	auto ss_data = &ss->data;

	BOOST_TEST_CHECKPOINT("openImage()");
	auto img = this->openImage(dims, std::move(ss), result, true);

	if (img->caps() & Image::Caps::SetDimensions) {
		BOOST_TEST_CHECKPOINT("Set dimensions");
		img->dimensions(dims);
	}
	BOOST_TEST_CHECKPOINT("Get dimensions");
	auto dimsReported = img->dimensions();
	BOOST_CHECK_EQUAL(dims.x, dimsReported.x);
	BOOST_CHECK_EQUAL(dims.y, dimsReported.y);

	BOOST_TEST_CHECKPOINT("Create mask data");
	Pixels maskData;
	if (this->hasMask) {
		maskData = createMaskData(dimsReported, this->hasHitmask);
	} else {
		// No mask, supply an entirely opaque one
		maskData = Pixels(dimsReported.x * dimsReported.y, '\x00');
	}
	BOOST_TEST_CHECKPOINT("Perform conversion");
	img->convert(createPixelData(dimsReported), maskData);

	BOOST_TEST_CHECKPOINT("Compare result of conversion");
	BOOST_REQUIRE_MESSAGE(
		this->is_equal(content, *ss_data),
		"Converting to native format produced incorrect result"
	);
	return;
}

boost::test_tools::predicate_result test_image::is_content_equal(
	const std::string& exp)
{
	return this->is_equal(exp, this->base->data);
}

boost::test_tools::predicate_result test_image::is_supp_equal(
	camoto::SuppItem::Type type, const std::string& strExpected)
{
	// Use the supp's test-class' own comparison function, as this will use its
	// preferred outputWidth value, which might be different to the main file's.
	return this->suppResult[type]->is_equal(strExpected,
		this->suppBase[type]->data);
}

std::unique_ptr<Image> test_image::openImage(const Point& dims,
	std::unique_ptr<stream::inout> content, ImageType::Certainty result,
	bool create)
{
	auto pTestType = ImageManager::byCode(this->type);
	BOOST_REQUIRE_MESSAGE(pTestType,
		createString("Could not find image type " << this->type));

	std::unique_ptr<Image> img;

	if (create) {
		BOOST_TEST_CHECKPOINT("Create image");
		img = pTestType->create(std::move(content), this->suppData);
	} else {
		BOOST_TEST_CHECKPOINT("isInstance check");
		BOOST_CHECK_EQUAL(pTestType->isInstance(*content), result);

		BOOST_TEST_CHECKPOINT("Open image");
		img = pTestType->open(std::move(content), this->suppData);
	}

	BOOST_REQUIRE_MESSAGE(img, "Unable to open content as image of this type.");
	return img;
}

void test_image::test_isinstance_others()
{
	// Check all file formats except this one to avoid any false positives
	BOOST_TEST_MESSAGE("isInstance check for other formats (not " << this->type
		<< ")");

	stream::string content;
	content << this->initialstate();

	for (const auto& pTestType : ImageManager::formats()) {
		// Don't check our own type, that's done by the other isinstance_* tests
		std::string otherType = pTestType->code();
		if (otherType.compare(this->type) == 0) continue;

		// Skip any formats known to produce false detections unavoidably
		if (
			std::find(
				this->skipInstDetect.begin(), this->skipInstDetect.end(), otherType
			) != this->skipInstDetect.end()) continue;

		BOOST_TEST_MESSAGE("Checking " << this->type
			<< " content against isInstance() for " << otherType);

		BOOST_CHECK_MESSAGE(pTestType->isInstance(content) < ImageType::DefinitelyYes,
			"isInstance() for " << otherType << " incorrectly recognises content for "
			<< this->type);
	}
	return;
}
