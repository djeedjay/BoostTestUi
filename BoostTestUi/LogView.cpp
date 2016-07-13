// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#include "stdafx.h"
#include <iomanip>
#include <regex>
#include <fstream>
#include <array>
#include "Resource.h"
#include "Utilities.h"
#include "DevEnv.h"
#include "MainFrm.h"
#include "LogView.h"

namespace gj {

BEGIN_MSG_MAP2(CLogView)
	MSG_WM_CREATE(OnCreate)
	MSG_WM_CONTEXTMENU(OnContextMenu);
	REFLECTED_NOTIFY_CODE_HANDLER_EX(NM_CUSTOMDRAW, OnCustomDraw)
	REFLECTED_NOTIFY_CODE_HANDLER_EX(NM_DBLCLK, OnDblClick)
	REFLECTED_NOTIFY_CODE_HANDLER_EX(LVN_ITEMCHANGED, OnItemChanged)
	REFLECTED_NOTIFY_CODE_HANDLER_EX(LVN_GETINFOTIP, OnGetInfoTip)
	REFLECTED_NOTIFY_CODE_HANDLER_EX(LVN_GETDISPINFO, OnGetDispInfo)
	DEFAULT_REFLECTION_HANDLER()
	CHAIN_MSG_MAP(CDoubleBufferImpl<CLogView>)
END_MSG_MAP()

CLogView::CLogView(CMainFrame& mainFrame) :
	m_pMainFrame(&mainFrame),
	m_clockTime(false),
	m_logHighLightBegin(0),
	m_logHighLightEnd(0),
	m_insidePaint(false)
{
}

void CLogView::OnException()
{
	MessageBox(L"Unknown exception", LoadString(IDR_APPNAME).c_str(), MB_ICONERROR | MB_OK);
}

void CLogView::OnException(const std::exception& ex)
{
	MessageBox(WStr(ex.what()), LoadString(IDR_APPNAME).c_str(), MB_ICONERROR | MB_OK);
}

BOOL CLogView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

LRESULT CLogView::OnCreate(const CREATESTRUCT* /*pCreate*/)
{
	DefWindowProc();

	SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

	int timeWidth = 90;
	InsertColumn(0, L"Time", LVCFMT_LEFT, timeWidth, 0);
	InsertColumn(1, L"Test Log", LVCFMT_LEFT, 600, 0);

	return 0;
}

void CLogView::OnContextMenu(HWND /*hWnd*/, CPoint pt)
{
	if (pt == CPoint(-1, -1))
	{
		RECT rect;
		GetItemRect(GetNextItem(-1, LVNI_ALL | LVNI_FOCUSED), &rect, LVIR_LABEL);
		pt = CPoint(rect.left, rect.bottom - 1);
	}
	else
	{
		ScreenToClient(&pt);
	}

	UINT flags = 0;
	HitTest(pt, &flags);
	if ((flags & LVHT_ONITEM) == 0)
		return;

	CMenu menuContext;
	menuContext.LoadMenu(IDR_LIST_CONTEXTMENU);
	CMenuHandle menuPopup(menuContext.GetSubMenu(0));
	ClientToScreen(&pt);
	menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, *m_pMainFrame);
}

bool CLogView::Empty() const
{
	return GetItemCount() == 0;
}

void CLogView::Clear()
{
	DeleteAllItems();
	m_logLines.clear();
	m_items.clear();
	SetHighLight(0, 0);
}

void CLogView::InvalidateLine(int line)
{
	InvalidateLines(line, line + 1);
}

void CLogView::InvalidateLines(int begin, int end)
{
	end = std::min(GetItemCount(), end);
	if (begin >= end)
		return;

	RECT recBegin;
	GetItemRect(begin, &recBegin, LVIR_BOUNDS);

	RECT recEnd;
	GetItemRect(end - 1, &recEnd, LVIR_BOUNDS);

	recBegin.bottom = recEnd.bottom;
	InvalidateRect(&recBegin);
}

void CLogView::SetHighLight(unsigned id)
{
	auto it = m_items.find(id);
	if (it == m_items.end())
		return;

	int begin = it->second.beginLine;
	int end = it->second.endLine;
	SetHighLight(begin, end);

	EnsureVisible(end, true);
	EnsureVisible(begin, true);
//	SetSelectionMark(begin);
	SetItemState(begin, LVIS_FOCUSED, LVIS_FOCUSED);
}

