// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#ifndef BOOST_TESTUI_FINDDLG_H
#define BOOST_TESTUI_FINDDLG_H

#pragma once

#include "Resource.h"

namespace gj {

class CMainFrame;

class CFindDlg :
	boost::noncopyable,
	public CDialogImpl<CFindDlg>,
	public CMessageFilter,
	public CDialogResize<CFindDlg>
{
public:
	explicit CFindDlg(CMainFrame& mainFrame);

	enum { IDD = IDD_FIND };

	BEGIN_MSG_MAP(CFindDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_ID_HANDLER_EX(IDOK, OnNext)
		COMMAND_ID_HANDLER_EX(IDC_NEXT, OnNext)
		COMMAND_ID_HANDLER_EX(IDC_PREVIOUS, OnPrevious)
		CHAIN_MSG_MAP(CDialogResize<CFindDlg>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CFindDlg)
		DLGRESIZE_CONTROL(IDC_TEXT, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_NEXT, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_PREVIOUS, DLSZ_MOVE_X)
	END_DLGRESIZE_MAP()

private:
	BOOL PreTranslateMessage(MSG* pMsg);

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnDestroy();
	void OnGetMinMaxInfo(MINMAXINFO* pInfo);
	void OnTextChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void OnPrevious(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void OnNext(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void OnClose(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);

	CMainFrame& m_mainFrame;
};

} // namespace gj

#endif // BOOST_TESTUI_FINDDLG_H
