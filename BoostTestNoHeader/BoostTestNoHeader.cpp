//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://boosttestui.wordpress.com/ for the boosttestui home page.

#define BOOST_TEST_MODULE Boost Unit Test Sample
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(TestSuite)

BOOST_AUTO_TEST_CASE(TestCase)
{
	BOOST_REQUIRE_EQUAL(1 + 1, 2);
}

BOOST_AUTO_TEST_SUITE_END()
