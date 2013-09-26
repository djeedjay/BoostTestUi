//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the boost test library home page.

#include "stdafx.h"
#include <vector>
#include "resource.h"
#include "version.h"
#include "Utilities.h"
#include "CategoryList.h"
#include "CategoryDlg.h"

namespace gj {

CategoryDlg::CategoryDlg(CategoryList& categoryList) :
	m_categories(categoryList.begin(), categoryList.end()),
	m_categoryList(categoryList)
{
}

BOOL CategoryDlg::OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
{
	CenterWindow(GetParent());
	InitializeLists();
	return TRUE;
}

void ClearListBox(CListBox& box)
{
	while (box.GetCount() > 0)
		box.DeleteString(0);
}

void CategoryDlg::InitializeLists()
{
	CListBox inBox(GetDlgItem(IDC_INCLUDE_LIST));
	CListBox exBox(GetDlgItem(IDC_EXCLUDE_LIST));
	ClearListBox(inBox);
	ClearListBox(exBox);
	for (auto it = m_categories.begin(); it != m_categories.end(); ++it)
	{
		if (it->second)
			inBox.AddString(WStr(it->first));
		else
			exBox.AddString(WStr(it->first));
	}
	UpdateUiState();
}

void CategoryDlg::UpdateUiState()
{
	CListBox inBox(GetDlgItem(IDC_INCLUDE_LIST));
	CListBox exBox(GetDlgItem(IDC_EXCLUDE_LIST));

	GetDlgItem(IDC_ALL).EnableWindow(exBox.GetCount() > 0);
	GetDlgItem(IDC_INCLUDE).EnableWindow(exBox.GetSelCount() > 0);
	GetDlgItem(IDC_EXCLUDE).EnableWindow(inBox.GetSelCount() > 0);
	GetDlgItem(IDC_NONE).EnableWindow(inBox.GetCount() > 0);
}

void CategoryDlg::OnSelChange(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	UpdateUiState();
}

void CategoryDlg::OnOk(UINT /*uNotifyCode*/, int nID, CWindow /*wndCtl*/)
{
	for (auto it = m_categories.begin(); it != m_categories.end(); ++it)
		m_categoryList.SetSelection(it->first, it->second);

	EndDialog(nID);
}

void CategoryDlg::OnCancel(UINT /*uNotifyCode*/, int nID, CWindow /*wndCtl*/)
{
	EndDialog(nID);
}

std::wstring GetListBoxText(const CListBox& box, int item)
{
	std::vector<wchar_t> text(box.GetTextLen(item) + 1);
	box.GetText(item, text.data());
	return text.data();
}

void CategoryDlg::UpdateSelectedItem(const CListBox& box, bool value)
{
	int i = box.GetCaretIndex();
	if (i != LB_ERR)
		m_categories[Str(GetListBoxText(box, i))] = value;
}

void CategoryDlg::OnIncludeDblClick(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	UpdateSelectedItem(CListBox(wndCtl), false);
	InitializeLists();
}

void CategoryDlg::OnExcludeDblClick(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	UpdateSelectedItem(CListBox(wndCtl), true);
	InitializeLists();
}

void CategoryDlg::UpdateSelectedItems(const CListBox& box, bool value)
{
	int count = box.GetCount();
	for (int i = 0; i < count; ++i)
	{
		if (box.GetSel(i))
			m_categories[Str(GetListBoxText(box, i))] = value;
	}
}

void CategoryDlg::OnBtnAll(UINT /*uNotifyCode*/, int nID, CWindow /*wndCtl*/)
{
	for (auto it = m_categories.begin(); it != m_categories.end(); ++it)
		it->second = true;
	InitializeLists();
}

void CategoryDlg::OnBtnInclude(UINT /*uNotifyCode*/, int nID, CWindow /*wndCtl*/)
{
	UpdateSelectedItems(CListBox(GetDlgItem(IDC_EXCLUDE_LIST)), true);
	InitializeLists();
}

void CategoryDlg::OnBtnExclude(UINT /*uNotifyCode*/, int nID, CWindow /*wndCtl*/)
{
	UpdateSelectedItems(CListBox(GetDlgItem(IDC_INCLUDE_LIST)), false);
	InitializeLists();
}

void CategoryDlg::OnBtnNone(UINT /*uNotifyCode*/, int nID, CWindow /*wndCtl*/)
{
	for (auto it = m_categories.begin(); it != m_categories.end(); ++it)
		it->second = false;
	InitializeLists();
}

} // namespace gj
