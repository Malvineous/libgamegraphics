/**
 * @file   test-img.hpp
 * @brief  Generic tileset handler test code.
 *
 * Copyright (C) 2010 Adam Nielsen <malvineous@shikadi.net>
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

//#include <boost/algorithm/string.hpp> // for case-insensitive string compare
//#include <boost/iostreams/copy.hpp>
#include <boost/bind.hpp>
#include <camoto/gamegraphics.hpp>
#include <iostream>
#include <iomanip>

#include "tests.hpp"

using namespace camoto;
using namespace camoto::gamegraphics;

const uint8_t stdformat_test_image1_8x8[] = {
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x0A
};
const uint8_t stdformat_test_mask1_8x8[] = {
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
};

const uint8_t stdformat_test_image2_8x8[] = {
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F
};
const uint8_t stdformat_test_mask2_8x8[] = {
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
};

const uint8_t stdformat_test_image3_8x8[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const uint8_t stdformat_test_mask3_8x8[] = {
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
};

// Defines to allow code reuse
#define COMBINE_CLASSNAME_EXP(c, n)  c ## _ ## n
#define COMBINE_CLASSNAME(c, n)  COMBINE_CLASSNAME_EXP(c, n)

#define TEST_VAR(n)        COMBINE_CLASSNAME(GRAPHICS_CLASS, n)
#define TEST_NAME(n)       TEST_VAR(n)
#define TEST_RESULT(n)     testdata_ ## n

#define FIXTURE_NAME       TEST_VAR(sample)
#define EMPTY_FIXTURE_NAME TEST_VAR(sample_empty)
#define SUITE_NAME         TEST_VAR(suite)
#define EMPTY_SUITE_NAME   TEST_VAR(suite_empty)
#define INITIALSTATE_NAME  TEST_RESULT(initialstate)

// Allow a string constant to be passed around with embedded nulls
#define makeString(x)  std::string((x), sizeof((x)) - 1)

struct FIXTURE_NAME: public default_sample {

	typedef boost::shared_ptr<std::stringstream> sstr_ptr;

	sstr_ptr baseData;
	void *_do; // unused var, but allows a statement to run in constructor init
	camoto::iostream_sptr baseStream;
	GraphicsPtr pGraphics;
	MP_SUPPDATA suppData;
	std::map<E_SUPPTYPE, sstr_ptr> suppBase;

	FIXTURE_NAME() :
		baseData(new std::stringstream),
		_do((*this->baseData) << makeString(INITIALSTATE_NAME)),
		baseStream(this->baseData)
	{
		BOOST_REQUIRE_NO_THROW(
			this->baseData->exceptions(std::ios::badbit | std::ios::failbit | std::ios::eofbit);

			ManagerPtr pManager(getManager());
			GraphicsTypePtr pTestType(pManager->getGraphicsTypeByCode(GRAPHICS_TYPE));
			assert(pTestType);
			this->pGraphics = pTestType->open(this->baseStream, this->suppData);
			BOOST_REQUIRE_MESSAGE(this->pGraphics, "Could not create graphics class");
			this->pGraphics->fnTruncate = boost::bind<void>(
				stringStreamTruncate, this->baseData.get(), _1);
		);
	}

	/*FIXTURE_NAME(int i) :
		baseData(new std::stringstream),
		baseStream(this->baseData)
	{
		this->baseData->exceptions(std::ios::badbit | std::ios::failbit | std::ios::eofbit);
	}

	boost::test_tools::predicate_result is_equal(const std::string& strExpected)
	{
		// Flush out any changes before we perform the check
		BOOST_CHECK_NO_THROW(
			this->pArchive->flush()
		);

		return this->default_sample::is_equal(strExpected, this->baseData->str());
	}

	boost::test_tools::predicate_result is_supp_equal(ga::E_SUPPTYPE type, const std::string& strExpected)
	{
		// Flush out any changes to the main archive before we perform the check,
		// in case this function was called first.
		BOOST_CHECK_NO_THROW(
			this->pArchive->flush()
		);

		return this->default_sample::is_equal(strExpected, this->suppBase[type]->str());
	}*/

};

BOOST_FIXTURE_TEST_SUITE(SUITE_NAME, FIXTURE_NAME)

