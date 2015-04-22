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

Pixels createPixelData(const Point& dims, bool cga)
{
	Pixels content(dims.x * dims.y, '\x00');

	// Write 0x0F across first row and 0x09 across last row
	for (unsigned int x = 0; x < dims.x; x++) {
		auto lastRow = dims.x * (dims.y - 1);
		// This is not the most efficient way to write this, but we want to make
		// use of the container's bounds checking as an additional test.
		content[x] = cga ? '\x03' : '\x0F';
		content[lastRow + x] = cga ? '\x02' : '\x09';
	}

	// Write 0x0C across first and 0x0A across last column, except first row
	for (unsigned int y = 1; y < dims.y; y++) {
		// This is not the most efficient way to write this, but we want to make
		// use of the container's bounds checking as an additional test.
		content[y * dims.x] = cga ? '\x01' : '\x0C';
		content[y * dims.x + (dims.x - 1)] = cga ? '\x02' : '\x0A';
	}

	// Bottom-right-most pixel is 0x0E
	if ((dims.y > 0) && (dims.y > 0)) {
		content[dims.x * dims.y - 1] = cga ? '\x01' : '\x0E';
	}

	return content;
}

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
	if (hit) {
		for (unsigned int x = 0; x < dims.x; x++) {
			content[dims.x + x] |= '\x02';
		}
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
	this->cga = false;

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
	const std::string& content, std::shared_ptr<const Palette> palette,
	std::string strPixelsExpected)
{
	// Read pixels
	std::function<void()> fnTest = std::bind(&test_image::test_sizedContent_read_pix,
		this, dims, result, content, palette, strPixelsExpected);
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
		this, dims, result, content, palette, strPixelsExpected);
	this->ts->add(boost::unit_test::make_test_case(
		boost::unit_test::callback0<>(fnTest),
		createString("test_image[" << this->basename << "]::sizedContent_create["
			<< dims.x << "x" << dims.y << "]")
	));
	return;
}

void test_image::test_sizedContent_read_pix(const Point& dims,
	ImageType::Certainty result, const std::string& content,
	std::shared_ptr<const Palette> palette, std::string strPixelsExpected)
{
	BOOST_TEST_MESSAGE(createString("sizedContent_read check (" << this->basename
		<< "[" << dims.x << "x" << dims.y << "])"));

	auto ss = std::make_unique<stream::string>(content);
	auto img = this->openImage(dims, std::move(ss), result, false);

	BOOST_TEST_CHECKPOINT("Get dimensions");
	auto dimsReported = img->dimensions();
	BOOST_CHECK_EQUAL(dims.x, dimsReported.x);
	BOOST_CHECK_EQUAL(dims.y, dimsReported.y);

	BOOST_TEST_CHECKPOINT("Convert to standard pixel data");
	auto pixels = img->convert();
	if ((dims.x > 0) && (dims.y > 0)) {
		if (strPixelsExpected.empty()) {
			// No target image was specified, load the standard one
			auto pixelsExpected = createPixelData(dimsReported, this->cga);
			strPixelsExpected = std::string(pixelsExpected.begin(), pixelsExpected.end());
		}
		auto strPixels = std::string(pixels.begin(), pixels.end());
		BOOST_REQUIRE_MESSAGE(
			this->is_equal(strPixelsExpected, strPixels),
			"Converting to standard pixel data produced incorrect result"
		);
	}

	// Check the palette
	if (palette) {
		BOOST_REQUIRE_MESSAGE(img->caps() & Image::Caps::HasPalette,
			"Image has palette but doesn't supply HasPalette capability flag");
		auto palImg = img->palette();
		BOOST_REQUIRE_EQUAL(palImg->size(), palette->size());
		for (unsigned int i = 0; i < palette->size(); i++) {

			BOOST_REQUIRE_MESSAGE(palette->at(i).red == palImg->at(i).red,
				"Red component of palette entry " << std::dec << i << std::hex
				<< " is incorrect (got 8:0x" << std::setfill('0')
				<< std::setw(2) << (unsigned int)palImg->at(i).red << "/6:0x"
				<< std::setw(2) << (unsigned int)pal_8to6(palImg->at(i).red)
				<< ", expected 8:0x"
				<< std::setw(2) << (unsigned int)palette->at(i).red << "/6:0x"
				<< std::setw(2) << (unsigned int)pal_8to6(palette->at(i).red) << ")");

			BOOST_REQUIRE_MESSAGE(palette->at(i).green == palImg->at(i).green,
				"Green component of palette entry " << std::dec << i << std::hex
				<< " is incorrect (got 8:0x" << std::setfill('0')
				<< std::setw(2) << (unsigned int)palImg->at(i).green << "/6:0x"
				<< std::setw(2) << (unsigned int)pal_8to6(palImg->at(i).green)
				<< ", expected 8:0x"
				<< std::setw(2) << (unsigned int)palette->at(i).green << "/6:0x"
				<< std::setw(2) << (unsigned int)pal_8to6(palette->at(i).green) << ")");

			BOOST_REQUIRE_MESSAGE(palette->at(i).blue == palImg->at(i).blue,
				"Blue component of palette entry " << std::dec << i << std::hex
				<< " is incorrect (got 8:0x" << std::setfill('0')
				<< std::setw(2) << (unsigned int)palImg->at(i).blue << "/6:0x"
				<< std::setw(2) << (unsigned int)pal_8to6(palImg->at(i).blue)
				<< ", expected 8:0x"
				<< std::setw(2) << (unsigned int)palette->at(i).blue << "/6:0x"
				<< std::setw(2) << (unsigned int)pal_8to6(palette->at(i).blue) << ")");
		}
	}
	return;
}

