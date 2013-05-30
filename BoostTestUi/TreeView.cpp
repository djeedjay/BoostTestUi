//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the boost test library home page.

#include "stdafx.h"
#include "Resource.h"
#include "Utilities.h"
#include "MainFrm.h"
#include "TreeView.h"

namespace gj {

BEGIN_MSG_MAP_TRY(CTreeView)
	MSG_WM_CREATE(OnCreate)
	MSG_WM_TIMER(OnTimer)
	MSG_WM_CONTEXTMENU(OnContextMenu);
	REFLECTED_NOTIFY_CODE_HANDLER(TVN_SELCHANGED, OnSelChanged)
	REFLECTED_NOTIFY_CODE_HANDLER(NM_CLICK, OnClick)
	REFLECTED_NOTIFY_CODE_HANDLER(NM_RCLICK, OnRClick)
	CHAIN_MSG_MAP(COffscreenPaint<CTreeView>)
END_MSG_MAP_CATCH(ExceptionHandler)

CTreeView::CTreeView(CMainFrame& mainFrame) :
	m_pMainFrame(&mainFrame),
	m_hCurrentItem(nullptr),
	m_runIndex(0)
{
}

void CTreeView::ExceptionHandler()
{
	MessageBox(WStr(GetExceptionMessage()), LoadString(IDR_APPNAME).c_str(), MB_ICONERROR | MB_OK);
}

BOOL CTreeView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

LRESULT CTreeView::OnCreate(const CREATESTRUCT* /*pCreate*/)
{
	DefWindowProc();

	SetWindowLong(GWL_STYLE, GetWindowLong(GWL_STYLE) | TVS_CHECKBOXES);
	m_treeImg.Create(16, 16, ILC_COLOR24 | ILC_MASK, 16, 0);
	m_iEmpty = m_treeImg.AddIcon(AtlLoadIcon(IDI_EMPTY));
	m_iTick = m_treeImg.AddIcon(AtlLoadIcon(IDI_TICK));
	m_iCross = m_treeImg.AddIcon(AtlLoadIcon(IDI_CROSS));
	m_iRun = m_treeImg.Add(AtlLoadBitmap(IDI_RUN), RGB(255,255,255));
	SetImageList(m_treeImg, LVSIL_NORMAL);

	return 0;
}

void CTreeView::OnTimer(UINT_PTR /*nIDEvent*/)
{
	m_runIndex = (m_runIndex + 1) % 12;
	if (m_hCurrentItem)
		UpdateIndicator();
}

void CTreeView::UpdateIndicator()
{
	HTREEITEM hItem = m_hCurrentItem;
	int img = m_iRun + m_runIndex;
	while (hItem)
	{
		SetItemImage(hItem, img);
		hItem = GetParentItem(hItem);
	}
}

void CTreeView::RemoveIndicator()
{
	HTREEITEM hItem = m_hCurrentItem;
	while (hItem)
	{
		SetItemImage(hItem, m_iEmpty);
		hItem = GetParentItem(hItem);
	}
	m_hCurrentItem = nullptr;
}

void CTreeView::OnContextMenu(HWND /*hWnd*/, CPoint pt)
{
	if (pt == CPoint(-1, -1))
	{
		RECT rect;
		GetItemRect(GetSelectedItem(), &rect, true);
		pt = CPoint(rect.left, rect.bottom - 1);
	}
	else
	{
		ScreenToClient(&pt);
	}

	UINT flags = 0;
	HTREEITEM hItem = HitTest(pt, &flags);
	if ((flags & TVHT_ONITEM) == 0)
		return;

	SelectItem(hItem);
	CMenu menuContext;
	menuContext.LoadMenu(IDR_TREE_CONTEXTMENU);
	CMenuHandle menuPopup(menuContext.GetSubMenu(0));
	ClientToScreen(&pt);
	menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, *m_pMainFrame);
}

void CTreeView::CheckAll()
{
	CheckSubTreeItems(GetRootItem(), true);
}

void CTreeView::UncheckAll()
{
	CheckSubTreeItems(GetRootItem(), false);
}

