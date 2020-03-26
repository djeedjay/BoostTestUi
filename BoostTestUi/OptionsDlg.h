// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#ifndef BOOST_TESTUI_OPTIONSDLG_H
#define BOOST_TESTUI_OPTIONSDLG_H

#pragma once

#include "Resource.h"

namespace gj {

class OptionsDlg :
	boost::noncopyable,
	public CDialogImpl<OptionsDlg>
{
public:
	explicit OptionsDlg(int reloadDelay);

	static const int IDD = IDD_OPTIONS;

	int ReloadDelay() const;

	BEGIN_MSG_MAP_EX(OptionsDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
	END_MSG_MAP()

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnOk(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl);

	int m_reloadDelay;
};

} // namespace gj

#endif // BOOST_TESTUI_OPTIONSDLG_H
