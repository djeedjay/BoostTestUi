// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#ifndef BOOST_TESTUI_DEVENV_H
#define BOOST_TESTUI_DEVENV_H

#pragma once

#include <string>
#include <vector>
#include <functional>

namespace gj {

class DevEnv
{
public:
	DevEnv(const std::function<int(const std::vector<std::wstring>& names, int indexHint)>& selectDte);
	~DevEnv();

	bool HasSelection();
	void ClearSelection();
	void SelectDte();

	void ShowSourceLine(const std::string& fileName, int lineNr);
	bool AttachDebugger(long testProcessID);

private:
	CComPtr<IUnknown> m_pIDte;
	std::function<int(const std::vector<std::wstring>& names, int indexHint)> m_selectDte;
};

} // namespace gj

#endif // BOOST_TESTUI_DEVENV_H
