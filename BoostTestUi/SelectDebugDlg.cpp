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

SelectDebugDlg::SelectDebugDlg(const std::vector<std::wstring>& types, bool autoSelect, const std::vector<std::wstring>& selection) :
	m_types(types),
	m_autoSelect(autoSelect),
	m_selection(selection)
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
	auto InSelection = [&](const std::wstring& type)
	{
		return std::find(m_selection.begin(), m_selection.end(), type) != m_selection.end();
	};

	CheckRadioButton(IDC_AUTO, IDC_TYPE, m_autoSelect ? IDC_AUTO : IDC_TYPE);

	CTreeViewCtrl tree = GetDlgItem(IDC_TREE);
	tree.SetWindowLongPtr(GWL_STYLE, tree.GetWindowLongPtr(GWL_STYLE) | WS_CLIPCHILDREN | TVS_DISABLEDRAGDROP | TVS_NOTOOLTIPS | TVS_CHECKBOXES);

	for (auto& type : m_types)
	{
		auto item = tree.InsertItem(WStr(type), TVI_ROOT, TVI_LAST);

		tree.SetCheckState(item, InSelection(type));
	}

	UpdateUiState();
}

void SelectDebugDlg::UpdateUiState()
{
	CTreeViewCtrl tree = GetDlgItem(IDC_TREE);
	auto item = tree.GetChildItem(TVI_ROOT);
	int count = IsDlgButtonChecked(IDC_TYPE) == BST_CHECKED ? 0 : 1;
	while (count == 0 && item != nullptr)
	{
		if (tree.GetCheckState(item))
			++count;
		item = tree.GetNextSiblingItem(item);
	}

	GetDlgItem(IDC_TREE).EnableWindow(IsDlgButtonChecked(IDC_TYPE) == BST_CHECKED);
	GetDlgItem(IDOK).EnableWindow(count > 0);
}

void SelectDebugDlg::OnClicked(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	UpdateUiState();
}

LRESULT SelectDebugDlg::OnItemChanged(LPNMHDR /*pnmh*/)
{
	UpdateUiState();
	return 0;
}

void SelectDebugDlg::OnOk(UINT /*uNotifyCode*/, int nID, CWindow /*wndCtl*/)
{
	CTreeViewCtrl tree = GetDlgItem(IDC_TREE);

	std::vector<std::wstring> selection;

	auto item = tree.GetChildItem(TVI_ROOT);
	while (item != nullptr)
	{
		if (tree.GetCheckState(item))
		{
			ATL::CString value;
			tree.GetItemText(item, value);
			selection.push_back(value.GetString());
		}
		item = tree.GetNextSiblingItem(item);
	}
	m_selection = std::move(selection);

	m_autoSelect = IsDlgButtonChecked(IDC_AUTO) == BST_CHECKED;

	EndDialog(nID);
}

void SelectDebugDlg::OnCancel(UINT /*uNotifyCode*/, int nID, CWindow /*wndCtl*/)
{
	EndDialog(nID);
}

bool SelectDebugDlg::GetAutoSelect() const
{
	return m_autoSelect;
}

std::vector<std::wstring> SelectDebugDlg::GetSelection() const
{
	return m_selection;
}

} // namespace gj