// Define an ISINSTANCE_TEST macro which we use to confirm the initial state
// is a valid instance of this format.  This is defined as a macro so the
// format-specific code can reuse it later to test various invalid formats.
#define ISINSTANCE_TEST(c, d, r) \
	BOOST_AUTO_TEST_CASE(TEST_NAME(isinstance_ ## c)) \
	{ \
		BOOST_TEST_MESSAGE("isInstance check (" GRAPHICS_TYPE "; " #c ")"); \
		\
		ManagerPtr pManager(getManager()); \
		GraphicsTypePtr pTestType(pManager->getGraphicsTypeByCode(GRAPHICS_TYPE)); \
		\
		boost::shared_ptr<std::stringstream> psstrBase(new std::stringstream); \
		(*psstrBase) << makeString(d); \
		iostream_sptr psBase(psstrBase); \
		\
		BOOST_CHECK(pTestType->isInstance(psBase) >= r); \
	}

ISINSTANCE_TEST(c00, INITIALSTATE_NAME, EC_POSSIBLY_YES);

BOOST_AUTO_TEST_CASE(TEST_NAME(get_images))
{
	BOOST_TEST_MESSAGE("Extracting first image in first tileset");

	GraphicsPtr nextTileset = this->pGraphics->getTileset(0);
	ImageConverterPtr img = nextTileset->openImage(0);
	StdImageDataPtr output = img->toStandard();

	BOOST_CHECK_MESSAGE(
		default_sample::is_equal(
			stdformat_test_image1_8x8,
			output.get(),
			8 * 8,
			8
		),
		"Error extracting first image"
	);

	BOOST_TEST_MESSAGE("Extracting second image in first tileset");

	nextTileset = this->pGraphics->getTileset(0);
	img = nextTileset->openImage(1);
	output = img->toStandard();

	BOOST_CHECK_MESSAGE(
		default_sample::is_equal(
			stdformat_test_image2_8x8,
			output.get(),
			8 * 8,
			8
		),
		"Error extracting second image"
	);
}

BOOST_AUTO_TEST_CASE(TEST_NAME(overwrite_image))
{
	BOOST_TEST_MESSAGE("Overwriting first image in first tileset");

	{
#ifdef TEST_HAS_SUBTILESET
		GraphicsPtr nextTileset = this->pGraphics->getTileset(0);
#else
		GraphicsPtr nextTileset = this->pGraphics;
#endif
		ImageConverterPtr img = nextTileset->openImage(0);

		StdImageDataPtr stddata(new uint8_t[8*8]);
		memcpy(stddata.get(), stdformat_test_image3_8x8, 8*8);

		StdImageDataPtr stdmask(new uint8_t[8*8]);
		memcpy(stdmask.get(), stdformat_test_mask3_8x8, 8*8);

		img->fromStandard(stddata, stdmask);
	}

	BOOST_TEST_MESSAGE("Extracting first image (now modified) in first tileset");

	GraphicsPtr nextTileset = this->pGraphics->getTileset(0);
	ImageConverterPtr img = nextTileset->openImage(0);
	StdImageDataPtr output = img->toStandard();

	BOOST_CHECK_MESSAGE(
		default_sample::is_equal(
			stdformat_test_image3_8x8,
			output.get(),
			8 * 8,
			8
		),
		"Error extracting first image"
	);

	// Extract following image to make sure it hasn't been corrupted
	BOOST_TEST_MESSAGE("Extracting second image in first tileset");

	nextTileset = this->pGraphics->getTileset(0);
	img = nextTileset->openImage(1);
	output = img->toStandard();

	BOOST_CHECK_MESSAGE(
		default_sample::is_equal(
			stdformat_test_image2_8x8,
			output.get(),
			8 * 8,
			8
		),
		"Error extracting second image"
	);

	// Make sure the right amount of data was written out
	BOOST_REQUIRE_EQUAL(baseData->str().length(), sizeof(testdata_overwrite) - 1);

	// Although the images extracted ok, confirm that the correct data
	// was written to the graphics file.
	BOOST_CHECK_MESSAGE(
		default_sample::is_equal(
			(const uint8_t *)testdata_overwrite,
			(const uint8_t *)(baseData->str().c_str()),
			8 * 8,
			sizeof(testdata_overwrite)
		),
		"Error overwriting first image"
	);
}

/*
#ifdef TEST_CAN_ADD_SUBTILESET

BOOST_AUTO_TEST_CASE(TEST_NAME(append_tileset))
{
	BOOST_TEST_MESSAGE("Appending tileset");

	{
		GraphicsPtr newTileset = this->pGraphics->insertTileset(0);
		assert(newTileset);

		newTileset->setImageCount(1);
		ImageConverterPtr img = newTileset->openImage(0);
		assert(img);

		StdImageDataPtr stddata(new uint8_t[8*8]);
		assert(stddata);
		memcpy(stddata.get(), stdformat_test_image3_8x8, 8*8);

		StdImageDataPtr stdmask(new uint8_t[8*8]);
		assert(stdmask);
		memcpy(stdmask.get(), stdformat_test_mask3_8x8, 8*8);

		img->fromStandard(stddata, stdmask);
	}

	// Make sure the right amount of data was written out
	BOOST_REQUIRE_EQUAL(baseData->str().length(), sizeof(testdata_append_tileset) - 1);

	BOOST_CHECK_MESSAGE(
		default_sample::is_equal(
			(const uint8_t *)testdata_append_tileset,
			(const uint8_t *)(baseData->str().c_str()),
			8 * 8,
			sizeof(testdata_append_tileset)
		),
		"Error overwriting first image"
	);
}

#endif
*/

BOOST_AUTO_TEST_SUITE_END()
