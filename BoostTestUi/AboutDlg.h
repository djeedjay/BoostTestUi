// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#ifndef BOOST_TESTUI_ABOUTDLG_H
#define BOOST_TESTUI_ABOUTDLG_H

#pragma once

#include "Resource.h"

namespace gj {

class CAboutDlg : public CDialogImpl<CAboutDlg>
{
public:
	static const int IDD = IDD_ABOUTBOX;

	BEGIN_MSG_MAP(CAboutDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER_EX(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnCloseCmd(UINT uNotifyCode, int nID, CWindow wndCtl);

private:
	CHyperLink m_link;
};

} // namespace gj

#endif // BOOST_TESTUI_ABOUTDLG_H
