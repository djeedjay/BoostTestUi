// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#include "stdafx.h"
#include "resource.h"
#include "Utilities.h"
#include "ArgumentsDlg.h"

namespace gj {

CArgumentsDlg::CArgumentsDlg(const std::wstring& arguments) :
	m_arguments(arguments)
{
}

std::wstring CArgumentsDlg::GetArguments() const
{
	return m_arguments;
}

BOOL CArgumentsDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	CenterWindow(GetParent());
	DlgResize_Init();

	SetDlgItemText(IDC_ARGUMENTS, m_arguments.c_str());
	return TRUE;
}

void CArgumentsDlg::OnGetMinMaxInfo(MINMAXINFO* pInfo)
{
	RECT rect;
	GetWindowRect(&rect);
	pInfo->ptMinTrackSize.x = 300;
	pInfo->ptMinTrackSize.y = pInfo->ptMaxTrackSize.y = rect.bottom - rect.top;
}

void CArgumentsDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	EndDialog(wID);
}

void CArgumentsDlg::OnOk(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	m_arguments = gj::GetDlgItemText(*this, IDC_ARGUMENTS);
	EndDialog(wID);
}

} // namespace gj
