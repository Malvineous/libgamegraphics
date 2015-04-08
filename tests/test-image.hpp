/**
 * @file   test-image.hpp
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

#ifndef _CAMOTO_GAMEGRAPHICS_TEST_IMAGE_HPP_
#define _CAMOTO_GAMEGRAPHICS_TEST_IMAGE_HPP_

#include <map>
#include <functional>
#include <boost/test/unit_test.hpp>
#include <camoto/stream_string.hpp>
#include <camoto/metadata.hpp>
#include <camoto/gamegraphics.hpp>
#include "tests.hpp"

// This header will only be used by test implementations.
using namespace camoto;
using namespace camoto::gamegraphics;

/// Create a standard pattern in the given size.
Pixels createPixelData(const Point& dims);

/// Create a standard pattern in the given size.
Pixels createMaskData(const Point& dims, bool hit);

class test_image: public test_main
{
	public:
		/// Constructor sets some default values.
		test_image();

		/// Add all the standard tests.
		/**
		 * This can be overridden by descendent classes to add more tests for
		 * particular file formats.  If this is done, remember to call this
		 * function from the overridden one or the standard tests won't get run.
		 */
		virtual void addTests();

		/// Reset pImage back to a known state.
		/**
		 * @param empty
		 *   true resets to an empty archive (via ImageType::create()) while
		 *   false resets to initialstate() and calls ImageType::open().
		 */
		virtual void prepareTest(bool empty);

		void test_isinstance_others();

	protected:
		// Get content for a generic instance of the format.
		virtual std::string initialstate() const = 0;

		/// Add a test to the suite.  Used by ADD_ARCH_TEST().
		void addBoundTest(bool empty, std::function<void()> fnTest,
			boost::unit_test::const_string name);

		/// Reset the image to the initial state and run the given test.
		/**
		 * @param empty
		 *   true resets to an empty image (via ImageType::create()) while
		 *   false resets to initialstate() and calls ImageType::open().
		 *
		 * @param fnTest
		 *   Function to call once image is back to initial state.
		 */
		void runTest(bool empty, std::function<void()> fnTest);

		/// Populate suppBase with default content.
		/**
		 * This may be called mid-test if the suppBase content should be reset to
		 * the initial state.
		 */
		void resetSuppData(bool emptyImage);

		/// Populate suppData with data loaded from suppBase.
		/**
		 * This may be called mid-test if new suppData structures are needed to
		 * create a new Image instance, since the original ones have been
		 * std::move()'d to the original Image instance and won't be available to
		 * create a new Image with.
		 *
		 * This repopulates suppData from the existing suppBase content, so it is
		 * possible to access modified data this way.  If you don't want suppData
		 * that may have been modified by a previous Image instance, call
		 * resetSuppData() first to return everything to the initialstate.
		 */
		void populateSuppData();

		/// Add an isInstance check to run later.
		/**
		 * @param result
		 *   Expected result when opening the content.
		 *
		 * @param content
		 *   Content to pass as an archive to ImageType::isInstance().
		 */
		void isInstance(ImageType::Certainty result, const std::string& content);

		/// Perform an isInstance check now.
		void test_isInstance(ImageType::Certainty result,
			const std::string& content, unsigned int testNumber);

		/// Add an invalidContent check to run later.
		/**
		 * These checks make sure files that are in the correct format
		 * don't cause segfaults or infinite loops if the data is corrupted.
		 *
		 * @param content
		 *   Content to pass as an archive to ImageType::isInstance() where
		 *   it will be reported as a valid instance, then passed to
		 *   ImageType::open(), where an exception should be thrown.
		 */
		void invalidContent(const std::string& content);

		/// Add an changeMetadata check to run later.
		/**
		 * These checks make sure metadata alterations work correctly.
		 *
		 * @param item
		 *   Metadata item to change.
		 *
		 * @param newValue
		 *   New content for metadata item.
		 *
		 * @param content
		 *   Expected result after taking the initialstate() and changing the
		 *   given metadata item as specified.
		 */
		void changeMetadata(camoto::Metadata::MetadataType item,
			const std::string& newValue, const std::string& content);

		/// Add another image size and matching content to run conversion tests.
		void sizedContent(const Point& dims, ImageType::Certainty result,
			const std::string& content, std::shared_ptr<Palette> palette = nullptr);

	protected:
		/// Perform an invalidContent check now.
		void test_invalidContent(const std::string& content,
			unsigned int testNumber);

		/// Perform a changeMetadata check now.
		void test_changeMetadata(camoto::Metadata::MetadataType item,
			const std::string& newValue, const std::string& content,
			unsigned int testNumber);

		/// Perform a sizedContent check now, reading the image (pixels).
		void test_sizedContent_read_pix(const Point& dims,
			ImageType::Certainty result, const std::string& content,
			std::shared_ptr<Palette> palette);

		/// Perform a sizedContent check now, reading the image (mask).
		void test_sizedContent_read_mask(const Point& dims,
			ImageType::Certainty result, const std::string& content);

		/// Perform a sizedContent check now, creating a new image.
		void test_sizedContent_create(const Point& dims,
			ImageType::Certainty result, const std::string& content,
			std::shared_ptr<Palette> palette);

		/// Does the archive content match the parameter?
		boost::test_tools::predicate_result is_content_equal(const std::string& exp);

		/// Does the given supplementary item content match the parameter?
		boost::test_tools::predicate_result is_supp_equal(
			camoto::SuppItem::Type type, const std::string& strExpected);

		/// Create an Image instance to test.
		/**
		 * This can be overridden by a test if it wants to test an Image format
		 * that isn't exposed through an ImageType interface, e.g. raw headerless
		 * image formats like ega-planar, used by other format handlers.
		 *
		 * @param create
		 *   true to create a new instance, false to open from existing data.
		 */
		virtual std::unique_ptr<Image> openImage(const Point& dims,
			std::unique_ptr<stream::inout> content, ImageType::Certainty result,
			bool create);

		/// Underlying data stream containing image file content.
		std::shared_ptr<stream::string> base;

		/// Factory class used to open images in this format.
		ImageManager::handler_t pImageType;

		/// Pointers to the underlying storage used for suppitems.
		std::map<SuppItem::Type, std::shared_ptr<stream::string>> suppBase;

		/// Supplementary data for the archive, populated by streams sitting on
		/// top of suppBase.
		camoto::SuppData suppData;

	private:
		/// Have we allocated pImageType yet?
		bool init;

		/// Number of isInstance tests, used to number them sequentially.
		unsigned int numIsInstanceTests;

		/// Number of invalidData tests, used to number them sequentially.
		unsigned int numInvalidContentTests;

		/// Number of changeMetadata tests, used to number them sequentially.
		unsigned int numChangeMetadataTests;

		//std::map<Point, std::string> contentBySize;

	public:
		/// File type code for this format.
		std::string type;

		/// Can new instances of this format be created? (default is true)
		bool create;

		/// Any formats here identify us as an instance of that type, and it
		/// cannot be avoided.
		/**
		 * If "otherformat" is listed here then we will not pass our initialstate
		 * to otherformat's isInstance function.  This is kind of backwards but is
		 * is the way the test functions are designed.
		 */
		std::vector<std::string> skipInstDetect;

		/// Does the format support transparent pixels?
		bool hasMask;

		/// Does the format support hitmask pixels?
		bool hasHitmask;

		/// Does the image have a fixed size?
		bool fixedDimensions;

		/// If fixedDimensions is true, this is the fixed size of the image.
		/**
		 * Formats with no image (palettes) should be 0x0 here.
		 */
		Point dimensions;

		/// Does this format support metadata?
		std::map<camoto::Metadata::MetadataType, bool> hasMetadata;

		/// Default value for metadata 'description' field.
		std::string metadataDesc;

		/// Metadata text to set for 'description' when the field should be
		/// shorter than the initial state.
		/**
		 * After setting the description to this value, the archive file should
		 * match the value returned by metadata_set_desc_smaller().
		 */
		std::string metadataDescSmaller;

		/// Metadata text to set for 'description' when the field should be
		/// longer than the initial state.
		/**
		 * After setting the description to this value, the archive file should
		 * match the value returned by metadata_set_desc_larger().
		 */
		std::string metadataDescLarger;

		/// Default value for metadata 'version' field.
		std::string metadataVer;

		/// Link between supplementary items and the class containing the expected
		/// content for each test case.
		std::map<camoto::SuppItem::Type, std::unique_ptr<test_image>> suppResult;
};

/// Add a test_archive member function to the test suite
#define ADD_IMAGE_TEST(empty, fn) {	  \
	std::function<void()> fnTest = std::bind(fn, this); \
	this->test_image::addBoundTest(empty, fnTest, BOOST_TEST_STRINGIZE(fn)); \
}

#endif // _CAMOTO_GAMEGRAPHICS_TEST_IMAGE_HPP_