int CTreeView::GetTestItemImage(HTREEITEM hItem) const
{
	int image, selectedImage;
	GetItemImage(hItem, image, selectedImage);
	return image;
}

void CTreeView::CheckFailed()
{
	for (auto it = m_items.begin(); it != m_items.end(); ++it)
		CheckTreeItem(it->second, GetTestItemImage(it->second) == m_iCross);
}

LRESULT CTreeView::OnSelChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	NMTREEVIEW* pNmTreeView = reinterpret_cast<NMTREEVIEW*>(pnmh);
	m_pMainFrame->SetLogHighLight(GetItemData(pNmTreeView->itemNew.hItem));
	return 0;
}

LRESULT CTreeView::OnClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
{
	// Q261289
	DWORD dwpos = GetMessagePos();
	CPoint pt(GET_X_LPARAM(dwpos), GET_Y_LPARAM(dwpos));
	ScreenToClient(&pt);

	UINT flags = 0;
	HTREEITEM hItem = HitTest(pt, &flags);
	if (flags & TVHT_ONITEMSTATEICON)
	{
		m_pMainFrame->EnQueue([this, hItem]() { CheckTreeItem(hItem, GetCheckState(hItem) != FALSE); });
	}

	return 0;
}

LRESULT CTreeView::OnRClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& bHandled)
{
	// Q222905
	SendMessage(WM_CONTEXTMENU, (WPARAM)m_hWnd, GetMessagePos());
	bHandled = TRUE;
	return 0;
}

void CTreeView::DoPaint(CDCHandle hdc, const RECT& rcClip)
{
	hdc.FillSolidRect(&rcClip, GetSysColor(COLOR_WINDOW));

	DefWindowProc(WM_PAINT, reinterpret_cast<WPARAM>(hdc.m_hDC), 0);
}

void CTreeView::CheckSubTreeItems(HTREEITEM hItem, bool check)
{
	SetCheckState(hItem, check);
	HTREEITEM hChild = GetChildItem(hItem);
	while (hChild)
	{
		CheckSubTreeItems(hChild, check);
		hChild = GetNextSiblingItem(hChild);
	}
}

void CTreeView::UncheckTreeItem(HTREEITEM hItem)
{
	SetCheckState(hItem, false);

	HTREEITEM hParent = GetParentItem(hItem);
	if (hParent == nullptr)
		return;

	HTREEITEM hChild = GetChildItem(hParent);
	bool allUnchecked = true;
	while (hChild)
	{
		if (GetCheckState(hChild) != FALSE)
			allUnchecked = false;
		hChild = GetNextSiblingItem(hChild);
	}
	if (allUnchecked)
		UncheckTreeItem(hParent);
}

void CTreeView::CheckTreeItem(HTREEITEM hItem)
{
	SetCheckState(hItem, true);
	if (HTREEITEM hParent = GetParentItem(hItem))
		CheckTreeItem(hParent);
}

void CTreeView::CheckTreeItem(HTREEITEM hItem, bool check)
{
	if (check)
		CheckTreeItem(hItem);
	else
		UncheckTreeItem(hItem);

	CheckSubTreeItems(hItem, check);
}

void CTreeView::SetTreeImages(HTREEITEM hItem, int image)
{
	SetItemImage(hItem, image);

	HTREEITEM hChild = GetChildItem(hItem);
	while (hChild)
	{
		SetTreeImages(hChild, image);
		hChild = GetNextSiblingItem(hChild);
	}
}

void CTreeView::ResetTreeImages()
{
	SetTreeImages(GetRootItem(), m_iEmpty);
}

void CTreeView::Clear()
{
	DeleteAllItems();
	m_parents.clear();
	m_parents.push_back(TVI_ROOT);
	m_depth = 0;
	m_levels.clear();
	m_levels.push_back(0);
}

void CTreeView::AddTestCase(unsigned id, const std::string& name, bool check)
{
	HTREEITEM hItem = InsertItem(WStr(name), m_iEmpty, m_iEmpty, m_parents.back(), TVI_LAST);
	SetItemData(hItem, id);
	SetCheckState(hItem, check);
	m_items[id] = hItem;
	++m_levels[m_depth];
}

