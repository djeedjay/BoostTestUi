// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#ifndef BOOST_TESTUI_DEBUGTYPEDLG_H
#define BOOST_TESTUI_DEBUGTYPEDLG_H

#pragma once

#include <vector>
#include <string>
#include <boost/noncopyable.hpp>
#include "Resource.h"

namespace gj {

class SelectDebugDlg :
	boost::noncopyable,
	public CDialogImpl<SelectDebugDlg>
{
public:
	explicit SelectDebugDlg(const std::vector<std::wstring>& types, int selection = -1);

	std::wstring GetSelection() const;

	static const int IDD = IDD_SELECT_DEBUG;

	BEGIN_MSG_MAP_EX(SelectDebugDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_HANDLER_EX(IDC_AUTO, BN_CLICKED, OnClicked)
		COMMAND_HANDLER_EX(IDC_TYPE, BN_CLICKED, OnClicked)
		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
	END_MSG_MAP()

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnClicked(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnOk(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl);

	void InitializeList();
	void UpdateUiState();

	const std::vector<std::wstring>& m_types;
};

} // namespace gj

#endif // BOOST_TESTUI_DEBUGTYPEDLG_H
