//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the boost test library home page.

#include "stdafx.h"
#include "resource.h"
#include "Utilities.h"
#include "GoogleHelpDlg.h"

namespace gj {

BOOL CGoogleHelpDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	CenterWindow(GetParent());
	DlgResize_Init();

	m_sample = GetDlgItem(IDC_SAMPLE);
	SetRichEditData(m_sample, SF_RTF, MAKEINTRESOURCE(IDR_GOOGLETESTSAMPLE_RTF));

	m_link.SubclassWindow(GetDlgItem(IDC_URL));
	return TRUE;
}

void SampleContextMenu(CWindow wnd, CRichEditCtrl& ctrl, CPoint pt);

void CGoogleHelpDlg::OnContextMenu(CWindow wnd, CPoint pt)
{
	SampleContextMenu(wnd, m_sample, pt);
}

void CGoogleHelpDlg::OnCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	m_sample.Copy();
}

void CGoogleHelpDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	EndDialog(wID);
}

} // namespace gj
