// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#include "stdafx.h"
#include <vector>
#include <algorithm>
#include "resource.h"
#include "version.h"
#include "Utilities.h"
#include "SelectDevEnvDlg.h"

namespace gj {

SelectDevEnvDlg::SelectDevEnvDlg(const std::vector<std::wstring>& names, int indexHint) :
	m_names(names),
	m_index(indexHint)
{
}

BOOL SelectDevEnvDlg::OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
{
	CenterWindow(GetParent());
	DlgResize_Init();
	InitializeList();
	return TRUE;
}

namespace
{

void ClearListBox(CListBox& box)
{
	while (box.GetCount() > 0)
		box.DeleteString(0);
}

int GetTextWidth(HDC hdc, const std::wstring& text)
{
	SIZE sz;
	if (!GetTextExtentPoint32W(hdc, text.c_str(), text.length(), &sz))
		ThrowLastError("GetTextExtentPoint32W");

	return sz.cx;
}

} // namespace

void SelectDevEnvDlg::InitializeList()
{
	CListBox devEnvBox(GetDlgItem(IDC_DEVENV_LIST));
	ClearListBox(devEnvBox);
	int devEnvWidth = 0;
	ScopedGdiObjectSelection hDevEnv(devEnvBox.GetDC(), devEnvBox.GetFont());

	for (auto it = m_names.begin(); it != m_names.end(); ++it)
	{
		devEnvBox.AddString(WStr(*it));
		devEnvWidth = std::max(devEnvWidth, GetTextWidth(devEnvBox.GetDC(), *it));
	}

	devEnvBox.SetCurSel(m_index);
	devEnvBox.SetHorizontalExtent(devEnvWidth);

	UpdateUiState();
}

void SelectDevEnvDlg::UpdateUiState()
{
}

void SelectDevEnvDlg::OnDblClick(UINT /*uNotifyCode*/, int nID, CWindow /*wndCtl*/)
{
	EndDialog(IDOK);
}

void SelectDevEnvDlg::OnSelChange(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	m_index = CListBox(GetDlgItem(IDC_DEVENV_LIST)).GetCurSel();
	UpdateUiState();
}

void SelectDevEnvDlg::OnOk(UINT /*uNotifyCode*/, int nID, CWindow /*wndCtl*/)
{
	EndDialog(nID);
}

void SelectDevEnvDlg::OnCancel(UINT /*uNotifyCode*/, int nID, CWindow /*wndCtl*/)
{
	EndDialog(nID);
}

int SelectDevEnvDlg::GetIndex() const
{
	return m_index;
}

} // namespace gj
