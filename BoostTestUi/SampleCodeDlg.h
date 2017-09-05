// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#ifndef BOOST_TESTUI_SAMPLECODEDLG_H
#define BOOST_TESTUI_SAMPLECODEDLG_H

#pragma once

#include "Resource.h"

namespace gj {

class SampleCodeDlg :
	public CDialogImpl<SampleCodeDlg>,
	public CDialogResize<SampleCodeDlg>
{
public:
	const int IDD;

	SampleCodeDlg(int dlgId, int rtfId);

	BEGIN_MSG_MAP(SampleCodeDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CONTEXTMENU(OnContextMenu);
		COMMAND_ID_HANDLER_EX(ID_SAMPLE_COPY, OnCopy)
		COMMAND_ID_HANDLER_EX(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCloseCmd)
		CHAIN_MSG_MAP(CDialogResize<SampleCodeDlg>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(SampleCodeDlg)
		DLGRESIZE_CONTROL(IDC_DESCRIPTION, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_SAMPLE, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_SEEALSO, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_URL, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X | DLSZ_MOVE_Y)
	END_DLGRESIZE_MAP()

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnContextMenu(CWindow wnd, CPoint point);
	void OnCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);

	int m_rtfId;
	CRichEditCtrl m_sample;
	CHyperLink m_link;
};

} // namespace gj

#endif // BOOST_TESTUI_SAMPLECODEDLG_H
