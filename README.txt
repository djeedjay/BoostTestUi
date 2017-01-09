
BoostTestUi
~~~~~~~~~~~

BoostTestUi provides a Windows gui runner for boost.test and google test
based unit test executables.


How to build
------------

This is a Visual Studio 2010 project with the following dependencies:
- boost 1.43 or later
- WTL 8.0 or later
- gtest 1.6.0 or later (only required for the GTestSample sample unit test)
- Catch 1.5.8 or later

These libraries must be installed in Libraries. All projects should now build.


Boost.test support
------------------

In your unit test, replace <boost/test/unit_test.hpp> with
<boost/test/unit_test_gui.hpp>:

	#define BOOST_TEST_MODULE Exe TestRunner Test
	#include <boost/test/unit_test_gui.hpp>

	BOOST_AUTO_TEST_CASE(MyTest) { ... }

You can also provide your own init_unit_test_suite() initialization function
by omitting BOOST_TEST_MODULE, as usual.


Google test support
-------------------

In your unit test, replace #include <gtest/gtest.h> with <gtest/gtest-gui.h>:

	#include <gtest/gtest-gui.h>

	TEST(MyTest, MyTestCase) { ... }

Then add InitGoogleTestGui() to your main function body like this:

	int main(int argc, char* argv[])
	{
		testing::InitGoogleTest(&argc, argv);
		testing::InitGoogleTestGui(&argc, argv);
		return RUN_ALL_TESTS();
	}


BoostTestUi
-----------

Run BoostTestUi.exe and open a unit test executable that was build with
the gui header included. The appropriate header can be generated from the
File -> Create Header menu. Store it preferably in a sub folder named
"boost/test" or "gtest" in one of your include directories. Now select
Run from the toolbar or the test tree context menu to run the tests.


Gert-Jan de Vos
mailto:boosttestui@on.nl
