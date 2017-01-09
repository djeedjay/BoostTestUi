// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#include "stdafx.h"
#include "Utilities.h"
#include "SampleCodeDlg.h"

namespace gj {

SampleCodeDlg::SampleCodeDlg(int dlgId, int rtfId) :
	IDD(dlgId),
	m_rtfId(rtfId)
{
}

BOOL SampleCodeDlg::OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
{
	CenterWindow(GetParent());
	DlgResize_Init();

	m_sample = GetDlgItem(IDC_SAMPLE);
	SetRichEditData(m_sample, SF_RTF, MAKEINTRESOURCE(m_rtfId));

	m_link.SubclassWindow(GetDlgItem(IDC_URL));
	return TRUE;
}

void SampleCodeDlg::OnContextMenu(CWindow /*wnd*/, CPoint pt)
{
	CHARRANGE selRange;
	m_sample.GetSel(selRange);
	if (pt == CPoint(-1, -1))
		pt = m_sample.PosFromChar(selRange.cpMin);
	else
		m_sample.ScreenToClient(&pt);

	CRect rect;
	m_sample.GetClientRect(&rect);
	if (!rect.PtInRect(pt))
		return;

	if (selRange.cpMin == selRange.cpMax)
		m_sample.SetSel(0, -1);
	CMenu menuContext;
	menuContext.LoadMenu(IDR_SAMPLE_CONTEXTMENU);
	CMenuHandle menuPopup(menuContext.GetSubMenu(0));
	m_sample.ClientToScreen(&pt);
	menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, *this);
}

void SampleCodeDlg::OnCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	m_sample.Copy();
}

void SampleCodeDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	EndDialog(wID);
}

} // namespace gj