void CLogView::SetHighLight(int begin, int end)
{
	InvalidateLines(m_logHighLightBegin, m_logHighLightEnd);
	m_logHighLightBegin = begin;
	m_logHighLightEnd = end;
	InvalidateLines(m_logHighLightBegin, m_logHighLightEnd);
}

bool CLogView::Find(const std::string& text, int direction)
{
	if (GetItemCount() == 0)
		return false;

	int begin = std::max(GetNextItem(-1, LVNI_FOCUSED), 0);
	int line = begin + direction;
	while (line != begin)
	{
		if (line < 0)
			line += m_logLines.size();
		if (static_cast<size_t>(line) == m_logLines.size())
			line = 0;

		if (m_logLines[line].message.find(text) != std::string::npos)
		{
			EnsureVisible(line, true);
			SetItemState(line, LVIS_FOCUSED, LVIS_FOCUSED);
			SelectItem(line);
			return true;
		}
		line += direction;
	}
	return false;
}

bool CLogView::FindNext(const std::wstring& text)
{
	return Find(Str(text).str(), +1);
}

bool CLogView::FindPrevious(const std::wstring& text)
{
	return Find(Str(text).str(), -1);
}

void CLogView::Add(unsigned id, const SYSTEMTIME& localTime, double t, Severity::type severity, const std::string& msg)
{
	int focus = GetNextItem(0, LVNI_FOCUSED);
	bool selectLast = focus < 0 || focus == GetItemCount() - 1;

	int item = m_logLines.size();
	m_logLines.push_back(LogLine(id, localTime, t, severity, msg));
	SetItemCount(m_logLines.size());

	if (selectLast)
	{
		EnsureVisible(item, true);
		SetItemState(item, LVIS_FOCUSED, LVIS_FOCUSED);
	}
}

bool CLogView::GetClockTime() const
{
	return m_clockTime;
}

void CLogView::SetClockTime(bool clockTime)
{
	m_clockTime = clockTime;
	Invalidate(false);
}

void CLogView::Save(const std::wstring& fileName)
{
	std::ofstream file(fileName);

	int lines = GetItemCount();
	for (int i = 0; i < lines; ++i)
		file << GetItemText(i, 0) << "\t" << GetItemText(i, 1) << "\n";

	file.close();
	if (!file)
		ThrowLastError(fileName);
}

void CLogView::LoadSettings(CRegKey& reg)
{
	std::array<wchar_t, 100> buf;
	DWORD len = buf.size();
	if (reg.QueryStringValue(L"ColWidths", buf.data(), &len) == ERROR_SUCCESS)
	{
		std::wistringstream ss(buf.data());
		int col = 0;
		int width;
		while (ss >> width)
		{
			SetColumnWidth(col, width);
			++col;
		}
	}
}

void CLogView::SaveSettings(CRegKey& reg)
{
	std::wostringstream ss;
	for (int i = 0; i < 2; ++i)
		ss << GetColumnWidth(i) << " ";
	reg.SetStringValue(L"ColWidths", ss.str().c_str());
}

void CLogView::BeginTestUnit(unsigned id)
{
	m_items[id].beginLine = GetItemCount();
	m_items[id].endLine = std::numeric_limits<int>::max();
}

void CLogView::EndTestUnit(unsigned id)
{
	m_items[id].endLine = GetItemCount();
	if (m_logHighLightEnd > GetItemCount())
		SetHighLight(m_items[id].beginLine, m_items[id].endLine);
}

void CLogView::SelectAll()
{
	int lines = GetItemCount();
	for (int i = 0; i < lines; ++i)
		SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
}

std::string CLogView::GetItemText(int item, int subItem) const
{
	CComBSTR bstr;
	GetItemText(item, subItem, bstr.m_str);
	return std::string(bstr.m_str, bstr.m_str + bstr.Length());
}

