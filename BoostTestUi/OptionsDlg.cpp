// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#include "stdafx.h"
#include "resource.h"
#include "Utilities.h"
#include "OptionsDlg.h"

namespace gj {

OptionsDlg::OptionsDlg(int reloadDelay) :
	m_reloadDelay(reloadDelay)
{
}

BOOL OptionsDlg::OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
{
	CenterWindow(GetParent());
	SetDlgItemInt(IDC_DELAY, m_reloadDelay, FALSE);
	CUpDownCtrl spin(GetDlgItem(IDC_SPIN));
	spin.SetRange(0, 60);
	return TRUE;
}

int OptionsDlg::ReloadDelay() const
{
	return m_reloadDelay;
}

void OptionsDlg::OnOk(UINT /*uNotifyCode*/, int nID, CWindow /*wndCtl*/)
{
	m_reloadDelay = GetDlgItemInt(IDC_DELAY, nullptr, FALSE);
	EndDialog(nID);
}

void OptionsDlg::OnCancel(UINT /*uNotifyCode*/, int nID, CWindow /*wndCtl*/)
{
	EndDialog(nID);
}

} // namespace gj
