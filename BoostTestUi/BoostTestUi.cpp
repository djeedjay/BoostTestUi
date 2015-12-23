// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#include "stdafx.h"
#include "Utilities.h"
#include "MainFrm.h"

CAppModule _Module;

namespace gj {

struct ComInitialization
{
	static const DWORD ApartmentThreaded = COINIT_APARTMENTTHREADED;
	static const DWORD Multithreaded = COINIT_MULTITHREADED;

	ComInitialization(DWORD coinit)
	{
		CheckHr(::CoInitializeEx(nullptr, coinit));
	}

	~ComInitialization()
	{
		::CoUninitialize();
	}
};

class CAppModuleInitialization
{
public:
	CAppModuleInitialization(CAppModule& module, HINSTANCE hInstance) :
		m_module(module)
	{
		CheckHr(m_module.Init(nullptr, hInstance));
	}

	~CAppModuleInitialization()
	{
		m_module.Term();
	}

private:
	CAppModule& m_module;
};

class MessageLoop : public CMessageLoop
{
public:
	explicit MessageLoop(CAppModule& module) :
		m_module(module)
	{
		if (!m_module.AddMessageLoop(this))
			throw std::runtime_error("MessageLoop initialization error");
	}

	~MessageLoop()
	{
		m_module.RemoveMessageLoop();
	}

private:
	CAppModule& m_module;
};

int Run(LPTSTR /*lpstrCmdLine*/, int nCmdShow)
{
	MessageLoop theLoop(_Module);

	std::wstring cmdLine = GetCommandLineW();
	std::wstring arguments;
	int pos = cmdLine.find(L" --args ");
	if (pos != cmdLine.npos)
	{
		arguments = cmdLine.substr(pos + 8);
		cmdLine = cmdLine.substr(0, pos);
	}

	int argc;
	auto argv = CommandLineToArgvW(cmdLine.c_str(), &argc);

	std::wstring fileName;
	std::wstring run;

	while (*argv != nullptr)
	{
		std::wstring arg = argv[0];
		std::wstring val;
		if (argv[1] != nullptr)
			val = argv[1];
		++argv;

		if (arg.substr(0, 2) != L"--")
		{
			fileName = arg;
		}
		else if (arg == L"--run" && !val.empty())
		{
			run = val;
			++argv;
		}
	}

	CMainFrame wndMain(fileName, arguments);
	LocalFree(argv);

	if (wndMain.CreateEx() == nullptr)
		ThrowLastError(L"Main window creation failed!");

	wndMain.ShowWindow(nCmdShow);
	return theLoop.Run();
}

int WINAPI Main(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	ComInitialization com(ComInitialization::ApartmentThreaded);

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(nullptr, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	CAppModuleInitialization moduleInit(_Module, hInstance);
	return Run(lpstrCmdLine, nCmdShow);
}

} // namespace gj

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpstrCmdLine, int nCmdShow)
try
{
	return gj::Main(hInstance, hPrevInstance, lpstrCmdLine, nCmdShow);
}
catch (std::exception& ex)
{
	MessageBoxA(nullptr, ex.what(), "BoostTestUi Error", MB_OK | MB_ICONERROR);
	return EXIT_FAILURE;
}
