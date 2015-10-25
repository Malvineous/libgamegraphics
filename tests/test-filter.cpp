/**
 * @file  test-filter.cpp
 * @brief Generic test code for filters.
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

#include <functional>
#include <iomanip>
#include "test-filter.hpp"

using namespace camoto;

test_filter::test_filter()
	:	numProcessTests(0),
		numFailTests(0)
{
}

void test_filter::addTests()
{
	return;
}

void test_filter::addBoundTest(bool empty, std::function<void()> fnTest,
	boost::unit_test::const_string file, std::size_t line,
	boost::unit_test::const_string name)
{
	this->ts->add(
		boost::unit_test::make_test_case(
			std::bind(&test_filter::runTest, this, empty, fnTest),
			createString(name << '[' << this->basename << ']'),
			file, line
		)
	);
	return;
}

void test_filter::runTest(bool empty, std::function<void()> fnTest)
{
	this->prepareTest(empty);
	fnTest();
	return;
}

void test_filter::prepareTest(bool empty)
{
	return;
}

void test_filter::process(std::unique_ptr<filter> algo,
	const std::string& input, const std::string& output)
{
	auto sh = std::make_shared<std::unique_ptr<filter>>(std::move(algo));
	std::function<void()> fnTest = std::bind([this, sh, input, output]() {
		auto algo = std::move(*sh.get());
		assert(algo);
		this->test_process(std::move(algo), input, output, this->numFailTests);
	});
	this->ts->add(
		boost::unit_test::make_test_case(
			fnTest,
			createString("test_filter[" << this->basename << "]::process["
				<< std::setfill('0') << std::setw(2) << this->numProcessTests << "]"),
			__FILE__, __LINE__
		)
	);
	this->numProcessTests++;
	return;
}

void test_filter::fail(std::unique_ptr<filter> algo,
	const std::string& input)
{
	auto sh = std::make_shared<std::unique_ptr<filter>>(std::move(algo));
	std::function<void()> fnTest = std::bind([this, sh, input]() {
		auto algo = std::move(*sh.get());
		assert(algo);
		this->test_fail(std::move(algo), input, this->numFailTests);
	});
	this->ts->add(
		boost::unit_test::make_test_case(
			fnTest,
			createString("test_filter[" << this->basename << "]::fail["
				<< std::setfill('0') << std::setw(2) << this->numFailTests << "]"),
			__FILE__, __LINE__
		)
	);
	this->numFailTests++;
	return;
}

void test_filter::test_process(std::unique_ptr<filter> algo,
	const std::string& input, const std::string& output, unsigned int testNumber)
{
	BOOST_TEST_CHECKPOINT("Init");
	BOOST_TEST_MESSAGE("process check (" << this->basename
		<< "; " << std::setfill('0') << std::setw(2) << testNumber << ")");
	assert(algo);

	auto in_ss = std::make_unique<stream::string>();
	in_ss->write(input);
	in_ss->seekg(0, stream::start);

	BOOST_TEST_CHECKPOINT("Construct input_filtered");
	auto in_filt = std::make_unique<stream::input_filtered>(
		std::move(in_ss),
		std::move(algo)
	);

	BOOST_TEST_CHECKPOINT("Copy to output stream");
	stream::string out;
	stream::copy(out, *in_filt);

	BOOST_REQUIRE_MESSAGE(
		this->is_equal(output, out.data),
		"Conversion failed"
	);

	return;
}

void test_filter::test_fail(std::unique_ptr<filter> algo,
	const std::string& input, unsigned int testNumber)
{
	BOOST_TEST_CHECKPOINT("Init");
	BOOST_TEST_MESSAGE(createString("fail check (" << this->basename
		<< "; " << std::setfill('0') << std::setw(2) << testNumber << ")"));
	assert(algo);

	auto in_ss = std::make_unique<stream::string>();
	in_ss->write(input);
	in_ss->seekg(0, stream::start);

	BOOST_TEST_CHECKPOINT("Construct input_filtered");
	auto in_filt = std::make_unique<stream::input_filtered>(
		std::move(in_ss),
		std::move(algo)
	);

	BOOST_TEST_CHECKPOINT("Copy to output stream");
	stream::string out;
	BOOST_REQUIRE_THROW(
		stream::copy(out, *in_filt);
		, filter_error
	);

	return;
}
