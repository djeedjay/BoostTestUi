//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the boost test library home page.

#ifndef BOOST_TESTUI_MESSAGEDLG_H
#define BOOST_TESTUI_MESSAGEDLG_H

#pragma once

#include <functional>
#include "Resource.h"

namespace gj {

class CMessageDlg : public CDialogImpl<CMessageDlg>
{
public:
	CMessageDlg(const std::wstring& msg, std::function<void ()> onHelp);

	enum { IDD = IDD_MESSAGE };

	BEGIN_MSG_MAP(CMessageDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER_EX(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCloseCmd)
		COMMAND_ID_HANDLER_EX(IDHELP, OnHelpCmd)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnCloseCmd(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnHelpCmd(UINT uNotifyCode, int nID, CWindow wndCtl);

private:
	std::wstring m_msg;
	std::function<void ()> m_onHelp;
};

} // namespace gj

#endif // BOOST_TESTUI_MESSAGEDLG_H
