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

LRESULT CGoogleHelpDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());
	DlgResize_Init();

	m_link.SubclassWindow(GetDlgItem(IDC_URL));

	CRichEditCtrl sample(GetDlgItem(IDC_SAMPLE));
	SetRichEditData(sample, SF_RTF, MAKEINTRESOURCE(IDR_GOOGLETESTSAMPLE_RTF));
	return TRUE;
}

LRESULT CGoogleHelpDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

} // namespace gj