void test_image::test_sizedContent_read_mask(const Point& dims,
	ImageType::Certainty result, const std::string& content)
{
	BOOST_TEST_MESSAGE(createString("sizedContent_read check (" << this->basename
		<< "[" << dims.x << "x" << dims.y << "])"));

	auto ss = std::make_unique<stream::string>(content);
	auto img = this->openImage(dims, std::move(ss), result, false);

	BOOST_TEST_CHECKPOINT("Get dimensions");
	auto dimsReported = img->dimensions();
	BOOST_CHECK_EQUAL(dims.x, dimsReported.x);
	BOOST_CHECK_EQUAL(dims.y, dimsReported.y);

	assert(this->hasMask || this->hasHitmask);

	BOOST_TEST_CHECKPOINT("Convert to standard mask data");
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
	ImageType::Certainty result, const std::string& content,
	std::shared_ptr<const Palette> palette, std::string strPixelsExpected)
{
	BOOST_TEST_CHECKPOINT("Init");
	BOOST_TEST_MESSAGE(createString("sizedContent_create check (" << this->basename
		<< "[" << dims.x << "x" << dims.y << "])"));

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

	BOOST_TEST_CHECKPOINT("Set palette");
	if (img->caps() & Image::Caps::SetPalette) {
		img->palette(palette);
	}

	BOOST_TEST_CHECKPOINT("Create mask data");
	Pixels maskData;
	if (this->hasMask) {
		maskData = createMaskData(dimsReported, this->hasHitmask);
	} else {
		// No mask, supply an entirely opaque one
		maskData = Pixels(dimsReported.x * dimsReported.y, '\x00');
	}
	BOOST_TEST_CHECKPOINT("Perform conversion");
	Pixels pix;
	if (strPixelsExpected.empty()) {
		pix = createPixelData(dimsReported, this->cga);
	} else {
		pix = Pixels(strPixelsExpected.begin(), strPixelsExpected.end());
		auto imageSize = dimsReported.x * dimsReported.y;
		BOOST_REQUIRE_MESSAGE(pix.size() == imageSize,
			"Image data supplied to this test is the wrong length (got "
			<< pix.size() << " bytes, need " << imageSize << " bytes)");
	}
	img->convert(pix, maskData);

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

		BOOST_CHECKPOINT("Checking " << this->type
			<< " content against isInstance() for " << otherType);

		// Put this outside the BOOST_CHECK_MESSAGE macro so if an exception is
		// thrown we can see the above BOOST_CHECKPOINT message telling us which
		// handler is to blame.
		auto isInstanceResult = pTestType->isInstance(content);

		BOOST_CHECK_MESSAGE(isInstanceResult < ImageType::DefinitelyYes,
			"isInstance() for " << otherType << " incorrectly recognises content for "
			<< this->type);
	}
	return;
}
