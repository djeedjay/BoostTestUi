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

BEGIN_MSG_MAP2(CArgumentsDlg)
	MSG_WM_INITDIALOG(OnInitDialog)
	MSG_WM_GETMINMAXINFO(OnGetMinMaxInfo)
	COMMAND_ID_HANDLER_EX(IDOK, OnOk)
	COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
	CHAIN_MSG_MAP(CDialogResize<CArgumentsDlg>)
END_MSG_MAP()

CArgumentsDlg::CArgumentsDlg(const std::wstring& arguments) :
	m_arguments(arguments)
{
}

std::wstring CArgumentsDlg::GetArguments() const
{
	return m_arguments;
}

BOOL CArgumentsDlg::OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
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

void CArgumentsDlg::OnException()
{
	MessageBox(L"Unknown exception", LoadString(IDR_APPNAME).c_str(), MB_ICONERROR | MB_OK);
}

void CArgumentsDlg::OnException(const std::exception& ex)
{
	MessageBox(WStr(ex.what()), LoadString(IDR_APPNAME).c_str(), MB_ICONERROR | MB_OK);
}

} // namespace gj
