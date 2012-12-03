#include <iostream>
#include <string>
#include "gtest/gtest.h"

namespace testing {

void InitGoogleTestGui(int* argc, char** argv)
{
	int arg = 1;
	while (arg < *argc)
	{
		std::string opt(argv[arg]);
		size_t p = opt.find('=');
		std::string name = opt.substr(0, p);
		std::string value;
		if (p != std::string::npos)
			value = opt.substr(p + 1);
		if (name == "--gui_wait")
		{
			std::cout << "#waiting" << std::endl;
			std::getchar();
		}
		else
		{
			++arg;
			continue;
		}

		--*argc;
		for (int i = arg; i < *argc; ++i)
			argv[i] = argv[i + 1];
	}
}

} // namespace testing

extern "C" __declspec(dllexport) void unit_test_type_google()
{
}
