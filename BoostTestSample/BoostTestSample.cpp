//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://boosttestui.wordpress.com/ for the boosttestui home page.


#define BOOST_TEST_MODULE Boost Test Sample
#include <boost/test/unit_test_gui.hpp>

#ifndef BOOST_TEST_MODULE

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
	using namespace boost::unit_test;

	framework::master_test_suite().p_name.value = "Exe TestRunner Test";

	return nullptr;
}

#endif

#include <boost/thread.hpp>
using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE(FailingTests)

BOOST_AUTO_TEST_SUITE(EmptySuite)
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE_ENABLE(TestFailedCheck, false)
{
	int i = 0;
	BOOST_REQUIRE_EQUAL(i, 2);
}

BOOST_AUTO_TEST_CASE_ENABLE(TestDivideByZero, true)
{
	BOOST_TEST_CHECKPOINT("About to force division by zero!");
	int i = 1, j = 0;

	i = i / j;
}

#ifdef EXERUNNER
BOOST_AUTO_TEST_CASE_ENABLE(TestAssertion, false)
{
	BOOST_CHECKPOINT("About to assert");

	assert(1 + 1 == 3);
}

BOOST_AUTO_TEST_CASE_ENABLE(TestLeak, false)
{
	BOOST_CHECKPOINT("About to leak");
	BOOST_CHECK(new int == nullptr);
}
#endif

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(SuccessTests)

BOOST_AUTO_TEST_CASE(TestOk)
{
	int i = 0;
	BOOST_CHECK_EQUAL(i, 0);
	BOOST_TEST_MESSAGE(
		"This is a very long line to demonstrate that BoostTestUI can display a line over two hundred and sixty characters in length, correctly. "
		"The Win32 listview control can not store lines with this length and it needs special effort to display longer lines in such a listview. "
		"This is beyond column two hundred seventy so if this is still visible, the view is ok.");
}

BOOST_AUTO_TEST_CASE(Test10Sec)
{
	for (int i = 0; i < 10; ++i)
	{
		BOOST_TEST_MESSAGE("Waiting for Abort");
		boost::this_thread::sleep(boost::posix_time::seconds(1));
	}
}

BOOST_AUTO_TEST_SUITE(LongRunningTests)

BOOST_AUTO_TEST_CASE(TestWait1)
{
	BOOST_TEST_MESSAGE("Waiting for Abort");
	boost::this_thread::sleep(boost::posix_time::seconds(1));
}

BOOST_AUTO_TEST_CASE(TestWait2)
{
	BOOST_TEST_MESSAGE("Waiting for Abort");
	boost::this_thread::sleep(boost::posix_time::seconds(1));
}

BOOST_AUTO_TEST_CASE(TestWait3)
{
	BOOST_TEST_MESSAGE("Waiting for Abort");
	boost::this_thread::sleep(boost::posix_time::seconds(1));
}

BOOST_AUTO_TEST_CASE(TestWait4)
{
	BOOST_TEST_MESSAGE("Waiting for Abort");
	boost::this_thread::sleep(boost::posix_time::seconds(1));
}

BOOST_AUTO_TEST_CASE(TestWait5)
{
	BOOST_TEST_MESSAGE("Waiting for Abort");
	boost::this_thread::sleep(boost::posix_time::seconds(1));
}

BOOST_AUTO_TEST_CASE(TestWait6)
{
	BOOST_TEST_MESSAGE("Waiting for Abort");
	boost::this_thread::sleep(boost::posix_time::seconds(1));
}

BOOST_AUTO_TEST_CASE(TestWait7)
{
	BOOST_TEST_MESSAGE("Waiting for Abort");
	boost::this_thread::sleep(boost::posix_time::seconds(1));
}

BOOST_AUTO_TEST_CASE(TestWait8)
{
	BOOST_TEST_MESSAGE("Waiting for Abort");
	boost::this_thread::sleep(boost::posix_time::seconds(1));
}

BOOST_AUTO_TEST_CASE(TestWait9)
{
	BOOST_TEST_MESSAGE("Waiting for Abort");
	boost::this_thread::sleep(boost::posix_time::seconds(1));
}

BOOST_AUTO_TEST_CASE(TestWait10)
{
	BOOST_TEST_MESSAGE("Waiting for Abort");
	boost::this_thread::sleep(boost::posix_time::seconds(1));
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
