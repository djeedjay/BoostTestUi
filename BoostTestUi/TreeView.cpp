// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#include "stdafx.h"
#include "Resource.h"
#include "Utilities.h"
#include "MainFrm.h"
#include "TreeView.h"

// ComCtrl.h
// Needs _WIN32_WINNT >= 0x0600 which breaks XP compatibility..
#define TVIS_EX_DISABLED        0x0002

namespace gj {

BEGIN_MSG_MAP2(CTreeView)
	MSG_WM_CREATE(OnCreate)
	MSG_WM_TIMER(OnTimer)
	MSG_WM_CONTEXTMENU(OnContextMenu)
	REFLECTED_NOTIFY_CODE_HANDLER_EX(TVN_SELCHANGED, OnSelChanged)
	REFLECTED_NOTIFY_CODE_HANDLER_EX(NM_CUSTOMDRAW, OnCustomDraw)
	REFLECTED_NOTIFY_CODE_HANDLER_EX(TVN_GETINFOTIP, OnGetInfoTip)
	REFLECTED_NOTIFY_CODE_HANDLER_EX(NM_CLICK, OnClick)
	REFLECTED_NOTIFY_CODE_HANDLER_EX(NM_RCLICK, OnRClick)
	CHAIN_MSG_MAP(CDoubleBufferImpl<CTreeView>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()

CTreeView::CTreeView(CMainFrame& mainFrame) :
	m_pMainFrame(&mainFrame),
	m_hCurrentItem(nullptr),
	m_runIndex(0)
{
}

void CTreeView::OnException()
{
	MessageBox(L"Unknown exception", LoadString(IDR_APPNAME).c_str(), MB_ICONERROR | MB_OK);
}

void CTreeView::OnException(const std::exception& ex)
{
	MessageBox(WStr(ex.what()), LoadString(IDR_APPNAME).c_str(), MB_ICONERROR | MB_OK);
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
	m_treeImg.Create(16, 16, ILC_COLOR24 | ILC_MASK, 17, 0);
	m_iEmpty = m_treeImg.AddIcon(AtlLoadIcon(IDI_EMPTY));
	m_iTick = m_treeImg.AddIcon(AtlLoadIcon(IDI_TICK));
	m_iTickG = m_treeImg.AddIcon(AtlLoadIcon(IDI_TICKG));
	m_iCross = m_treeImg.AddIcon(AtlLoadIcon(IDI_CROSS));
	m_iWarn = m_treeImg.AddIcon(AtlLoadIcon(IDI_WARN));
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

void CTreeView::CheckFailed()
{
	for (auto it = m_items.begin(); it != m_items.end(); ++it)
		CheckTreeItem(it->second, GetTestItemImage(it->second) == m_iCross);
}

LRESULT CTreeView::OnSelChanged(NMHDR* pnmh)
{
	NMTREEVIEW* pNmTreeView = reinterpret_cast<NMTREEVIEW*>(pnmh);
	if (pNmTreeView->action != TVC_UNKNOWN)
		m_pMainFrame->SetLogHighLight(GetItemData(pNmTreeView->itemNew.hItem));
	return 0;
}

LRESULT CTreeView::OnCustomDraw(NMHDR* pnmh)
{
	NMTVCUSTOMDRAW* pCustomDraw = reinterpret_cast<NMTVCUSTOMDRAW*>(pnmh);

	switch (pCustomDraw->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		return CDRF_NOTIFYITEMDRAW;

	case CDDS_ITEMPREPAINT:
	{
		HTREEITEM hItem = reinterpret_cast<HTREEITEM>(pCustomDraw->nmcd.dwItemSpec);
		if (!m_pMainFrame->IsActiveItem(GetItemData(hItem)))
		{
			pCustomDraw->clrText = GetSysColor(COLOR_GRAYTEXT);
			pCustomDraw->clrTextBk = GetSysColor(COLOR_3DLIGHT);
		}
		return CDRF_DODEFAULT;
	}
	}

	return CDRF_DODEFAULT;
}

LRESULT CTreeView::OnGetInfoTip(NMHDR* pnmh)
{
	NMTVGETINFOTIP* pNmGetInfoTip = reinterpret_cast<NMTVGETINFOTIP*>(pnmh);
	std::wstring tooltip = WStr(m_pMainFrame->GetTestItem(GetItemData(pNmGetInfoTip->hItem)).fullName).str();
	size_t maxSize = pNmGetInfoTip->cchTextMax;
	if (tooltip.size() + 1 > maxSize)
		tooltip = tooltip.substr(maxSize - 4) + L"...";
	assert(tooltip.size() + 1 <= maxSize);
	wcsncpy(pNmGetInfoTip->pszText, tooltip.c_str(), tooltip.size() + 1);
	return 0;
}

LRESULT CTreeView::OnClick(NMHDR* /*pnmh*/)
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

LRESULT CTreeView::OnRClick(NMHDR* /*pnmh*/)
{
	// Q222905
	SendMessage(WM_CONTEXTMENU, (WPARAM)m_hWnd, GetMessagePos());
	return 0;
}

void CTreeView::DoPaint(CDCHandle hdc)
{
	RECT rect;
	GetClientRect(&rect);
	hdc.FillSolidRect(&rect, GetSysColor(COLOR_WINDOW));

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

int CTreeView::GetTestItemImage(HTREEITEM hItem) const
{
	int image, selectedImage;
	GetItemImage(hItem, image, selectedImage);
	return image;
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
	CTreeViewCtrl::Expand(hItem, depth > 0 ? TVE_EXPAND : TVE_COLLAPSE);
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

void ExpandTreeViewItem(CTreeViewCtrl& treeView, HTREEITEM item, bool expand)
{
	treeView.Expand(item, expand ? TVE_EXPAND : TVE_COLLAPSE);
}

bool IsTreeViewItemChecked(const CTreeViewCtrl& treeView, HTREEITEM item)
{
	return treeView.GetCheckState(item) != FALSE;
}

bool IsTreeViewItemEnabled(const CTreeViewCtrl& treeView, HTREEITEM hItem)
{
	TVITEMEX item = { 0 };
	item.mask = TVIF_STATEEX;
	item.hItem = hItem;
	treeView.GetItem(&item);

	return (item.uStateEx & TVIS_EX_DISABLED) == 0;
}

bool IsTreeViewItemExpanded(const CTreeViewCtrl& treeView, HTREEITEM item)
{
	return treeView.GetItemState(item, TVIS_EXPANDED) == TVIS_EXPANDED;
}

TreeViewItemState CTreeView::GetTestItemState(unsigned id) const
{
	auto it = m_items.find(id);
	if (it == m_items.end())
		return TreeViewItemState(false, false);

	return TreeViewItemState(IsTreeViewItemChecked(*this, it->second), IsTreeViewItemExpanded(*this, it->second));
}

void CTreeView::SetTestItemState(unsigned id, const TreeViewItemState& state)
{
	auto it = m_items.find(id);
	if (it == m_items.end())
		return;

	ExpandTreeViewItem(*this, it->second, state.expand);
	SetCheckState(it->second, state.enable);
}

bool CTreeView::IsExpanded(unsigned id) const
{
	auto it = m_items.find(id);
	return it != m_items.end() && IsTreeViewItemExpanded(*this, it->second);
}

void CTreeView::Expand(unsigned id, bool expand)
{
	auto it = m_items.find(id);
	if (it != m_items.end())
		ExpandTreeViewItem(*this, it->second, expand);
}

bool CTreeView::IsChecked(unsigned id) const
{
	auto it = m_items.find(id);
	return
		it != m_items.end() &&
		m_pMainFrame->IsActiveItem(id) &&
		IsTreeViewItemEnabled(*this, it->second) &&
		IsTreeViewItemChecked(*this, it->second);
}

void CTreeView::Check(unsigned id, bool check)
{
	auto it = m_items.find(id);
	if (it != m_items.end())
		SetCheckState(it->second, check);
}

BOOL CTreeView::SetExtendedState(HTREEITEM hItem, UINT stateEx)
{
	TVITEMEX item = { 0 };

	item.mask = TVIF_STATEEX;
	item.hItem = hItem;
	item.uStateEx = stateEx;
	return SetItem(&item);
}

void CTreeView::EnableItem(unsigned /*id*/, bool /*enable*/)
{
//	auto it = m_items.find(id);
//	if (it != m_items.end())
//		SetExtendedState(it->second, enable ? 0 : TVIS_EX_DISABLED);
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

void CTreeView::SetTestIgnore(HTREEITEM hItem)
{
	SetItemImage(hItem, m_iWarn);
}

void CTreeView::SetTestFail(HTREEITEM hItem)
{
	SetItemImage(hItem, m_iCross);
}

void CTreeView::SetTestOk(HTREEITEM hItem)
{
	SetItemImage(hItem, m_iTick);
}

void CTreeView::EndTestCase(unsigned id, TestCaseState::type state)
{
	m_hCurrentItem = nullptr;

	auto it = m_items.find(id);
	if (it == m_items.end())
		return;

	switch (state)
	{
	case TestCaseState::Ignored:
		SetTestIgnore(it->second);
		break;

	case TestCaseState::Success:
		SetTestOk(it->second);
		break;

	case TestCaseState::Failed:
	default:
		SetTestFail(it->second);
		break;
	}
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
		if (suiteImg == m_iTick && img == m_iEmpty)
			suiteImg = m_iTickG;
		if (img == m_iWarn)
			suiteImg = m_iWarn;

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
