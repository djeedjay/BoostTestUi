// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#pragma once

#include "AtlWinExt.h"
#include "TestCaseState.h"

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

typedef CWinTraitsOR<TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_INFOTIP> CTreeViewTraits;

class CTreeView :
	public CWindowImpl<CTreeView, CTreeViewCtrl, CTreeViewTraits>,
	public CDoubleBufferImpl<CTreeView>,
	public ExceptionHandler<CTreeView, std::exception>
{
public:
	DECLARE_WND_SUPERCLASS(nullptr, CTreeViewCtrl::GetWndClassName())

	explicit CTreeView(CMainFrame& mainFrame);

	BOOL PreTranslateMessage(MSG* pMsg);
	void DoPaint(CDCHandle hdc);

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

	void CheckSingle(unsigned id);
	void CheckAll();
	void UncheckAll();
	void CheckFailed();

	void BeginTestSuite(unsigned id);
	void BeginTestCase(unsigned id);
	void EndTestCase(unsigned id, TestCaseState::type state);
	void EndTestSuite(unsigned id);

	void OnTestStart();
	void OnTestFinish();

private:
	DECLARE_MSG_MAP()

	void OnException();
	void OnException(const std::exception& ex);
	LRESULT OnCreate(const CREATESTRUCT* pCreate);
	void OnTimer(UINT_PTR /*nIDEvent*/);
	void OnContextMenu(HWND hWnd, CPoint pt);
	LRESULT OnCustomDraw(NMHDR* pnmh);
	LRESULT OnSelChanged(NMHDR* pnmh);
	LRESULT OnGetInfoTip(NMHDR* pnmh);
	LRESULT OnClick(NMHDR* pnmh);
	LRESULT OnRClick(NMHDR* pnmh);

	void ExpandToDepth(HTREEITEM hItem, int depth);
	void CheckSubTreeItems(HTREEITEM hItem, bool check);
	void UncheckTreeItem(HTREEITEM hItem);
	void CheckTreeItem(HTREEITEM hItem);
	void CheckTreeItem(HTREEITEM hItem, bool check);
	int GetTestItemImage(HTREEITEM hItem) const;
	void SetTreeImages(HTREEITEM hItem, int image);
	void SetItemImage(HTREEITEM hItem, int img);
	void SetTestItemImage(unsigned id, int img);
	void SetTestIgnore(HTREEITEM hItem);
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
	int m_iWarn;
	int m_iRun;
	int m_runIndex;
	HTREEITEM m_hCurrentItem;
	std::vector<HTREEITEM> m_parents;
	std::vector<int> m_levels;
	int m_depth;
	std::map<unsigned, HTREEITEM> m_items;
};

} // namespace gj
