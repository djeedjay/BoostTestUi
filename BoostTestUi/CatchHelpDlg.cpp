// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#include "stdafx.h"
#include "resource.h"
#include "Utilities.h"
#include "CatchHelpDlg.h"

namespace gj {

BOOL CCatchHelpDlg::OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
{
	CenterWindow(GetParent());
	DlgResize_Init();

	m_sample = GetDlgItem(IDC_SAMPLE);
	SetRichEditData(m_sample, SF_RTF, MAKEINTRESOURCE(IDR_CATCHTESTSAMPLE_RTF));

	m_link.SubclassWindow(GetDlgItem(IDC_URL));
	return TRUE;
}

void SampleContextMenu(CWindow wnd, CRichEditCtrl& ctrl, CPoint pt);

void CCatchHelpDlg::OnContextMenu(CWindow /*wnd*/, CPoint pt)
{
	SampleContextMenu(*this, m_sample, pt);
}

void CCatchHelpDlg::OnCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	m_sample.Copy();
}

void CCatchHelpDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	EndDialog(wID);
}

} // namespace gj
