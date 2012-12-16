//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the boost test library home page.

#ifndef BOOST_TESTUI_BOOSTHELPDLG_H
#define BOOST_TESTUI_BOOSTHELPDLG_H

#pragma once

#include "Resource.h"

namespace gj {

class CBoostHelpDlg : public CDialogImpl<CBoostHelpDlg>
{
public:
	enum { IDD = IDD_BOOSTHELP };

	BEGIN_MSG_MAP(CBoostHelpDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	DWORD OnStreamInCallback(BYTE* pbBuff, long cb, long* pcb);

private:
	CHyperLink m_link;
	long m_streamIndex;
};

} // namespace gj

#endif // BOOST_TESTUI_BOOSTHELPDLG_H
