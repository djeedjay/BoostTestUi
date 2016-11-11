// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#include "stdafx.h"
#include "resource.h"
#include "Utilities.h"
#include "BoostHelpDlg.h"

namespace gj {

BOOL CBoostHelpDlg::OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
{
	CenterWindow(GetParent());
	DlgResize_Init();

	m_sample = GetDlgItem(IDC_SAMPLE);
	SetRichEditData(m_sample, SF_RTF, MAKEINTRESOURCE(IDR_BOOSTTESTSAMPLE_RTF));

	m_link.SubclassWindow(GetDlgItem(IDC_URL));
	return TRUE;
}

void SampleContextMenu(CWindow wnd, CRichEditCtrl& ctrl, CPoint pt)
{
	CHARRANGE selRange;
	ctrl.GetSel(selRange);
	if (pt == CPoint(-1, -1))
		pt = ctrl.PosFromChar(selRange.cpMin);
	else
		ctrl.ScreenToClient(&pt);

	CRect rect;
	ctrl.GetClientRect(&rect);
	if (!rect.PtInRect(pt))
		return;

	if (selRange.cpMin == selRange.cpMax)
		ctrl.SetSel(0, -1);
	CMenu menuContext;
	menuContext.LoadMenu(IDR_SAMPLE_CONTEXTMENU);
	CMenuHandle menuPopup(menuContext.GetSubMenu(0));
	ctrl.ClientToScreen(&pt);
	menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, wnd);
}

void CBoostHelpDlg::OnContextMenu(CWindow /*wnd*/, CPoint pt)
{
	SampleContextMenu(*this, m_sample, pt);
}

void CBoostHelpDlg::OnCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	m_sample.Copy();
}

void CBoostHelpDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	EndDialog(wID);
}

} // namespace gj
