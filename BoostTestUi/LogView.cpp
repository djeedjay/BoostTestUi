//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the boost test library home page.

#include "stdafx.h"
#include <iomanip>
#include <regex>
#include <fstream>
#include <array>
#include "Resource.h"
#include "Utilities.h"
#include "ShowSourceLine.h"
#include "MainFrm.h"
#include "LogView.h"

namespace gj {

BEGIN_MSG_MAP_TRY(CLogView)
	MSG_WM_CREATE(OnCreate)
	MSG_WM_CONTEXTMENU(OnContextMenu);
	REFLECTED_NOTIFY_CODE_HANDLER(NM_CUSTOMDRAW, OnCustomDraw)
	REFLECTED_NOTIFY_CODE_HANDLER(NM_CLICK, OnClick)
	REFLECTED_NOTIFY_CODE_HANDLER(NM_DBLCLK, OnDblClick)
	DEFAULT_REFLECTION_HANDLER();
END_MSG_MAP_CATCH(ExceptionHandler)

CLogView::CLogView(CMainFrame& mainFrame) :
	m_pMainFrame(&mainFrame),
	m_logHighLightBegin(0),
	m_logHighLightEnd(0)
{
}

void CLogView::ExceptionHandler()
{
	MessageBox(WStr(GetExceptionMessage()), L"Boost Test", MB_ICONERROR | MB_OK);
}

BOOL CLogView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

LRESULT CLogView::OnCreate(const CREATESTRUCT* pCreate)
{
	DefWindowProc();

	SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

	int timeWidth = 90;
	InsertColumn(0, L"Time", LVCFMT_LEFT, timeWidth, 0);
	InsertColumn(1, L"Test Log", LVCFMT_LEFT, 600, 0);

	return 0;
}

void CLogView::OnContextMenu(HWND hWnd, CPoint pt)
{
	if (pt == CPoint(-1, -1))
	{
		RECT rect;
		GetItemRect(GetNextItem(-1, LVNI_ALL | LVIS_FOCUSED), &rect, LVIR_LABEL);
		pt = CPoint(rect.left, rect.bottom - 1);
	}
	else
	{
		ScreenToClient(&pt);
	}

	UINT flags = 0;
	int item = HitTest(pt, &flags);
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

void CLogView::InvalidateLines(int begin, int end)
{
	RECT recBegin;
	GetItemRect(begin, &recBegin, LVIR_BOUNDS);

	RECT recEnd;
	GetItemRect(end, &recEnd, LVIR_BOUNDS);

	recBegin.bottom = recEnd.bottom;
	InvalidateRect(&recBegin);
}

void CLogView::SetHighLight(unsigned id)
{
	auto it = m_items.find(id);
	if (it != m_items.end())
		SetHighLight(it->second.beginLine, it->second.endLine);
}

void CLogView::SetHighLight(int begin, int end)
{
	InvalidateLines(m_logHighLightBegin, m_logHighLightEnd);
	m_logHighLightBegin = begin;
	m_logHighLightEnd = end;
	InvalidateLines(m_logHighLightBegin, m_logHighLightEnd);
	SetSelectionMark(begin);
	EnsureVisible(end, true);
	EnsureVisible(begin, true);
}

void CLogView::Add(unsigned id, double t, Severity::type severity, const std::string& msg)
{
	auto it = msg.end();
	while (it > msg.begin())
	{
		if (*(it - 1) >= ' ')
			break;
		--it;
	}

	bool selectLast = GetSelectionMark() == GetItemCount() - 1;

	int item = InsertItem(
		std::numeric_limits<int>::max(),
		WStr(wstringbuilder() << std::fixed << std::setprecision(6) << t));

	SetItemText(item, 1, std::wstring(msg.begin(), it).c_str());
	m_logLines.push_back(LogLine(id, severity));

	if (selectLast)
	{
		EnsureVisible(item, true);
		SetSelectionMark(item);
	}
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
	const std::string& str = ss.str();

	HGLOBAL hdst = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, str.size() + 1);
	char* dst = static_cast<char*>(GlobalLock(hdst));
	std::copy(str.begin(), str.end(), dst);
	dst[str.size()] = '\0';
	GlobalUnlock(hdst);
	if (OpenClipboard())
	{
		EmptyClipboard();
		SetClipboardData(CF_TEXT, hdst);
		CloseClipboard();
	}
}

int to_int(const std::string& s)
{
	std::stringstream ss;
	ss << s;
	int i = 0;
	ss >> i;
	return i;
}

COLORREF GetHighLightBkColor(Severity::type sev, bool highLight)
{
	switch (sev)
	{
	case Severity::Info: return highLight? RGB(224, 224, 224): CLR_DEFAULT;
	case Severity::Error: return RGB(255, 188, 0);
	case Severity::Fatal: return RGB(255, 64, 64);
	case Severity::Assertion: return RGB(200, 0, 224);
	}
	return CLR_DEFAULT;
}

LRESULT CLogView::OnCustomDraw(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	NMLVCUSTOMDRAW* pCustomDraw = reinterpret_cast<NMLVCUSTOMDRAW*>(pnmh);

	int item = pCustomDraw->nmcd.dwItemSpec;
	switch (pCustomDraw->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		return CDRF_NOTIFYITEMDRAW;

	case CDDS_ITEMPREPAINT:
		return CDRF_NOTIFYSUBITEMDRAW;

	case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
		pCustomDraw->clrTextBk = GetHighLightBkColor(m_logLines[item].severity, item >= m_logHighLightBegin && item <= m_logHighLightEnd);
		return CDRF_DODEFAULT;
	}

	return CDRF_DODEFAULT;
}

LRESULT CLogView::OnClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	NMITEMACTIVATE* pItemActivate = reinterpret_cast<NMITEMACTIVATE*>(pnmh);

	if (pItemActivate->iItem < 0  || static_cast<size_t>(pItemActivate->iItem) >= m_logLines.size())
		return 0;

	m_pMainFrame->SelectItem(m_logLines[pItemActivate->iItem].id);
	return 0;
}

LRESULT CLogView::OnDblClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	NMITEMACTIVATE* pItemActivate = reinterpret_cast<NMITEMACTIVATE*>(pnmh);

	std::string line = GetItemText(pItemActivate->iItem, 1);
	static const std::regex re1("(.+)\\((\\d+)\\): .*");
	static const std::regex re2("file (.+), line (\\d+)");
	std::smatch sm;
	if (std::regex_match(line, sm, re1) || std::regex_search(line, sm, re2))
		ShowSourceLine(sm[1], to_int(sm[2]));

	return 0;
}

} // namespace gj
