// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#pragma once

#include <vector>
#include <map>
#include "AtlWinExt.h"
#include "Severity.h"

namespace gj {

typedef CWinTraitsOR<LVS_REPORT | LVS_OWNERDATA | LVS_NOSORTHEADER | LVS_SHOWSELALWAYS> CListViewTraits;

class CMainFrame;

class CLogView :
	public CWindowImpl<CLogView, CListViewCtrl, CListViewTraits>,
	public CDoubleBufferImpl<CLogView>,
	public ExceptionHandler<CLogView, std::exception>
{
public:
	explicit CLogView(CMainFrame& mainFrame);

	void SelectAll();
	void Copy();
	bool Empty() const;
	void Clear();
	void Add(unsigned id, const SYSTEMTIME& localTime, double t, Severity::type severity, const std::string& msg);
	bool GetClockTime() const;
	void SetClockTime(bool clockTime);
	void SetHighLight(unsigned id);
	void SetHighLight(int begin, int end);
	bool FindNext(const std::wstring& text);
	bool FindPrevious(const std::wstring& text);
	void Save(const std::wstring& fileName);
	void LoadSettings(CRegKey& reg);
	void SaveSettings(CRegKey& reg);
	void BeginTestUnit(unsigned id);
	void EndTestUnit(unsigned id);

	using CListViewCtrl::GetItemText;
	std::string GetItemText(int item, int subItem) const;

	DECLARE_WND_SUPERCLASS(nullptr, CListViewCtrl::GetWndClassName())

	BOOL PreTranslateMessage(MSG* pMsg);
	void DoPaint(CDCHandle dc);

private:
	struct LogLine
	{
		LogLine(unsigned id, const SYSTEMTIME& localTime, double time, Severity::type severity, const std::string& message) :
			id(id), localTime(localTime), time(time), severity(severity), message(message)
		{
		}

		unsigned id;
		SYSTEMTIME localTime;
		double time;
		Severity::type severity;
		std::string message;
	};

	struct TestItem
	{
		int beginLine;
		int endLine;
	};

	DECLARE_MSG_MAP()
	void OnException();
	void OnException(const std::exception& ex);

	LRESULT OnCreate(const CREATESTRUCT* pCreate);
	void OnContextMenu(HWND hWnd, CPoint pt);
	LRESULT OnCustomDraw(LPNMHDR pnmh);
	LRESULT OnDblClick(LPNMHDR pnmh);
	LRESULT OnItemChanged(LPNMHDR pnmh);
	LRESULT OnGetInfoTip(LPNMHDR pnmh);
	LRESULT OnGetDispInfo(LPNMHDR pnmh);
	void DrawSubItem(CDCHandle dc, int iItem, int iSubItem) const;
	void DrawItem(CDCHandle dc, int iItem, unsigned iItemState) const;
	RECT GetSubItemRect(int iItem, int iSubItem, unsigned code) const;
	std::string GetSubItemText(int iItem, int iSubItem) const;

	bool Find(const std::string& text, int direction);
	COLORREF GetHighLightBkColor(Severity::type sev, int item) const;
	void InvalidateLine(int line);
	void InvalidateLines(int begin, int end);
	static std::string GetTimeText(double t);
	static std::string GetTimeText(const SYSTEMTIME& t);
	std::string GetTimeText(const LogLine& log) const;

	CMainFrame* m_pMainFrame;
	std::vector<LogLine> m_logLines;
	std::map<unsigned, TestItem> m_items;
	bool m_clockTime;
	int m_logHighLightBegin;
	int m_logHighLightEnd;
	bool m_insidePaint;
	std::wstring m_dispInfoText;
};

} // namespace gj
