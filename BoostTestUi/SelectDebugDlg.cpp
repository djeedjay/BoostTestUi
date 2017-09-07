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
#include "SelectDebugDlg.h"

namespace gj {

SelectDebugDlg::SelectDebugDlg(const std::vector<std::wstring>& types, int selection) :
	m_types(types)
{
}

BOOL SelectDebugDlg::OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
{
	CenterWindow(GetParent());
	InitializeList();
	return TRUE;
}

void SelectDebugDlg::InitializeList()
{
	CheckRadioButton(IDC_AUTO, IDC_TYPE, IDC_AUTO);

	CTreeViewCtrl tree = GetDlgItem(IDC_TREE);
	tree.SetWindowLongPtr(GWL_STYLE, tree.GetWindowLongPtr(GWL_STYLE) | WS_CLIPCHILDREN | TVS_DISABLEDRAGDROP | TVS_NOTOOLTIPS | TVS_CHECKBOXES);

	for (auto& type : m_types)
	{
		tree.InsertItem(WStr(type), TVI_ROOT, TVI_LAST);
	}

	UpdateUiState();
}

void SelectDebugDlg::UpdateUiState()
{
	GetDlgItem(IDC_TREE).EnableWindow(IsDlgButtonChecked(IDC_TYPE) == BST_CHECKED);
}

void SelectDebugDlg::OnClicked(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	UpdateUiState();
}

void SelectDebugDlg::OnOk(UINT /*uNotifyCode*/, int nID, CWindow /*wndCtl*/)
{
	EndDialog(nID);
}

void SelectDebugDlg::OnCancel(UINT /*uNotifyCode*/, int nID, CWindow /*wndCtl*/)
{
	EndDialog(nID);
}

std::wstring SelectDebugDlg::GetSelection() const
{
	CTreeViewCtrl tree = GetDlgItem(IDC_TREE);

	auto item = tree.GetChildItem(TVI_ROOT);
	while (item != nullptr)
	{
		if (tree.GetCheckState(item))
		{
			ATL::CString value;
			tree.GetItemText(item, value);
			return value.GetString();
		}
		item = tree.GetNextSiblingItem(item);
	}

	return std::wstring();
}

} // namespace gj
