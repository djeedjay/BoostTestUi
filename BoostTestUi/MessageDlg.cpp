//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the boost test library home page.

#include "stdafx.h"
#include "resource.h"
#include "Utilities.h"
#include "MessageDlg.h"

namespace gj {

CMessageDlg::CMessageDlg(const std::wstring& msg, std::function<void ()> onHelp) :
	m_msg(msg),
	m_onHelp(onHelp)
{
}

BOOL CMessageDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	CenterWindow(GetParent());
	CStatic(GetDlgItem(IDC_ICO)).SetIcon(LoadIcon(nullptr, IDI_ERROR));
	SetDlgItemText(IDC_MESSAGE, m_msg.c_str());
	return TRUE;
}

void CMessageDlg::OnCloseCmd(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	EndDialog(nID);
}

void CMessageDlg::OnHelpCmd(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_onHelp();
}

} // namespace gj
