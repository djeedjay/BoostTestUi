//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://boosttestui.wordpress.com/ for the boosttestui home page.

#ifndef BOOST_TESTUI_GOOGLEHELPDLG_H
#define BOOST_TESTUI_GOOGLEHELPDLG_H

#pragma once

#include "Resource.h"

namespace gj {

class CGoogleHelpDlg :
	public CDialogImpl<CGoogleHelpDlg>,
	public CDialogResize<CGoogleHelpDlg>
{
public:
	enum { IDD = IDD_GOOGLEHELP };

	BEGIN_MSG_MAP(CGoogleHelpDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CONTEXTMENU(OnContextMenu);
		COMMAND_ID_HANDLER_EX(ID_SAMPLE_COPY, OnCopy)
		COMMAND_ID_HANDLER_EX(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCloseCmd)
		CHAIN_MSG_MAP(CDialogResize<CGoogleHelpDlg>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CGoogleHelpDlg)
		DLGRESIZE_CONTROL(IDC_DESCRIPTION, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_SAMPLE, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_SEEALSO, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_URL, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X | DLSZ_MOVE_Y)
	END_DLGRESIZE_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnContextMenu(CWindow wnd, CPoint point);
	void OnCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);

private:
	CRichEditCtrl m_sample;
	CHyperLink m_link;
};

} // namespace gj

#endif // BOOST_TESTUI_GOOGLEHELPDLG_H
