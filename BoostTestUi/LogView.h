// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#pragma once

#include <vector>
#include <map>
#include "Severity.h"
#include "OffscreenPaint.h"

namespace gj {

typedef CWinTraitsOR<LVS_REPORT | LVS_OWNERDATA | LVS_NOSORTHEADER | LVS_SHOWSELALWAYS> CListViewTraits;

class CMainFrame;

class CLogView :
	public CWindowImpl<CLogView, CListViewCtrl, CListViewTraits>,
	public COffscreenPaint<CLogView>
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

	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID);
	void ExceptionHandler();
	BOOL PreTranslateMessage(MSG* pMsg);

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(const CREATESTRUCT* pCreate);
	void OnContextMenu(HWND hWnd, CPoint pt);
	LRESULT OnCustomDraw(LPNMHDR pnmh);
	LRESULT OnDblClick(LPNMHDR pnmh);
	LRESULT OnItemChanged(LPNMHDR pnmh);
	LRESULT OnGetInfoTip(LPNMHDR pnmh);
	LRESULT OnGetDispInfo(LPNMHDR pnmh);
	void DoPaint(CDCHandle dc, const RECT& rcClip);

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
};

} // namespace gj
