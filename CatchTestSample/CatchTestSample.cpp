#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch/catch-gui.hpp"
#include "catch_reporter_teamcity.hpp"

#include <iostream>

unsigned int Factorial(unsigned int number)
{
    return number <= 1 ? number : Factorial(number-1)*number;
}

TEST_CASE("Factorials are computed", "[factorial]")
{
	std::cout << "Factorial test enter\n";

    REQUIRE(Factorial(1) == 1);
    REQUIRE(Factorial(2) == 2);
    REQUIRE(Factorial(3) == 6);
    REQUIRE(Factorial(10) == 3628800);

	std::cout << "Factorial test leave\n";
}

TEST_CASE("Test2")
{
	std::cout << "Test2 enter\n";

	REQUIRE(Factorial(0) == 1);

	std::cout << "Test2 leave\n";
}
