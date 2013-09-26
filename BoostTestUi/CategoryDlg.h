//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the boost test library home page.

#ifndef BOOST_TESTUI_CATEGORYDLG_H
#define BOOST_TESTUI_CATEGORYDLG_H

#pragma once

#include <map>
#include "Resource.h"

namespace gj {

class CategoryList;

class CategoryDlg : public CDialogImpl<CategoryDlg>
{
public:
	explicit CategoryDlg(CategoryList& categoryList);

	enum { IDD = IDD_CATEGORIES };

	BEGIN_MSG_MAP_EX(CategoryDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER_EX(IDC_ALL, OnBtnAll)
		COMMAND_ID_HANDLER_EX(IDC_INCLUDE, OnBtnInclude)
		COMMAND_ID_HANDLER_EX(IDC_EXCLUDE, OnBtnExclude)
		COMMAND_ID_HANDLER_EX(IDC_NONE, OnBtnNone)
		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
		COMMAND_HANDLER_EX(IDC_INCLUDE_LIST, LBN_DBLCLK, OnIncludeDblClick)
		COMMAND_HANDLER_EX(IDC_EXCLUDE_LIST, LBN_DBLCLK, OnExcludeDblClick)
		COMMAND_HANDLER_EX(IDC_INCLUDE_LIST, LBN_SELCHANGE, OnSelChange)
		COMMAND_HANDLER_EX(IDC_EXCLUDE_LIST, LBN_SELCHANGE, OnSelChange)
	END_MSG_MAP()

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnBtnAll(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnBtnInclude(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnBtnExclude(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnBtnNone(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnOk(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnIncludeDblClick(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnExcludeDblClick(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnSelChange(UINT uNotifyCode, int nID, CWindow wndCtl);

private:
	void InitializeLists();
	void UpdateUiState();
	void UpdateSelectedItem(const CListBox& box, bool value);
	void UpdateSelectedItems(const CListBox& box, bool value);

	std::map<std::string, bool> m_categories;
	CategoryList& m_categoryList;
};

} // namespace gj

#endif // BOOST_TESTUI_CATEGORYDLG_H
