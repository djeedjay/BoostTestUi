//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the boost test library home page.

#pragma once

#include "OffscreenDraw.h"

namespace gj {

class CMainFrame;

typedef CWinTraitsOR<TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS> CTreeViewTraits;

class CTreeView :
	public CWindowImpl<CTreeView, CTreeViewCtrl, CTreeViewTraits>,
	public COffscreenDraw<CTreeView>
{
public:
	DECLARE_WND_SUPERCLASS(nullptr, CTreeViewCtrl::GetWndClassName())

	explicit CTreeView(CMainFrame& mainFrame);

	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID);
	void ExceptionHandler();
	BOOL PreTranslateMessage(MSG* pMsg);

	void ResetTreeImages();
	void Clear();
	void AddTestCase(unsigned id, const std::string& name, bool check);
	void EnterTestSuite(unsigned id, const std::string& name, bool check);
	void LeaveTestSuite();

	bool IsChecked(unsigned id) const;
	void Check(unsigned id, bool check);
	void SelectTestItem(unsigned id);
	unsigned GetSelectedTestItem() const;

	bool IsSelected(unsigned id) const;

	void CheckAll();
	void UncheckAll();
	void CheckFailed();

	void BeginTestSuite(unsigned id);
	void BeginTestCase(unsigned id);
	void EndTestCase(unsigned id, bool succeeded);
	void EndTestSuite(unsigned id);

	void OnTestStart();
	void OnTestFinish();

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(const CREATESTRUCT* pCreate);
	void OnTimer(UINT_PTR /*nIDEvent*/);
	void OnContextMenu(HWND hWnd, CPoint pt);
	LRESULT OnSelChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);
	LRESULT OnClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);
	LRESULT OnRClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);
	void DoPaint(CDCHandle dc);

private:
	void CheckSubTreeItems(HTREEITEM hItem, bool check);
	void UncheckTreeItem(HTREEITEM hItem);
	void CheckTreeItem(HTREEITEM hItem);
	void CheckTreeItem(HTREEITEM hItem, bool check);
	int GetTestItemImage(HTREEITEM hItem) const;
	void SetTreeImages(HTREEITEM hItem, int image);
	void SetItemImage(HTREEITEM hItem, int img);
	void SetTestItemImage(unsigned id, int img);
	void SetTestFail(HTREEITEM hItem);
	void SetTestOk(HTREEITEM hItem);
	void UpdateIndicator();
	void RemoveIndicator();

	CMainFrame* m_pMainFrame;
	CImageList m_treeImg;
	int m_iEmpty;
	int m_iArrow;
	int m_iTick;
	int m_iCross;
	int m_iRun;
	int m_runIndex;
	HTREEITEM m_hCurrentItem;
	std::vector<HTREEITEM> m_parents;
	std::map<unsigned, HTREEITEM> m_items;
};

} // namespace gj
