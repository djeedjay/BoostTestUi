// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#include "stdafx.h"
#include "resource.h"
#include "version.h"
#include "Utilities.h"
#include "AboutDlg.h"

#define STRINGIZE2(x) #x
#define STRINGIZE(x) STRINGIZE2(x)

namespace gj {

BOOL CAboutDlg::OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
{
	CenterWindow(GetParent());
	m_link.SubclassWindow(GetDlgItem(IDC_BOOST_URL));
	int version[4] = { VERSION };
	SetDlgItemText(IDC_VERSION, WStr(wstringbuilder() << L"Boost Test UI V" << version[0] << L"." << version[1] << L"." << version[2] << L"." << version[3]));
	SetDlgItemText(IDC_DATE, _T(__DATE__));
	return TRUE;
}

void CAboutDlg::OnCloseCmd(UINT /*uNotifyCode*/, int nID, CWindow /*wndCtl*/)
{
	EndDialog(nID);
}

} // namespace gj
