//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the boost test library home page.

#pragma once

#include <vector>
#include <map>
#include "Severity.h"

namespace gj {

typedef CWinTraitsOR<LVS_REPORT | LVS_NOSORTHEADER | LVS_SHOWSELALWAYS> CListViewTraits;

class CMainFrame;

class CLogView : public CWindowImpl<CLogView, CListViewCtrl, CListViewTraits>
{
public:
	explicit CLogView(CMainFrame& mainFrame);

	void SelectAll();
	void Copy();
	bool Empty() const;
	void Clear();
	void Add(unsigned id, double t, Severity::type severity, const std::string& msg);
	void SetHighLight(unsigned id);
	void SetHighLight(int begin, int end);
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
	LRESULT OnCustomDraw(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);
	LRESULT OnDblClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);
	LRESULT OnItemChanging(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);
	LRESULT OnGetInfoTip(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);

private:
	struct LogLine
	{
		LogLine(unsigned id, Severity::type severity = Severity::Info) :
			id(id), severity(severity)
		{
		}

		unsigned id;
		Severity::type severity;
	};

	struct TestItem
	{
		int beginLine;
		int endLine;
	};

	void InvalidateLines(int begin, int end);

	CMainFrame* m_pMainFrame;
	std::vector<LogLine> m_logLines;
	std::map<unsigned, TestItem> m_items;
	int m_logHighLightBegin;
	int m_logHighLightEnd;
};

} // namespace gj