void CLogView::Copy()
{
	std::ostringstream ss;

	int item = -1;
	while ((item = GetNextItem(item, LVNI_ALL | LVNI_SELECTED)) >= 0)
		ss << GetItemText(item, 0) << "\t" << GetItemText(item, 1) << "\r\n";
	CopyToClipboard(ss.str(), *this);
}

int to_int(const std::string& s)
{
	std::stringstream ss;
	ss << s;
	int i = 0;
	ss >> i;
	return i;
}

COLORREF CLogView::GetHighLightBkColor(Severity::type sev, int item) const
{
	bool highLight = item >= m_logHighLightBegin && item < m_logHighLightEnd;

	switch (sev)
	{
	case Severity::Info: return highLight ? RGB(224, 224, 224) : GetSysColor(COLOR_WINDOW);
	case Severity::Error: return RGB(255, 188, 0);
	case Severity::Fatal: return RGB(255, 64, 64);
	case Severity::Assertion: return RGB(200, 0, 224);
	}
	return GetSysColor(COLOR_WINDOW);
}

RECT CLogView::GetSubItemRect(int iItem, int iSubItem, unsigned code) const
{
	int subitemCount = GetHeader().GetItemCount();

	RECT rect;
	CListViewCtrl::GetSubItemRect(iItem, iSubItem, code, &rect);
	if (iSubItem == 0 && subitemCount > 1)
		rect.right = GetSubItemRect(iItem, 1, code).left;
	return rect;
}

unsigned GetTextAlign(const HDITEM& item)
{
	switch (item.fmt & HDF_JUSTIFYMASK)
	{
	case HDF_LEFT: return DT_LEFT;
	case HDF_CENTER: return DT_CENTER;
	case HDF_RIGHT: return DT_RIGHT;
	}
	return HDF_LEFT;
}

class ScopedBkColor
{
public:
	ScopedBkColor(HDC hdc, COLORREF col) :
		m_hdc(hdc),
		m_col(SetBkColor(hdc, col))
	{
	}

	~ScopedBkColor()
	{
		SetBkColor(m_hdc, m_col);
	}

private:
	HDC m_hdc;
	COLORREF m_col;
};

class ScopedTextColor
{
public:
	ScopedTextColor(HDC hdc, COLORREF col) :
		m_hdc(hdc),
		m_col(SetTextColor(hdc, col))
	{
	}

	~ScopedTextColor()
	{
		SetTextColor(m_hdc, m_col);
	}

private:
	HDC m_hdc;
	COLORREF m_col;
};

void CLogView::DrawSubItem(CDCHandle dc, int iItem, int iSubItem) const
{
	auto text = GetSubItemText(iItem, iSubItem);
	RECT rect = GetSubItemRect(iItem, iSubItem, LVIR_BOUNDS);
	int margin = GetHeader().GetBitmapMargin();
	rect.left += margin;
	rect.right -= margin;

	HDITEM item;
	item.mask = HDI_FORMAT;
	unsigned align = (GetHeader().GetItem(iSubItem, &item)) ? GetTextAlign(item) : HDF_LEFT;
	::DrawTextA(dc, text.c_str(), text.size(), &rect, align | DT_NOCLIP | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
}

void CLogView::DrawItem(CDCHandle dc, int iItem, unsigned /*iItemState*/) const
{
	RECT rect;
	CListViewCtrl::GetItemRect(iItem, &rect, LVIR_BOUNDS);

	bool selected = GetItemState(iItem, LVIS_SELECTED) == LVIS_SELECTED;
	bool focused = GetItemState(iItem, LVIS_FOCUSED) == LVIS_FOCUSED;
	auto bkColor = selected ? GetSysColor(COLOR_HIGHLIGHT) : GetHighLightBkColor(m_logLines[iItem].severity, iItem);
	auto txColor = selected ? GetSysColor(COLOR_HIGHLIGHTTEXT) : GetSysColor(COLOR_WINDOWTEXT);
	dc.FillSolidRect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, bkColor);
	ScopedBkColor bcol(dc, bkColor);
	ScopedTextColor tcol(dc, txColor);

	int subitemCount = GetHeader().GetItemCount();
	for (int i = 0; i < subitemCount; ++i)
		DrawSubItem(dc, iItem, i);
	if (focused)
		dc.DrawFocusRect(&rect);
}

