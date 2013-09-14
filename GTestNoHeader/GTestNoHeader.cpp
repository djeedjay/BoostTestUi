//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#ifdef _DEBUG
#	pragma comment(lib, "gtestd.lib")
#else
#	pragma comment(lib, "gtest.lib")
#endif

TEST(TestCase, Test)
{
	EXPECT_EQ(1 + 1, 2);
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}
