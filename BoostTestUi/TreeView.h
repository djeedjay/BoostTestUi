//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the boost test library home page.

#pragma once

#include "OffscreenPaint.h"

namespace gj {

class CMainFrame;

struct TreeViewItemState
{
	TreeViewItemState(bool enable, bool expand) :
		enable(enable), expand(expand)
	{
	}

	bool enable;
	bool expand;
};

typedef CWinTraitsOR<TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS> CTreeViewTraits;

class CTreeView :
	public CWindowImpl<CTreeView, CTreeViewCtrl, CTreeViewTraits>,
	public COffscreenPaint<CTreeView>
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
	void ExpandToView();

	TreeViewItemState GetTestItemState(unsigned id) const;
	void SetTestItemState(unsigned id, const TreeViewItemState& state);
	bool IsExpanded(unsigned id) const;
	void Expand(unsigned id, bool expand);
	bool IsChecked(unsigned id) const;
	void Check(unsigned id, bool check);
	BOOL SetExtendedState(HTREEITEM hItem, UINT stateEx);
	void EnableItem(unsigned id, bool enable);
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
	LRESULT OnCustomDraw(NMHDR* pnmh);
	LRESULT OnSelChanged(NMHDR* pnmh);
	LRESULT OnClick(NMHDR* pnmh);
	LRESULT OnRClick(NMHDR* pnmh);
	void DoPaint(CDCHandle hdc, const RECT& rcClip);

private:
	void ExpandToDepth(HTREEITEM hItem, int depth);
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
	int m_iTickG;
	int m_iCross;
	int m_iRun;
	int m_runIndex;
	HTREEITEM m_hCurrentItem;
	std::vector<HTREEITEM> m_parents;
	std::vector<int> m_levels;
	int m_depth;
	std::map<unsigned, HTREEITEM> m_items;
};

} // namespace gj