LRESULT CLogView::OnCustomDraw(NMHDR* pnmh)
{
	// See: http://stackoverflow.com/questions/938896/flickering-in-listview-with-ownerdraw-and-virtualmode
	if (!m_insidePaint)
		return CDRF_SKIPDEFAULT;

	auto& nmhdr = *reinterpret_cast<NMLVCUSTOMDRAW*>(pnmh);

	switch (nmhdr.nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		return CDRF_NOTIFYITEMDRAW;

	case CDDS_ITEMPREPAINT:
		DrawItem(nmhdr.nmcd.hdc, nmhdr.nmcd.dwItemSpec, nmhdr.nmcd.uItemState);
		return CDRF_SKIPDEFAULT;
	}

	return CDRF_DODEFAULT;
}

LRESULT CLogView::OnDblClick(NMHDR* pnmh)
{
	auto& nmhdr = *reinterpret_cast<NMITEMACTIVATE*>(pnmh);

	std::string line = GetItemText(nmhdr.iItem, 1);
	static const std::regex re1("(.+)\\((\\d+)\\): .*");
	static const std::regex re2("file (.+), line (\\d+)");
	static const std::regex re3(" in (.+):line (\\d+)");

	std::smatch sm;
	if (std::regex_match(line, sm, re1) ||
		std::regex_search(line, sm, re2) ||
		std::regex_search(line, sm, re3))
		m_pMainFrame->getDevEnv().ShowSourceLine(sm[1], to_int(sm[2]));

	return 0;
}

LRESULT CLogView::OnItemChanged(NMHDR* pnmh)
{
	auto& nmhdr = *reinterpret_cast<NMLISTVIEW*>(pnmh);

	if ((nmhdr.uNewState & LVIS_FOCUSED) == 0 ||
		nmhdr.iItem < 0  ||
		static_cast<size_t>(nmhdr.iItem) >= m_logLines.size())
		return 0;

	m_pMainFrame->SelectItem(m_logLines[nmhdr.iItem].id);

	return 0;
}

LRESULT CLogView::OnGetInfoTip(NMHDR* /*pnmh*/)
{
//	auto& nmhdr = *reinterpret_cast<NMLVGETINFOTIP*>(pnmh);

	return 0;
}

void CopyItemText(const std::string& s, wchar_t* buf, size_t maxLen)
{
	for (auto it = s.begin(); maxLen > 1 && it != s.end(); ++it, ++buf, --maxLen)
		*buf = *it;
	*buf = '\0';
}

std::string CLogView::GetTimeText(double t)
{
	return stringbuilder() << std::fixed << std::setprecision(6) << t;
}

std::string CLogView::GetTimeText(const SYSTEMTIME& t)
{
	char buf[32];
	sprintf(buf, "%d:%02d:%02d.%03d", t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
	return buf;
}

std::string CLogView::GetTimeText(const LogLine& log) const
{
	return m_clockTime ? GetTimeText(log.localTime) : GetTimeText(log.time);
}

std::string CLogView::GetSubItemText(int iItem, int iSubItem) const
{
	switch (iSubItem)
	{
	case 0: return GetTimeText(m_logLines[iItem]);
	case 1: return m_logLines[iItem].message;
	}
	return std::string();
}

LRESULT CLogView::OnGetDispInfo(NMHDR* pnmh)
{
	auto& nmhdr = *reinterpret_cast<NMLVDISPINFO*>(pnmh);
	LVITEM& item = nmhdr.item;
	if ((item.mask & LVIF_TEXT) == 0 || item.iItem >= static_cast<int>(m_logLines.size()))
		return 0;

	m_dispInfoText = WStr(GetSubItemText(item.iItem, item.iSubItem)).str();
	item.pszText = const_cast<wchar_t*>(m_dispInfoText.c_str());

	return 0;
}

void CLogView::DoPaint(CDCHandle dc)
{
	m_insidePaint = true;

	RECT rect;
	dc.GetClipBox(&rect);
	dc.FillSolidRect(&rect, GetSysColor(COLOR_WINDOW)); 
	DefWindowProc(WM_PAINT, reinterpret_cast<WPARAM>(dc.m_hDC), 0);

	m_insidePaint = false;
}

} // namespace gj
