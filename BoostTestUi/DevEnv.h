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
#include "SelectDevEnvDlg.h"

namespace gj {

class DevEnv
{
public:
	DevEnv();

	bool ShowSourceLine(const std::string& fileName, int lineNr);
	bool AttachDebugger(unsigned processId);

private:
	CComPtr<EnvDTE::_DTE> GetDte();
	bool Attach(EnvDTE80::Process2* pIProcess2);

	int m_index;
	CComPtr<EnvDTE::_DTE> m_pIDte;
	bool m_autoSelectEngine;
	std::vector<std::wstring> m_engineSelection;
};

} // namespace gj

#endif // BOOST_TESTUI_DEVENV_H
