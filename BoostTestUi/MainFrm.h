// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#ifndef BOOST_TESTUI_MAINFRM_H
#define BOOST_TESTUI_MAINFRM_H

#pragma once

#pragma warning(disable: 4481) // nonstandard extension used: override specifier 'override'

#include <string>
#include <vector>
#include <queue>
#include <memory>
#pragma warning(push, 3) // conversion from 'int' to 'unsigned short', possible loss of data
#include <boost/thread.hpp>
#pragma warning(pop)
#include "AtlWinExt.h"
#include "Utilities.h"
#include "TreeView.h"
#include "LogView.h"
#include "FindDlg.h"
#include "CategoryList.h"
#include "ExeRunner.h"
#include "DevEnv.h"

namespace gj {

class DevEnv;

class TreeViewStateStorage
{
public:
	void Clear();
	void SaveScrollPos(const CWindow& treeView);
	void SaveState(const TestUnit& tu, const TreeViewItemState& state);
	bool RestoreState(TestUnit& tu, TreeViewItemState& state) const;
	void RestoreScrollPos(CWindow& treeView);

private:
	std::map<std::string, TreeViewItemState> m_tests;
	int m_scrollPos;
};

class CMainFrame :
	public CFrameWindowImpl<CMainFrame>,
	public CUpdateUI<CMainFrame>,
	public CMessageFilter,
	public CIdleHandler,
	public ExceptionHandler<CMainFrame, std::exception>,
	public TestObserver
{
public:
	CMainFrame(const std::wstring& fileName, const std::wstring& arguments);

	DECLARE_FRAME_WND_CLASS(nullptr, IDR_MAINFRAME)

	void SetLogHighLight(unsigned id);
	void FindNext(const std::wstring& text);
	void FindPrevious(const std::wstring& text);

	void AddLogMessage(const SYSTEMTIME& localTime, double t, Severity::type severity, const std::string& msg);
	void SelectItem(unsigned id);
	bool IsActiveItem(unsigned id) const;
	TestUnit GetTestItem(unsigned id) const;

	void EnQueue(const std::function<void ()>& fn);

	DevEnv& getDevEnv();

	BEGIN_UPDATE_UI_MAP(CMainFrame)
	    UPDATE_ELEMENT(ID_FILE_AUTO_RUN, UPDUI_MENUPOPUP)
	    UPDATE_ELEMENT(ID_LOG_AUTO_CLEAR, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
	    UPDATE_ELEMENT(ID_LOG_TIME, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
	    UPDATE_ELEMENT(ID_TEST_RANDOMIZE, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
	    UPDATE_ELEMENT(ID_TEST_REPEAT, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
	    UPDATE_ELEMENT(ID_TEST_DEBUGGER, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_TEST_RUNNERARGS, UPDUI_MENUPOPUP)
	    UPDATE_ELEMENT(ID_TREE_RUN, UPDUI_MENUPOPUP)
	    UPDATE_ELEMENT(ID_TREE_RUN_CHECKED, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
	    UPDATE_ELEMENT(ID_TREE_RUN_ALL, UPDUI_MENUPOPUP)
	    UPDATE_ELEMENT(ID_TEST_ABORT, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_TEST_CATEGORIES, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_LOGLEVEL, UPDUI_TOOLBAR)
		UPDATE_ELEMENT(0, UPDUI_STATUSBAR)
		UPDATE_ELEMENT(1, UPDUI_STATUSBAR)
		UPDATE_ELEMENT(2, UPDUI_STATUSBAR)
		UPDATE_ELEMENT(3, UPDUI_STATUSBAR)
		UPDATE_ELEMENT(4, UPDUI_STATUSBAR)
	END_UPDATE_UI_MAP()

	enum { UM_DEQUEUE = WM_APP + 100 };

private:
	DECLARE_MSG_MAP()

	void OnException();
	void OnException(const std::exception& ex);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	virtual void test_message(Severity::type, const std::string& msg) override;

	virtual void test_waiting(const std::wstring& processName, unsigned processId) override;
	virtual void test_start() override;
	virtual void test_finish() override;
	virtual void test_aborted() override;
	virtual void test_iteration_start(unsigned test_cases_amount) override;
	virtual void test_iteration_finish() override;
	virtual void test_suite_start(unsigned id) override;
	virtual void test_case_start(unsigned id) override;
	virtual void test_case_finish(unsigned id, unsigned long elapsed) override;
	virtual void test_case_finish(unsigned id, unsigned long /*elapsed*/, TestCaseState::type state) override;
	virtual void test_suite_finish(unsigned id, unsigned long elapsed) override;
	virtual void test_unit_skipped(unsigned id) override;
	virtual void test_unit_aborted(unsigned id) override;

	virtual void test_unit_ignored(const std::string& msg) override;
	virtual void assertion_result(bool passed) override;
	virtual void exception_caught(const std::string& what) override;

	virtual void TestStarted() override;
	virtual void TestFinished() override;

	LRESULT OnCreate(const CREATESTRUCT* pCreate);
	LRESULT OnDeQueue(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	void OnTimer(UINT_PTR nIDEvent);
	void OnDropFiles(HDROP hDropInfo);
	void OnHelp(LPHELPINFO lpHelpInfo);
	void OnFileExit(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnFileOpen(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnFileSave(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnFileSaveAs(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnFileAutoRun(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnFileCreateBoostHpp(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnFileCreateCatchHpp(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnFileCreateGoogleHpp(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnLogAutoClear(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnResetSelection(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnLogSelectAll(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnLogClear(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnLogTime(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnLogCopy(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnLogFind(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnTestRandomize(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnTestRepeat(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnTestDebugger(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnTestRunnerArgs(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnTestAbort(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnTestCategories(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnHelpBoost(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnHelpCatch(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnHelpGoogle(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnHelpNUnit(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnAppAbout(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnTreeRun(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnTreeRunChecked(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnTreeRunAll(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnTreeCheckAll(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnTreeUncheckAll(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnTreeCheckFailed(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnTreeCopyName(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnTreeCopyCommand(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnMruMenuItem(UINT uCode, int nID, HWND hwndCtrl);
	void OnClose();

	void UpdateUI();
	void UpdateStatusBar();
	void UpdateProgressBar();
	void ClearTestSelection();
	void SaveTestSelection();
	void RestoreTestSelection();
	void LoadNew(const std::wstring& fileName);
	void Reload();
	void Load(const std::wstring& fileName);
	void CreateHpp(int resourceId, const std::wstring& fileName);
	std::wstring GetLogFileName(const std::wstring& fileName) const;
	void SaveLogFile(const std::wstring& fileName);
	unsigned GetOptions() const;
	bool LoadSettings();
	void SaveSettings();
	bool IsRunnable() const;
	void RunSingle(unsigned id);
	void RunChecked();
	void RunAll();
	void Run();
	void EndTestCase(unsigned id, unsigned long /*elapsed*/, TestCaseState::type state);
	int SelectDevEnv(const std::vector<std::wstring>& names, int indexHint);

	std::wstring m_pathName;
	std::wstring m_arguments;
	std::wstring m_logFileName;
	FILETIME m_fileTime;

	CCommandBarCtrl m_CmdBar;
	CComboBox m_combo;
	CMultiPaneStatusBarCtrl m_statusBar;
	CSplitterWindow m_vSplit;
	CTreeView m_treeView;
	CHorSplitterWindow m_hSplit;
	CProgressBarCtrl m_progressBar;
	CLogView m_logView;
	CRecentDocumentList m_mru;
	CFindDlg m_findDlg;
	std::unique_ptr<TestRunner> m_pRunner;
	CategoryList m_categories;
	UnitTestType::type m_helpType;
	TreeViewStateStorage m_testStateStorage;
	unsigned m_currentId;
	bool m_autoRun;
	bool m_logAutoClear;
	bool m_randomize;
	bool m_repeat;
	bool m_debugger;
	bool m_resetTimer;
	Timer m_timer;
	int m_testIterationCount;
	int m_testCaseCount;
	int m_testsRunCount;
	int m_ignoredTestCount;
	int m_failedTestCount;
	TestCaseState::type m_testCaseState;
	DevEnv m_devEnv;

	boost::mutex m_mtx;
	std::queue<std::function<void ()>> m_q;
};

} // namespace gj

#endif // BOOST_TESTUI_MAINFRM_H
