#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch/catch-gui.hpp"

#include <iostream>

unsigned int Factorial(unsigned int number)
{
    return number <= 1 ? number : Factorial(number-1)*number;
}

TEST_CASE("Factorials are computed", "[factorial][tag1][tag2]")
{
	std::cout << "Factorial test enter\n";

    REQUIRE(Factorial(1) == 1);
    REQUIRE(Factorial(2) == 2);
    REQUIRE(Factorial(3) == 6);
    REQUIRE(Factorial(10) == 3628800);

	std::cout << "Factorial test leave\n";
}

TEST_CASE("Test2", "[!shouldfail]")
{
	std::cout << "Test2 enter\n";

	REQUIRE(Factorial(0) == 1);

	std::cout << "Test2 leave\n";
}

TEST_CASE("TestSections")
{
	std::vector<int> v;

	REQUIRE(v.empty());

	SECTION("Section 1")
	{
		v.push_back(1);
		REQUIRE(v.size() == 1);

		SECTION("Section 1.1")
		{
			v.push_back(2);
			REQUIRE(v.size() == 2);
		}
	}

	SECTION("Section 2")
	{
		v.emplace_back(2);
		REQUIRE(v.size() == 1);
	}
}

struct Fixture
{
	Fixture() :
		value(0)
	{
	}

	int value;
};

TEST_CASE_METHOD(Fixture, "Fixture Test Method 1")
{
	REQUIRE(value == 0);
}

TEST_CASE_METHOD(Fixture, "Fixture Test Method 2", "[!hide]")
{
	++value;
	REQUIRE(value == 1);
}
