//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the boost test library home page.

#include "stdafx.h"
#include "resource.h"
#include "Utilities.h"
#include "BoostHelpDlg.h"

namespace gj {

BOOL CBoostHelpDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	CenterWindow(GetParent());
	DlgResize_Init();

	m_sample = GetDlgItem(IDC_SAMPLE);
	SetRichEditData(m_sample, SF_RTF, MAKEINTRESOURCE(IDR_BOOSTTESTSAMPLE_RTF));

	m_link.SubclassWindow(GetDlgItem(IDC_URL));
	return TRUE;
}

void CBoostHelpDlg::OnContextMenu(CWindow wnd, CPoint pt)
{
	if (pt == CPoint(-1, -1))
	{
		long begin, end;
		m_sample.GetSel(begin, end);
		pt = m_sample.PosFromChar(begin);
	}
	else
	{
		m_sample.ScreenToClient(&pt);
	}

	CRect rect;
	m_sample.GetClientRect(&rect);
	if (!rect.PtInRect(pt))
		return;

	m_sample.SetSel(0, -1);
	CMenu menuContext;
	menuContext.LoadMenu(IDR_SAMPLE_CONTEXTMENU);
	CMenuHandle menuPopup(menuContext.GetSubMenu(0));
	m_sample.ClientToScreen(&pt);
	menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, *this);
}

void CBoostHelpDlg::OnCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	m_sample.Copy();
}

void CBoostHelpDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	EndDialog(wID);
}

} // namespace gj