void CTreeView::EnterTestSuite(unsigned id, const std::string& name, bool check)
{
	HTREEITEM hItem = InsertItem(WStr(name), m_iEmpty, m_iEmpty, m_parents.back(), TVI_LAST);
	SetItemData(hItem, id);
	SetCheckState(hItem, check);
	m_items[id] = hItem;

	m_parents.push_back(hItem);
	++m_levels[m_depth];
	++m_depth;
	if (m_depth >= static_cast<int>(m_levels.size()))
		m_levels.push_back(0);
}

void CTreeView::ExpandToDepth(HTREEITEM hItem, int depth)
{
	Expand(hItem, depth > 0? TVE_EXPAND: TVE_COLLAPSE);
	HTREEITEM hChild = GetChildItem(hItem);
	while (hChild)
	{
		ExpandToDepth(hChild, depth - 1);
		hChild = GetNextSiblingItem(hChild);
	}
}

void CTreeView::ExpandToView()
{
	unsigned count = 0;
	int depth = 0;
	for (auto it = m_levels.begin(); it != m_levels.end(); ++it)
	{
		count += *it;
		if (count > GetVisibleCount())
			break;
		++depth;
	}
	ExpandToDepth(TVI_ROOT, depth);
}

void CTreeView::LeaveTestSuite()
{
	m_parents.pop_back();
	--m_depth;
}

bool CTreeView::IsChecked(unsigned id) const
{
	auto it = m_items.find(id);
	return it != m_items.end() && GetCheckState(it->second) != FALSE;
}

void CTreeView::Check(unsigned id, bool check)
{
	auto it = m_items.find(id);
	if (it != m_items.end())
		SetCheckState(it->second, check);
}

void CTreeView::SelectTestItem(unsigned id)
{
	auto it = m_items.find(id);
	if (it == m_items.end())
		SelectItem(nullptr);
	else
		SelectItem(it->second);
}

unsigned CTreeView::GetSelectedTestItem() const
{
	return GetItemData(GetSelectedItem());
}

void CTreeView::SetItemImage(HTREEITEM hItem, int img)
{
	CTreeViewCtrl::SetItemImage(hItem, img, img);
}

void CTreeView::SetTestItemImage(unsigned id, int img)
{
	auto it = m_items.find(id);
	if (it != m_items.end())
		SetItemImage(it->second, img);
}

void CTreeView::BeginTestSuite(unsigned /*id*/)
{
}

void CTreeView::BeginTestCase(unsigned id)
{
	auto it = m_items.find(id);
	if (it != m_items.end())
	{
		m_hCurrentItem = it->second;
		UpdateIndicator();
	}
}

void CTreeView::SetTestFail(HTREEITEM hItem)
{
	SetItemImage(hItem, m_iCross);
}

void CTreeView::SetTestOk(HTREEITEM hItem)
{
	SetItemImage(hItem, m_iTick);
}

void CTreeView::EndTestCase(unsigned id, bool succeeded)
{
	m_hCurrentItem = nullptr;

	auto it = m_items.find(id);
	if (it == m_items.end())
		return;

	if (succeeded)
		SetTestOk(it->second);
	else
		SetTestFail(it->second);
}

void CTreeView::EndTestSuite(unsigned id)
{
	auto it = m_items.find(id);
	if (it == m_items.end())
		return;

	HTREEITEM hChild = GetChildItem(it->second);
	int suiteImg = m_iTick;
	while (hChild)
	{
		int img = GetTestItemImage(hChild);
		if (img == m_iCross)
		{
			suiteImg = m_iCross;
			break;
		}
		if (img == m_iEmpty)
			suiteImg = m_iEmpty;

		hChild = GetNextSiblingItem(hChild);
	}
	SetItemImage(it->second, suiteImg);
}

void CTreeView::OnTestStart()
{
	SetTimer(1, 100);
}

void CTreeView::OnTestFinish()
{
	KillTimer(1);
	RemoveIndicator();
}

} // namespace gj
