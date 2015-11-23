// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#include "stdafx.h"

#include <stdexcept>
#include <limits>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <array>
#include <boost/filesystem.hpp>
#include "Utilities.h"
#include "CategoryDlg.h"
#include "BoostHelpDlg.h"
#include "GoogleHelpDlg.h"
#include "NUnitHelpDlg.h"
#include "AboutDlg.h"
#include "ArgumentsDlg.h"
#include "ExeRunner.h"
#include "MainFrm.h"

// ComCtrl.h
// Needs _WIN32_WINNT >= 0x0600 which breaks XP compatibility..
#define PBM_SETSTATE            (WM_USER + 16) // wParam = PBST_[State] (NORMAL, ERROR, PAUSED)
#define PBST_NORMAL             0x0001
#define PBST_ERROR              0x0002
#define PBST_PAUSED             0x0003

namespace gj {

const COLORREF FailColor = RGB(255, 64, 64);
const COLORREF WarnColor = RGB(255, 224, 0);
const COLORREF SuccessColor = RGB(20, 180, 20);

struct Options
{
	enum type
	{
		LogAutoClear = 1 << 16,
		AutoRun = 1 << 17,
		ClockTime = 1 << 18,
	};
};

BEGIN_MSG_MAP2(CMainFrame)
	MSG_WM_CREATE(OnCreate)
	MSG_WM_CLOSE(OnClose)
	MESSAGE_HANDLER_EX(UM_DEQUEUE, OnDeQueue);
	MSG_WM_TIMER(OnTimer)
	MSG_WM_DROPFILES(OnDropFiles)
	MSG_WM_HELP(OnHelp)
	COMMAND_ID_HANDLER_EX(ID_APP_EXIT, OnFileExit)
	COMMAND_ID_HANDLER_EX(ID_FILE_OPEN, OnFileOpen)
	COMMAND_ID_HANDLER_EX(ID_FILE_SAVE, OnFileSave)
	COMMAND_ID_HANDLER_EX(ID_FILE_SAVE_AS, OnFileSaveAs)
	COMMAND_ID_HANDLER_EX(ID_FILE_AUTO_RUN, OnFileAutoRun)
	COMMAND_ID_HANDLER_EX(ID_FILE_CREATE_BOOST_HPP, OnFileCreateBoostHpp)
	COMMAND_ID_HANDLER_EX(ID_FILE_CREATE_GOOGLE_HPP, OnFileCreateGoogleHpp)
	COMMAND_ID_HANDLER_EX(ID_LOG_AUTO_CLEAR, OnLogAutoClear)
	COMMAND_ID_HANDLER_EX(ID_RESET_SELECTION, OnResetSelection)
	COMMAND_ID_HANDLER_EX(ID_LOG_SELECTALL, OnLogSelectAll)
	COMMAND_ID_HANDLER_EX(ID_LOG_CLEAR, OnLogClear)
	COMMAND_ID_HANDLER_EX(ID_LOG_TIME, OnLogTime)
	COMMAND_ID_HANDLER_EX(ID_LOG_COPY, OnLogCopy)
	COMMAND_ID_HANDLER_EX(ID_LOG_FIND, OnLogFind)
	COMMAND_ID_HANDLER_EX(ID_TEST_RANDOMIZE, OnTestRandomize)
	COMMAND_ID_HANDLER_EX(ID_TEST_REPEAT, OnTestRepeat)
	COMMAND_ID_HANDLER_EX(ID_TEST_DEBUGGER, OnTestDebugger)
	COMMAND_ID_HANDLER_EX(ID_TEST_RUNNERARGS, OnTestRunnerArgs)
	COMMAND_ID_HANDLER_EX(ID_TEST_ABORT, OnTestAbort)
	COMMAND_ID_HANDLER_EX(ID_TEST_CATEGORIES, OnTestCategories)
	COMMAND_ID_HANDLER_EX(ID_HELP_BOOST, OnHelpBoost)
	COMMAND_ID_HANDLER_EX(ID_HELP_GOOGLE, OnHelpGoogle)
	COMMAND_ID_HANDLER_EX(ID_HELP_NUNIT, OnHelpNUnit)
	COMMAND_ID_HANDLER_EX(ID_APP_ABOUT, OnAppAbout)
	COMMAND_ID_HANDLER_EX(ID_TREE_RUN, OnTreeRun)
	COMMAND_ID_HANDLER_EX(ID_TREE_RUN_CHECKED, OnTreeRunChecked)
	COMMAND_ID_HANDLER_EX(ID_TREE_RUN_ALL, OnTreeRunAll)
	COMMAND_ID_HANDLER_EX(ID_TREE_CHECK_ALL, OnTreeCheckAll)
	COMMAND_ID_HANDLER_EX(ID_TREE_UNCHECK_ALL, OnTreeUncheckAll)
	COMMAND_ID_HANDLER_EX(ID_TREE_CHECK_FAILED, OnTreeCheckFailed)
	COMMAND_ID_HANDLER_EX(ID_TREE_COPY_NAME, OnTreeCopyName)
	COMMAND_RANGE_HANDLER_EX(ID_FILE_MRU_FIRST, ID_FILE_MRU_LAST, OnMruMenuItem)
	CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
	CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
	REFLECT_NOTIFICATIONS()
END_MSG_MAP()

CMainFrame::CMainFrame(const std::wstring& fileName, const std::wstring& arguments) :
	m_pathName(fileName),
	m_arguments(arguments),
	m_treeView(*this),
	m_logView(*this),
	m_findDlg(*this),
	m_autoRun(false),
	m_logAutoClear(true),
	m_randomize(false),
	m_repeat(false),
	m_debugger(false)
{
}

void CMainFrame::OnException()
{
	MessageBox(L"Unknown exception", LoadString(IDR_APPNAME).c_str(), MB_ICONERROR | MB_OK);
}

void CMainFrame::OnException(const std::exception& ex)
{
	MessageBox(WStr(ex.what()), LoadString(IDR_APPNAME).c_str(), MB_ICONERROR | MB_OK);
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if (GetActiveWindow() == m_findDlg)
	{
		if (m_findDlg.IsDialogMessage(pMsg))
			return TRUE;
	}

	return CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnIdle()
{
	UpdateUI();
	UIUpdateToolBar();
	UIUpdateStatusBar();
	return FALSE;
}

void CMainFrame::UpdateUI()
{
	bool isLoaded = m_pRunner.get() != nullptr;
	bool isRunning = isLoaded && m_pRunner->IsRunning();
	bool isRunnable = IsRunnable();
	unsigned enabled = isLoaded ? m_pRunner->GetEnabledOptions(GetOptions()) : 0;
	UIEnable(ID_FILE_SAVE, isLoaded);
	UIEnable(ID_FILE_SAVE_AS, isLoaded);
	UIEnable(ID_TEST_RANDOMIZE, (enabled & TestRunner::Randomize) != 0);
	UIEnable(ID_TEST_REPEAT, isLoaded);
	UIEnable(ID_TEST_DEBUGGER, (enabled & TestRunner::WaitForDebugger) != 0);
	UIEnable(ID_TEST_RUNNERARGS, isLoaded);
	UIEnable(ID_TREE_RUN, isRunnable);
	UIEnable(ID_TREE_RUN_CHECKED, isRunnable);
	UIEnable(ID_TREE_RUN_ALL, isRunnable);
	UIEnable(ID_TEST_ABORT, isRunning);
	UIEnable(ID_TEST_CATEGORIES, !m_categories.IsEmpty());
	UIEnable(ID_LOGLEVEL, !isRunning);
	UISetCheck(ID_FILE_AUTO_RUN, m_autoRun);
	UISetCheck(ID_LOG_AUTO_CLEAR, m_logAutoClear);
	UISetCheck(ID_LOG_TIME, m_logView.GetClockTime());
	UISetCheck(ID_TEST_RANDOMIZE, m_randomize);
	UISetCheck(ID_TEST_REPEAT, m_repeat);
	UISetCheck(ID_TEST_DEBUGGER, m_debugger);
	UpdateStatusBar();
}

LRESULT CMainFrame::OnCreate(const CREATESTRUCT* /*pCreate*/)
{
	HWND hWndCmdBar = m_CmdBar.Create(*this, rcDefault, nullptr, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	m_CmdBar.AttachMenu(GetMenu());
	m_CmdBar.LoadImages(IDR_MAINFRAME);
	SetMenu(nullptr);

	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	CReBarCtrl rebar(m_hWndToolBar);
	AddSimpleReBarBand(hWndCmdBar);

	HWND hWndToolBar = CreateSimpleToolBarCtrl(rebar, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
	AddSimpleReBarBand(hWndToolBar, nullptr, true);
	UIAddToolBar(hWndToolBar);

	m_combo.Create(m_hWnd, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBS_HASSTRINGS | CBS_DROPDOWNLIST | CBS_AUTOHSCROLL, 0, ID_LOGLEVEL);
	m_combo.SetFont(AtlGetDefaultGuiFont());
	m_combo.AddString(L"Minimal");
	m_combo.AddString(L"Medium");
	m_combo.AddString(L"All");
	m_combo.SetCurSel(2);
	AddSimpleReBarBand(m_combo, L"Log Level: ", false, 60);
	SizeSimpleReBarBands();

	m_findDlg.Create(rebar);
	AddSimpleReBarBand(m_findDlg, L"Find: ", false, 10000);
	SizeSimpleReBarBands();

	rebar.LockBands(true);
	rebar.SetNotifyWnd(*this);

	m_hWndStatusBar = m_statusBar.Create(*this);
	UIAddStatusBar(m_hWndStatusBar);

	int paneIds[] = { ID_DEFAULT_PANE, ID_ITERATIONS_PANE, ID_TOTAL_PANE, ID_RUN_PANE, ID_FAILED_PANE };
	m_statusBar.SetPanes(paneIds, 5, false);

	// client rect for vertical splitter
	WTL::CRect rcVert;
	GetClientRect(&rcVert);

	m_hWndClient = m_vSplit.Create(*this, rcVert, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	m_vSplit.SetSplitterExtendedStyle(0);
	m_vSplit.m_cxyMin = 35; // minimum size
	m_vSplit.SetSplitterPos(244); // from left

	m_treeView.Create(m_vSplit.m_hWnd, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);

	CRect rcHor;
	GetClientRect(&rcHor);
	m_hSplit.Create(m_vSplit.m_hWnd, rcHor, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	m_hSplit.SetSplitterExtendedStyle(SPLIT_NONINTERACTIVE); 
	m_hSplit.SetSplitterPos(20);
	m_vSplit.SetSplitterPane(1, m_hSplit);

	m_progressBar.Create(m_hSplit.m_hWnd, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, IDC_PROGRESSBAR);
	m_logView.Create(m_hSplit.m_hWnd, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);

	m_hSplit.SetSplitterPanes(m_progressBar, m_logView);
	m_vSplit.SetSplitterPanes(m_treeView, m_hSplit);

	m_mru.SetMaxEntries(10);
	m_mru.SetMenuHandle(m_CmdBar.GetMenu().GetSubMenu(0).GetSubMenu(7));

	LoadSettings();

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != nullptr);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	if (!m_pathName.empty())
		Load(m_pathName);

	SetTimer(1, 1000);
	DragAcceptFiles(true);
	return 0;
}

void CMainFrame::OnFileExit(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	PostMessage(WM_CLOSE);
}

class CountTestCases : public TestTreeVisitor
{
public:
	explicit CountTestCases() : m_count(0)
	{
	}

	virtual void VisitTestCase(TestCase& /*tc*/) override
	{
		++m_count;
	}

	int count() const
	{
		return m_count;
	}

private:
	int m_count;
};

class CountEnabledTestCases : public TestTreeVisitor
{
public:
	explicit CountEnabledTestCases() : m_count(0)
	{
	}

	virtual void VisitTestCase(TestCase& tc) override
	{
		if (tc.enabled)
			++m_count;
	}

	int count() const
	{
		return m_count;
	}

private:
	int m_count;
};

class TestCaseLoader :
	boost::noncopyable,
	public TestTreeVisitor
{
public:
	TestCaseLoader(CTreeView& treeView, CategoryList& categories) :
		m_treeView(treeView),
		m_categories(categories),
		m_testCaseCount(0)
	{
	}

	virtual void VisitTestCase(TestCase& tc) override
	{
		GetCategories(tc);
		m_treeView.AddTestCase(tc.id, tc.name, tc.enabled);
		++m_testCaseCount;
	}

	virtual void EnterTestSuite(TestSuite& ts) override
	{
		GetCategories(ts);
		m_treeView.EnterTestSuite(ts.id, ts.name, ts.enabled);
	}

	virtual void LeaveTestSuite() override
	{
		m_treeView.LeaveTestSuite();
	}

	int TestCaseCount() const
	{
		return m_testCaseCount;
	}

	void GetCategories(const TestUnit& tu) const
	{
		for (auto it = tu.categories.begin(); it != tu.categories.end(); ++it)
			m_categories.Add(*it);
	}

private:
	CTreeView& m_treeView;
	CategoryList& m_categories;
	int m_testCaseCount;
};

class CategoryFilter :
	boost::noncopyable,
	public TestTreeVisitor
{
public:
	CategoryFilter(CTreeView& treeView, CategoryList& categories) :
		m_treeView(treeView),
		m_categories(categories)
	{
	}

	virtual void VisitTestCase(TestCase& tc) override
	{
		tc.active = Match(tc.categories);
		m_treeView.EnableItem(tc.id, tc.active);
	}

	virtual void EnterTestSuite(TestSuite& ts) override
	{
		ts.active = Match(ts.categories);
		m_treeView.EnableItem(ts.id, ts.active);
		m_suites.push_back(ts.active);
	}

	virtual void LeaveTestSuite() override
	{
		m_suites.pop_back();
	}

private:
	bool Match(const std::vector<std::string>& categories)
	{
		if (m_suites.empty())
			return true;
		if (!m_suites.back())
			return false;
		if (categories.empty())
			return m_categories.IsDefaultSelected();

		for (auto it = categories.begin(); it != categories.end(); ++it)
			if (m_categories.IsSelected(*it))
				return true;

		return false;
	}

	CTreeView& m_treeView;
	CategoryList& m_categories;
	std::vector<bool> m_suites;
};

class TestCaseStateSaveVisitor :
	boost::noncopyable,
	public TestTreeVisitor
{
public:
	TestCaseStateSaveVisitor(TreeViewStateStorage& testState, const CTreeView& treeView) :
		m_testState(testState),
		m_treeView(treeView)
	{
	}

	void VisitTestCase(TestCase& tc)
	{
		m_testState.SaveState(tc, m_treeView.GetTestItemState(tc.id));
	}

	void EnterTestSuite(TestSuite& ts)
	{
		m_testState.SaveState(ts, m_treeView.GetTestItemState(ts.id));
	}

private:
	TreeViewStateStorage& m_testState;
	const CTreeView& m_treeView;
};

class TestCaseStateRestoreVisitor :
	boost::noncopyable,
	public TestTreeVisitor
{
public:
	TestCaseStateRestoreVisitor(const TreeViewStateStorage& testState, CTreeView& treeView) :
		m_testState(testState),
		m_treeView(treeView)
	{
	}

	virtual void VisitTestCase(TestCase& tc)
	{
		EnterTestUnit(tc);
	}

	virtual void EnterTestSuite(TestSuite& ts)
	{
		EnterTestUnit(ts);
	}

	void EnterTestUnit(TestUnit& tu)
	{
		TreeViewItemState state(false, false);
		if (m_testState.RestoreState(tu, state))
			m_treeView.SetTestItemState(tu.id, state);
	}

private:
	const TreeViewStateStorage& m_testState;
	CTreeView& m_treeView;
};

void CMainFrame::EnQueue(const std::function<void ()>& fn)
{
	boost::mutex::scoped_lock lock(m_mtx);
	bool notify = m_q.empty();
	m_q.push(fn);
	if (notify)
		PostMessage(UM_DEQUEUE);
}

LRESULT CMainFrame::OnDeQueue(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	std::queue<std::function<void ()>> fnq;
	{
		boost::mutex::scoped_lock lock(m_mtx);
		std::swap(m_q, fnq);
	}
	while (!fnq.empty())
	{
		fnq.front()();
		fnq.pop();
	}
	return 0;
}

FILETIME GetLastWriteTime(HANDLE hFile, const std::wstring& fileName)
{
	FILETIME ftCreate, ftAccess, ftWrite;
	if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
		ThrowLastError(fileName);

	return ftWrite;
}

FILETIME GetLastWriteTime(const std::wstring& fileName)
{
	CHandle hFile(CreateFile(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr));
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hFile.Detach();
		ThrowLastError(fileName);
	}

	return GetLastWriteTime(hFile, fileName);
}

bool operator==(const FILETIME& ft1, const FILETIME& ft2)
{
	return
		ft1.dwLowDateTime == ft2.dwLowDateTime &&
		ft1.dwHighDateTime == ft2.dwHighDateTime;
}

bool operator!=(const FILETIME& ft1, const FILETIME& ft2)
{
	return !(ft1 == ft2);
}

void CMainFrame::ClearTestSelection()
{
	m_testStateStorage.Clear();
}

void CMainFrame::SaveTestSelection()
{
	if (!m_pRunner)
		return;

	ClearTestSelection();
	TestCaseStateSaveVisitor vis(m_testStateStorage, m_treeView);
	m_pRunner->TraverseTestTree(vis);
	m_testStateStorage.SaveScrollPos(m_treeView);
}

void CMainFrame::RestoreTestSelection()
{
	if (!m_pRunner)
		return;

	TestCaseStateRestoreVisitor vis(m_testStateStorage, m_treeView);
	m_pRunner->TraverseTestTree(vis);
	m_testStateStorage.RestoreScrollPos(m_treeView);
}

void CMainFrame::LoadNew(const std::wstring& fileName)
{
	m_progressBar.SetPos(0);
	m_logView.Clear();
	Load(fileName);
	m_logFileName = L"";
}

void CMainFrame::Reload()
{
	Load(m_pathName);
}

void CMainFrame::OnTimer(UINT_PTR /*nIDEvent*/)
{
	UpdateProgressBar();

	if (!m_pRunner || m_pRunner->IsRunning())
		return;

	CHandle hFile(CreateFile(m_pathName.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr));
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hFile.Detach();
		return;
	}

	FILETIME fileTime = GetLastWriteTime(hFile, m_pathName);
	if (fileTime == m_fileTime)
		return;

	SaveTestSelection();
	Reload();
	if (m_autoRun)
		RunChecked();
}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	auto guard = make_guard([hDropInfo]() { DragFinish(hDropInfo); });

	if (DragQueryFile(hDropInfo, 0xFFFFFFFF, nullptr, 0) == 1)
	{
		std::vector<wchar_t> fileName(DragQueryFile(hDropInfo, 0, nullptr, 0) + 1);
		if (DragQueryFile(hDropInfo, 0, fileName.data(), fileName.size()))
			LoadNew(fileName.data());
	}
}

void CMainFrame::OnHelp(LPHELPINFO /*lpHelpInfo*/)
{
	switch (m_helpType)
	{
	case UnitTestType::Boost: return OnHelpBoost(0, 0, *this);
	case UnitTestType::Google: return OnHelpGoogle(0, 0, *this); 
	case UnitTestType::NUnit: return OnHelpNUnit(0, 0, *this); 
	}
}

class TestFile
{
public:
	explicit TestFile(const std::wstring& fileName);

private:
	std::unique_ptr<TestRunner> m_pRunner;
	FILETIME m_fileTime;
};

void CMainFrame::Load(const std::wstring& fileName)
try
{
	// Store the time stamp of this file so that we don't retry Load()-ing it until it changes:
	m_fileTime = GetLastWriteTime(fileName);

	ScopedCursor cursor(::LoadCursor(nullptr, IDC_WAIT));
	UISetText(0, WStr(wstringbuilder() << "Loading " << fileName));

	namespace fs = boost::filesystem;
	fs::wpath fullPath = fs::system_complete(fs::wpath(fileName));
	m_pRunner.reset(new ExeRunner(fullPath.wstring(), *this));

	m_testIterationCount = 0;
	m_testCaseCount = 0;
	m_testsRunCount = 0;
	m_ignoredTestCount = 0;
	m_failedTestCount = 0;
	m_treeView.Clear();
	m_logView.Clear();
	m_categories.Clear();
	TestCaseLoader loadTestCases(m_treeView, m_categories);
	m_pRunner->TraverseTestTree(loadTestCases);
	m_testCaseCount = loadTestCases.TestCaseCount();
	m_treeView.ExpandToView();
	RestoreTestSelection();
	ClearTestSelection();

	m_progressBar.SetPos(0);

	SetWindowText(WStr(wstringbuilder() << fullPath.filename().wstring() << L" - Boost Test Runner"));
	m_pathName = fullPath.wstring();

	m_mru.AddToList(m_pathName.c_str());
}
catch (NoHeaderError& e)
{
	m_helpType = e.GetUnitTestType();
	MessageBox(WStr(e.what()), LoadString(IDR_APPNAME).c_str(), MB_ICONERROR | MB_HELP | MB_OK);
}

std::string GetListViewText(const CListViewCtrl& listView, int item, int subItem)
{
	CComBSTR bstr;
	listView.GetItemText(item, subItem, bstr.m_str);
	return std::string(bstr.m_str, bstr.m_str + bstr.Length());
}

void CMainFrame::CreateHpp(int resourceId, const std::wstring& fileName)
{
	CResource resource;
	if (!resource.Load(RT_RCDATA, MAKEINTRESOURCE(resourceId)))
		ThrowLastError("RT_RCDATA");

	std::ofstream f(fileName.c_str(), std::ios::out | std::ios::binary);
	f.write(static_cast<const char*>(resource.Lock()), resource.GetSize());
	f.close();
	if (!f)
		ThrowLastError(fileName);
}

void CMainFrame::OnFileOpen(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	CFileDialog dlg(TRUE, L"*.exe;*.dll", L"", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		L"Unit Test Executable (*.exe)\0*.exe\0"
		L"NUnit Test Assembly (*.dll)\0*.dll\0"
		L"All Unit Test Files (*.exe; *.dll)\0*.exe;*.dll\0"
		L"\0", 0);
	dlg.m_ofn.nFilterIndex = 3;
	dlg.m_ofn.lpstrTitle = L"Load Unit Test";
	if (dlg.DoModal() != IDOK)
		return;

	LoadNew(dlg.m_szFileName);
}

std::wstring CMainFrame::GetLogFileName(const std::wstring& fileName) const
{
	CFileDialog dlg(false, L".txt", fileName.c_str(), OFN_OVERWRITEPROMPT, L"Text Files (*.txt)\0*.txt\0All Files\0*.*\0\0", 0);
	dlg.m_ofn.nFilterIndex = 0;
	dlg.m_ofn.lpstrTitle = L"Save unit test log";
	return dlg.DoModal() == IDOK ? dlg.m_szFileName : L"";
}

void CMainFrame::SaveLogFile(const std::wstring& fileName)
{
	UISetText(0, WStr(wstringbuilder() << "Saving " << fileName));
	ScopedCursor cursor(::LoadCursor(nullptr, IDC_WAIT));
	m_logView.Save(fileName);
	m_logFileName = fileName;
	UpdateStatusBar();
}

void CMainFrame::OnFileSave(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	namespace fs = boost::filesystem;

	auto fileName = m_logFileName.empty() ? GetLogFileName(fs::wpath(m_pathName).replace_extension(L"txt").wstring()) : m_logFileName;
	if (!fileName.empty())
		SaveLogFile(fileName);
}

void CMainFrame::OnFileSaveAs(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	auto fileName = GetLogFileName(m_logFileName);
	if (!fileName.empty())
		SaveLogFile(fileName);
}

void CMainFrame::OnFileAutoRun(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	m_autoRun = !m_autoRun;
}

void CMainFrame::OnFileCreateBoostHpp(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	CFileDialog dlg(FALSE, L".hpp", L"unit_test_gui.hpp", OFN_OVERWRITEPROMPT, L"C++ Header Files (*.hpp)\0*.hpp\0All Files\0*.*\0\0", 0);
	dlg.m_ofn.nFilterIndex = 0;
	dlg.m_ofn.lpstrTitle = L"Create unit_test_gui.hpp";
	if (dlg.DoModal() != IDOK)
		return;

	ScopedCursor cursor(::LoadCursor(nullptr, IDC_WAIT));
	CreateHpp(IDR_UNIT_TEST_GUI_HPP, dlg.m_szFileName);
}

void CMainFrame::OnFileCreateGoogleHpp(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	CFileDialog dlg(FALSE, L".h", L"gtest-gui.h", OFN_OVERWRITEPROMPT, L"C++ Header Files (*.h)\0*.h\0All Files\0*.*\0\0", 0);
	dlg.m_ofn.nFilterIndex = 0;
	dlg.m_ofn.lpstrTitle = L"Create gtest-gui.h";
	if (dlg.DoModal() != IDOK)
		return;

	ScopedCursor cursor(::LoadCursor(nullptr, IDC_WAIT));
	CreateHpp(IDR_GTEST_GUI_H, dlg.m_szFileName);
}

void CMainFrame::OnLogAutoClear(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	m_logAutoClear = !m_logAutoClear;
}

void CMainFrame::OnResetSelection(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	ClearTestSelection();
	Reload();
}

void CMainFrame::OnLogSelectAll(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	m_logView.SelectAll();
}

void CMainFrame::OnLogClear(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	m_logView.Clear();
}

void CMainFrame::OnLogTime(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	m_logView.SetClockTime(!m_logView.GetClockTime());
}

void CMainFrame::OnLogCopy(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	ScopedCursor cursor(::LoadCursor(nullptr, IDC_WAIT));
	m_logView.Copy();
}

void CMainFrame::OnLogFind(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	m_findDlg.SetFocus();
}

void CMainFrame::test_message(Severity::type severity, const std::string& msg)
{
	SYSTEMTIME localTime;
	GetLocalTime(&localTime);
	double t = m_resetTimer? (m_timer.Reset(), 0): m_timer.Get();
	m_resetTimer = false;
	std::string text = msg;
	EnQueue([this, localTime, t, severity, text]()
	{
		AddLogMessage(localTime, t, severity, text);
	});
}

void CMainFrame::AddLogMessage(const SYSTEMTIME& localTime, double t, Severity::type severity, const std::string& msg)
{
	m_logView.Add(m_currentId, localTime, t, severity, msg);
}

void CMainFrame::SelectItem(unsigned id)
{
	m_treeView.SelectTestItem(id);
}

bool CMainFrame::IsActiveItem(unsigned id) const
{
	return m_pRunner && m_pRunner->GetTestUnit(id).active;
}

TestUnit CMainFrame::GetTestItem(unsigned id) const
{
	return m_pRunner->GetTestUnit(id);
}

void CMainFrame::test_waiting(const std::wstring& processName, unsigned processId)
{
	EnQueue([this, processName, processId]()
	{
		if (this->MessageBox(
			WStr(wstringbuilder() << L"Attach debugger to " << processName << L", pid: "<< processId),
			LoadString(IDR_APPNAME).c_str(),
			MB_OKCANCEL) == IDOK)
		{
			m_pRunner->Continue();
		}
		else
		{
			m_pRunner->Abort();
		}
	});
}

void CMainFrame::test_start()
{
}

void CMainFrame::test_finish()
{
}

void CMainFrame::test_aborted()
{
}

void CMainFrame::test_iteration_start(unsigned /*test_cases_amount*/)
{
	EnQueue([this]()
	{
		m_currentId = m_pRunner->RootTestSuite().id;
		m_progressBar.SetPos(0);
		UpdateProgressBar();
		m_treeView.ResetTreeImages();
	});
}

void CMainFrame::test_iteration_finish()
{
	EnQueue([this]()
	{
		++m_testIterationCount;
		UpdateStatusBar();
	});
}

void CMainFrame::test_suite_start(unsigned id)
{
	EnQueue([this, id]()
	{
		m_currentId = id;
		m_treeView.BeginTestSuite(id);
		m_logView.BeginTestUnit(id);
	});
}

void CMainFrame::test_case_start(unsigned id)
{
	EnQueue([this, id]()
	{
		m_currentId = id;
		m_testCaseState = TestCaseState::Success;
		m_treeView.BeginTestCase(id);
		m_logView.BeginTestUnit(id);
	});
}

void CMainFrame::test_case_finish(unsigned id, unsigned long elapsed)
{
	EnQueue([this, id, elapsed]()
	{
		EndTestCase(id, elapsed, m_testCaseState);
	});
}

void CMainFrame::test_case_finish(unsigned id, unsigned long elapsed, TestCaseState::type state)
{
	EnQueue([this, id, elapsed, state]()
	{
		EndTestCase(id, elapsed, state);
	});
}

void CMainFrame::UpdateProgressBar()
{
	int pbst =
		m_failedTestCount > 0 || m_testCaseState == TestCaseState::Failed ? PBST_ERROR :
		m_ignoredTestCount > 0 || m_testCaseState == TestCaseState::Ignored ? PBST_PAUSED : PBST_NORMAL;
	m_progressBar.SendMessage(PBM_SETSTATE, pbst);
	COLORREF color =
		pbst == PBST_ERROR ? FailColor : 
		pbst == PBST_PAUSED ? WarnColor : SuccessColor;
	m_progressBar.SetBarColor(color);
	m_progressBar.SetPos(m_progressBar.GetPos()); // Win7 progress bar is one off when calling SetPos() just once ?!
}

void CMainFrame::EndTestCase(unsigned id, unsigned long /*elapsed*/, TestCaseState::type state)
{
	m_treeView.EndTestCase(id, state);
	m_logView.EndTestUnit(id);

	if (state == TestCaseState::Ignored)
		++m_ignoredTestCount;
	if (state == TestCaseState::Failed)
		++m_failedTestCount;

	++m_testsRunCount;
	m_progressBar.OffsetPos(1);
	UpdateStatusBar();
	UpdateProgressBar();
}

void CMainFrame::test_suite_finish(unsigned id, unsigned long /*elapsed*/)
{
	EnQueue([this, id]()
	{
		m_treeView.EndTestSuite(id);
		m_logView.EndTestUnit(id);
		UpdateProgressBar();
	});
}

void CMainFrame::test_unit_skipped(unsigned id)
{
	EnQueue([this, id]()
	{
		CountTestCases ctc;
		m_pRunner->TraverseTestTree(id, ctc);
		m_progressBar.OffsetPos(ctc.count());
		UpdateProgressBar();
	});
}

void CMainFrame::test_unit_aborted(unsigned /*id*/)
{
}

void CMainFrame::UpdateStatusBar()
{
	bool isLoaded = m_pRunner.get() != nullptr;
	bool isRunning = isLoaded && m_pRunner->IsRunning();

	UISetText(ID_DEFAULT_PANE, isRunning ? L"Running..." : L"Ready");
	UISetText(ID_ITERATIONS_PANE, isLoaded ? WStr(wstringbuilder() << L"Test iterations: " << m_testIterationCount) : L"");
	UISetText(ID_TOTAL_PANE, isLoaded ? WStr(wstringbuilder() << L"Test cases: " << m_testCaseCount) : L"");
	UISetText(ID_RUN_PANE, isLoaded ? WStr(wstringbuilder() << L"Tests run: " << m_testsRunCount) : L"");
	UISetText(ID_FAILED_PANE, isLoaded ? WStr(wstringbuilder() << L"Failed tests: " << m_failedTestCount) : L"");
}

void CMainFrame::test_unit_ignored(const std::string& /*msg*/)
{
	EnQueue([this]()
	{
		m_testCaseState = TestCaseState::Ignored;
		UpdateProgressBar();
	});
}

void CMainFrame::assertion_result(bool passed)
{
	if (passed)
		return;

	EnQueue([this]()
	{
		m_testCaseState = TestCaseState::Failed;
		UpdateProgressBar();
	});
}

void CMainFrame::exception_caught(const std::string& /*what*/)
{
	EnQueue([this]()
	{
		m_testCaseState = TestCaseState::Failed;
		UpdateProgressBar();
		UpdateStatusBar();
	});
}

void CMainFrame::TestStarted()
{
	EnQueue([this]()
	{
		m_treeView.OnTestStart();
	});
}

void CMainFrame::TestFinished()
{
	EnQueue([this]()
	{
		m_pRunner->Wait();
		m_treeView.OnTestFinish();
	});
}

void CMainFrame::OnTestRandomize(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	m_randomize = !m_randomize;
}

void CMainFrame::OnTestRepeat(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	m_repeat = !m_repeat;
	m_pRunner->SetRepeat(m_repeat);
}

void CMainFrame::OnTestDebugger(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	m_debugger = !m_debugger;
}

void CMainFrame::OnTestRunnerArgs(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	CArgumentsDlg dlg(m_arguments);
	if (dlg.DoModal() == IDOK)
		m_arguments = dlg.GetArguments();
}

unsigned CMainFrame::GetOptions() const
{
	unsigned options = 0;
	if (m_logView.GetClockTime())
		options |= Options::ClockTime;
	if (m_autoRun)
		options |= Options::AutoRun;
	if (m_logAutoClear)
		options |= Options::LogAutoClear;
	if (m_debugger)
		options |= TestRunner::WaitForDebugger;
	if (m_randomize)
		options |= TestRunner::Randomize;
	if (m_repeat)
		options |= TestRunner::Repeat;
	return options;
}

void CMainFrame::OnTestAbort(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	m_pRunner->Abort();
}

void CMainFrame::OnTestCategories(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	CategoryDlg dlg(m_categories);
	if (dlg.DoModal() != IDOK)
		return;

	CategoryFilter filter(m_treeView, m_categories);
	m_pRunner->TraverseTestTree(filter);
	m_treeView.RedrawWindow();
}

void LoadRichEditLibrary()
{
	static HINSTANCE h = ::LoadLibrary(CRichEditCtrl::GetLibraryName());
	if (!h)
		ThrowLastError(CRichEditCtrl::GetLibraryName());
}

void CMainFrame::OnHelpBoost(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	LoadRichEditLibrary();
	CBoostHelpDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnHelpGoogle(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	LoadRichEditLibrary();
	CGoogleHelpDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnHelpNUnit(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	LoadRichEditLibrary();
	CNUnitHelpDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnAppAbout(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnTreeRun(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	RunSingle(m_treeView.GetSelectedTestItem());
}

void CMainFrame::OnTreeRunChecked(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	RunChecked();
}

void CMainFrame::OnTreeRunAll(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	RunAll();
}

void CMainFrame::OnTreeCheckAll(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	m_treeView.CheckAll();
}

void CMainFrame::OnTreeUncheckAll(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	m_treeView.UncheckAll();
}

void CMainFrame::OnTreeCheckFailed(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	m_treeView.CheckFailed();
}

void CMainFrame::OnTreeCopyName(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
{
	CopyToClipboard(GetTestItem(m_treeView.GetSelectedTestItem()).fullName, *this);
}

void CMainFrame::OnMruMenuItem(UINT /*uCode*/, int nID, HWND /*hwndCtrl*/)
{
	wchar_t pathName[m_mru.m_cchMaxItemLen_Max + 1];
	if (!m_mru.GetFromList(nID, pathName, m_mru.m_cchMaxItemLen_Max))
		return;

	m_mru.RemoveFromList(nID);
	LoadNew(pathName);
}

void CMainFrame::SetLogHighLight(unsigned id)
{
	m_logView.SetHighLight(id);
}

void CMainFrame::FindNext(const std::wstring& text)
{
	if (!m_logView.FindNext(text))
		MessageBeep(MB_ICONASTERISK);
}

void CMainFrame::FindPrevious(const std::wstring& text)
{
	if (!m_logView.FindPrevious(text))
		MessageBeep(MB_ICONASTERISK);
}

void CMainFrame::OnClose()
{
	m_pRunner.reset();
	SaveSettings();
	DestroyWindow();
}

const wchar_t* RegistryPath = L"Software\\Boost\\BoostTestUi";

bool CMainFrame::LoadSettings()
{
	DWORD x, y, cx, cy;
	CRegKey reg;
	if (reg.Open(HKEY_CURRENT_USER, RegistryPath, KEY_READ) != ERROR_SUCCESS)
		return false;
	reg.QueryDWORDValue(L"x", x);
	reg.QueryDWORDValue(L"y", y);
	reg.QueryDWORDValue(L"width", cx);
	reg.QueryDWORDValue(L"height", cy);
	SetWindowPos(0, x, y, cx, cy, SWP_NOZORDER);
	DWORD split;
	reg.QueryDWORDValue(L"vSplit", split);
	m_vSplit.SetSplitterPos(split);

	m_logView.LoadSettings(reg);

	DWORD options;
	if (reg.QueryDWORDValue(L"Options", options) == ERROR_SUCCESS)
	{
		m_autoRun = (options & Options::AutoRun) != 0;
		m_logAutoClear = (options & Options::LogAutoClear) != 0;
		m_logView.SetClockTime((options & Options::ClockTime) != 0);
		m_randomize = (options & TestRunner::Randomize) != 0;
		m_repeat = (options & TestRunner::Repeat) != 0;
		m_debugger = (options & TestRunner::WaitForDebugger) != 0;
	}

	DWORD logLevel;
	if (reg.QueryDWORDValue(L"LogLevel", logLevel) == ERROR_SUCCESS)
	{
		m_combo.SetCurSel(logLevel);
	}

	m_mru.ReadFromRegistry(RegistryPath);

	return true;
}

void CMainFrame::SaveSettings()
{
	WINDOWPLACEMENT placement;
	placement.length = sizeof(placement);
	GetWindowPlacement(&placement);

	CRegKey reg;
	reg.Create(HKEY_CURRENT_USER, RegistryPath);
	reg.SetDWORDValue(L"x", placement.rcNormalPosition.left);
	reg.SetDWORDValue(L"y", placement.rcNormalPosition.top);
	reg.SetDWORDValue(L"width", placement.rcNormalPosition.right - placement.rcNormalPosition.left);
	reg.SetDWORDValue(L"height", placement.rcNormalPosition.bottom - placement.rcNormalPosition.top);
	reg.SetDWORDValue(L"vSplit", m_vSplit.GetSplitterPos());
	m_logView.SaveSettings(reg);
	reg.SetDWORDValue(L"Options", GetOptions());
	reg.SetDWORDValue(L"LogLevel", m_combo.GetCurSel());

	m_mru.WriteToRegistry(RegistryPath);
}


class SingleTestCaseSelector : public TestTreeVisitor
{
public:
	SingleTestCaseSelector(const CTreeView& treeView, unsigned id) :
		m_pTreeView(&treeView),
		m_id(id),
		m_enable(false)
	{
	}

	virtual void VisitTestCase(TestCase& tc) override
	{
		if (tc.id == m_id)
			EnableSuites();
		tc.enabled = IsEnabled(tc.id);
	}

	virtual void EnterTestSuite(TestSuite& ts) override
	{
		if (ts.id == m_id)
		{
			EnableSuites();
			m_enable = true;
		}
		ts.enabled = IsEnabled(ts.id);
		m_suites.push_back(&ts);
	}

	virtual void LeaveTestSuite() override
	{
		if (m_suites.back()->id == m_id)
			m_enable = false;
		m_suites.pop_back();
	}

private:
	bool IsEnabled(unsigned id) const
	{
		return m_enable && m_pTreeView->IsChecked(id) || id == m_id;
	}

	void EnableSuites()
	{
		for (auto it = m_suites.begin(); it != m_suites.end(); ++it)
			(*it)->enabled = true;
	}

	const CTreeView* m_pTreeView;
	unsigned m_id;
	bool m_enable;
	std::vector<TestSuite*> m_suites;
};

void CMainFrame::RunSingle(unsigned id)
{
	if (!IsRunnable())
		return;

	SingleTestCaseSelector selector(m_treeView, id);
	m_pRunner->TraverseTestTree(selector);
	Run();
}

class CheckedTestCaseSelector : public TestTreeVisitor
{
public:
	explicit CheckedTestCaseSelector(const CTreeView& treeView) :
		m_pTreeView(&treeView)
	{
	}

	virtual void VisitTestCase(TestCase& tc) override
	{
		tc.enabled = m_pTreeView->IsChecked(tc.id);
	}

	virtual void EnterTestSuite(TestSuite& ts) override
	{
		ts.enabled = m_pTreeView->IsChecked(ts.id);
	}

private:
	const CTreeView* m_pTreeView;
};

bool CMainFrame::IsRunnable() const
{
	return m_pRunner && !m_pRunner->IsRunning();
}

void CMainFrame::RunChecked()
{
	if (!IsRunnable())
		return;

	CheckedTestCaseSelector selector(m_treeView);
	m_pRunner->TraverseTestTree(selector);
	Run();
}

struct TestCaseSelector : TestTreeVisitor
{
	virtual void VisitTestCase(TestCase& tc) override
	{
		tc.enabled = true;
	}

	virtual void EnterTestSuite(TestSuite& ts) override
	{
		ts.enabled = true;
	}
};

void CMainFrame::RunAll()
{
	if (!IsRunnable())
		return;

	TestCaseSelector selector;
	m_pRunner->TraverseTestTree(selector);
	Run();
}

void CMainFrame::Run()
{
	if (!IsRunnable())
		return;

	CountEnabledTestCases counter;
	m_pRunner->TraverseTestTree(counter);
	if (counter.count() == 0)
		return;

	m_testIterationCount = 0;
	m_testsRunCount = 0;
	m_ignoredTestCount = 0;
	m_failedTestCount = 0;
	m_progressBar.SetRange(0, counter.count());
	m_progressBar.SetPos(0);
	if (m_logAutoClear)
		m_logView.Clear();
	m_resetTimer = m_logView.Empty();
	m_pRunner->Run(m_combo.GetCurSel(), GetOptions(), m_arguments);
}

void TreeViewStateStorage::Clear()
{
	m_tests.clear();
}

void TreeViewStateStorage::SaveScrollPos(const CWindow& treeView)
{
	m_scrollPos = treeView.GetScrollPos(SB_VERT);
}

void TreeViewStateStorage::RestoreScrollPos(CWindow& treeView)
{
	 treeView.SetScrollPos(SB_VERT, m_scrollPos);
}

void TreeViewStateStorage::SaveState(const TestUnit& tu, const TreeViewItemState& state)
{
	m_tests.insert(std::make_pair(tu.fullName, state));
}

bool TreeViewStateStorage::RestoreState(TestUnit& tu, TreeViewItemState& state) const
{
	auto it = m_tests.find(tu.fullName);
	if (it == m_tests.end())
		return false;

	state = it->second;
	return true;
}

} // namespace gj
