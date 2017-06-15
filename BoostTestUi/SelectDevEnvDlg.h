// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#ifndef BOOST_TESTUI_SELECTDEVENVDLG_H
#define BOOST_TESTUI_SELECTDEVENVDLG_H

#pragma once

#include "Resource.h"
#include <vector>
#include <string>

namespace gj {

class SelectDevEnvDlg :
	boost::noncopyable,
	public CDialogImpl<SelectDevEnvDlg>,
	public CDialogResize<SelectDevEnvDlg>
{
public:
	explicit SelectDevEnvDlg(const std::vector<std::wstring>& names, int indexHint);

	int GetIndex() const;

	static const int IDD = IDD_SELECT_DEVENV;

	BEGIN_MSG_MAP_EX(SelectDevEnvDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
		COMMAND_HANDLER_EX(IDC_DEVENV_LIST, LBN_DBLCLK, OnDblClick)
		COMMAND_HANDLER_EX(IDC_DEVENV_LIST, LBN_SELCHANGE, OnSelChange)
		CHAIN_MSG_MAP(CDialogResize<SelectDevEnvDlg>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(SelectDevEnvDlg)
		DLGRESIZE_CONTROL(IDC_DEVENV_LIST, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X | DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDCANCEL, DLSZ_MOVE_X | DLSZ_MOVE_Y)
	END_DLGRESIZE_MAP()

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnOk(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnDblClick(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnSelChange(UINT uNotifyCode, int nID, CWindow wndCtl);

	void InitializeList();
	void UpdateUiState();

	const std::vector<std::wstring>& m_names;
	int m_index;
};

} // namespace gj

#endif // BOOST_TESTUI_SELECTDEVENVDLG_H
