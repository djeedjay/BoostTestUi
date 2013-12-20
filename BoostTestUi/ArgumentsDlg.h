//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://boosttestui.wordpress.com/ for the boosttestui home page.

#ifndef BOOST_TESTUI_ARGUMENTSDLG_H
#define BOOST_TESTUI_ARGUMENTSDLG_H

#pragma once

#include "Resource.h"

namespace gj {

class CArgumentsDlg :
	public CDialogImpl<CArgumentsDlg>,
	public CDialogResize<CArgumentsDlg>
{
public:
	explicit CArgumentsDlg(const std::wstring& arguments);
	std::wstring GetArguments() const;

	enum { IDD = IDD_ARGUMENTS };

	BEGIN_MSG_MAP(CArgumentsDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_GETMINMAXINFO(OnGetMinMaxInfo)
		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
		CHAIN_MSG_MAP(CDialogResize<CArgumentsDlg>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CArgumentsDlg)
		DLGRESIZE_CONTROL(IDC_ARGUMENTS, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDCANCEL, DLSZ_MOVE_X)
	END_DLGRESIZE_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnGetMinMaxInfo(MINMAXINFO* pInfo);
	void OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void OnOk(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);

private:
	std::wstring m_arguments;
};

} // namespace gj

#endif // BOOST_TESTUI_ARGUMENTSDLG_H
