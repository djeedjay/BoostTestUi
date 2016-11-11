// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#include "stdafx.h"
#include <array>
#include <sstream>
#include <regex>
#include <boost/filesystem.hpp>
#include "Utilities.h"
#include "ExeRunner.h"
#include "NUnitTest.h"

namespace gj {
namespace NUnitTest {

const wchar_t* GetLogLevelArg(int logLevel)
{
	switch (logLevel)
	{
	case 0: return L"minimal";
	case 1: return L"medium";
	case 2:
	default: return L"all";
	}
}

class GetEnableArg : public TestTreeVisitor
{
public:
	std::wstring GetArg() const
	{
		return m_arg;
	}

	virtual void VisitTestCase(TestCase& tc) override
	{
		m_arg += GetChar(tc.enabled);
	}

	virtual void EnterTestSuite(TestSuite& ts) override
	{
		m_arg += GetChar(ts.enabled);
	}

private:
	static wchar_t GetChar(bool enabled)
	{
		return enabled? L'1': L'0';
	}

	std::wstring m_arg;
};

class GetEnableArg2 : public TestTreeVisitor
{
public:
	GetEnableArg2() : m_suites(1, Suite(""))
	{
	}

	bool AllCases() const
	{
		return m_suites[0].all;
	}

	std::wstring GetArg() const
	{
		std::wstring arg;
		const Suite& suite = m_suites[0];
		for (auto it = suite.cases.begin(); it != suite.cases.end(); ++it)
		{
			if (!arg.empty())
				arg += L',';
			arg += MultiByteToWideChar(*it);
		}
		return arg;
	}

	static std::string MakeTestCaseName(const std::string& name)
	{
		std::string s;
		s.reserve(name.size() + 2);
		for (auto it = name.begin(); it != name.end(); ++it)
		{
			if (*it == '"')
				s += '\\';
			s += *it;
		}
		return s;
	}

	virtual void VisitTestCase(TestCase& tc) override
	{
		if (tc.enabled)
			m_suites.back().cases.push_back(MakeTestCaseName(tc.name));
		else
			m_suites.back().all = false;
	}

	virtual void EnterTestSuite(TestSuite& ts) override
	{
		m_suites.push_back(Suite(ts.name));
	}

	virtual void LeaveTestSuite() override
	{
		if (m_suites.size() <= 1)
			return; // Skip top level

		auto it = m_suites.rbegin();
		Suite& suite = *it++;
		Suite& parent = *it;
		if (suite.all)
		{
			if (!suite.cases.empty())
				parent.cases.push_back(suite.name);
		}
		else
		{
			parent.all = false;
			for (auto it = suite.cases.begin(); it != suite.cases.end(); ++it)
				parent.cases.push_back(suite.name + '.' + *it);
		}
		m_suites.pop_back();
	}

private:
	struct Suite
	{
		explicit Suite(const std::string& name) : name(name), all(true) { }

		std::string name;
		bool all;
		std::vector<std::string> cases;
	};

	std::vector<Suite> m_suites;
};


ArgumentBuilder::ArgumentBuilder(const std::wstring& exeName, const std::wstring& fileName, ExeRunner& runner, TestObserver& observer) :
	m_exeName(exeName),
	m_fileName(fileName),
	m_pRunner(&runner),
	m_pObserver(&observer),
	m_exception(false)
{
}

namespace fs = boost::filesystem;

fs::wpath GetParentPath(const std::wstring& fileName)
{
	return fs::wpath(fileName).parent_path();
}

fs::wpath GetTestUiPath()
{
	std::array<wchar_t, MAX_PATH> buf;
	DWORD size = GetModuleFileNameW(nullptr, buf.data(), buf.size());
	if (size == 0)
		ThrowLastError("GetModuleFileName");
	return GetParentPath(buf.data());
}

fs::wpath GetNUnitPath()
{
	CRegKey key;
	std::array<wchar_t, MAX_PATH> buf;
	ULONG length;
	if (key.Open(HKEY_CURRENT_USER, L"Software\\Classes\\NUnitTestProject\\DefaultIcon", KEY_READ) == ERROR_SUCCESS &&
		key.QueryStringValue(L"", buf.data(), &length) == ERROR_SUCCESS)
		return GetParentPath(buf.data());
	return fs::wpath();
}

std::wstring ArgumentBuilder::GetExePathName()
{
	fs::wpath runner(m_exeName);
	return fs::wpath(GetTestUiPath() / runner).wstring();
}

std::wstring ArgumentBuilder::GetListArg()
{
	return L"/list \"" + m_fileName + L"\"";
}

template <typename T>
T GetArg(std::istream& is)
{
	T value = T();
	is >> value;
	return value;
}

template <typename T>
T GetArg(const std::string& s)
{
	std::istringstream ss(s);
	return GetArg<T>(ss);
}

TestUnit::Type GetTestUnitType(const std::string& s)
{
	if (s.size() == 1)
	{
		switch (s[0])
		{
		case 'c': case 'C': return TestUnit::TestCase;
		case 's': case 'S': return TestUnit::TestSuite;
		}
	}
	throw std::runtime_error("Unexpected TestUnit");
}

bool GetTestUnitEnable(const std::string& s)
{
	if (s.size() == 1)
	{
		switch (s[0])
		{
		case 'c': case 's': return false;
		case 'C': case 'S': return true;
		}
	}
	throw std::runtime_error("Unexpected TestUnit");
}

std::vector<std::string> GetCategories(const std::string& s)
{
	std::vector<std::string> categories;
	auto it = s.begin();
	while (it != s.end())
	{
		auto next = std::find(it, s.end(), ',');
		categories.push_back(std::string(it, next));
		if (next == s.end())
			break;
		it = next + 1;
	}
	return categories;
}

TestCaseState::type GetTestCaseState(std::istream& is)
{
	std::string state;
	is >> state;
	if (state == "0")
		return TestCaseState::Failed;
	if (state == "1")
		return TestCaseState::Success;
	if (state == "-")
		return TestCaseState::Ignored;

	return TestCaseState::Failed;
}

std::string LoadTestUnits(TestUnitNode& node, std::istream& is, TestObserver* pObserver, int indent = 0)
{
	static const std::regex re("(\\s*)([cCsS])(\\d+):\\[(.*)\\](.+)");

	std::string line;
	std::getline(is, line);
	while (is)
	{
		line = Chomp(line);
		std::smatch sm;
		if (!std::regex_match(line, sm, re))
		{
			pObserver->test_message(Severity::Info, line);
			std::getline(is, line);
			continue;
		}

		if (sm[1].length() < indent)
			return line;

		unsigned id = GetArg<unsigned>(sm[3]);
		TestUnit::Type type = GetTestUnitType(sm[2]);
		std::string name = sm[5];
		bool enable = GetTestUnitEnable(sm[2]);

		TestUnit tu(id, type, name, enable);
		tu.categories = GetCategories(sm[4]);
		node.children.push_back(tu);
		if (type == TestUnit::TestSuite)
			line = LoadTestUnits(node.children.back(), is, pObserver, sm[1].length() + 1);
		else
			std::getline(is, line);
	}
	return "";
}

void ArgumentBuilder::LoadTestUnits(TestUnitNode& tree, std::istream& is, const std::string&)
{
	NUnitTest::LoadTestUnits(tree, is, m_pObserver);
}

unsigned ArgumentBuilder::GetEnabledOptions(unsigned /*options*/) const
{
	return ExeRunner::Randomize | ExeRunner::WaitForDebugger;
}

std::wstring ArgumentBuilder::BuildArgs(TestRunner& runner, int logLevel, unsigned& options)
{
	std::wostringstream args;
	args << L"/log:" << GetLogLevelArg(logLevel);

	if (options & ExeRunner::Randomize)
		args << L" /randomize";
	if (options & ExeRunner::WaitForDebugger)
		args << L" /wait";

	GetEnableArg2 getArg;
	runner.TraverseTestTree(getArg);
	args << L" /run";
	if (!getArg.AllCases())
		args << L":" << getArg.GetArg();
	args << L" \"" << m_fileName + L"\"";
	return args.str();
}

void ArgumentBuilder::HandleClientNotification(const std::string& line)
{
	std::istringstream ss(line);
	char c;
	std::string command;
	ss >> c >> command;

	if (command == "RunStarted")
		m_pRunner->OnTestIterationStart(GetArg<unsigned>(ss));
	else if (command == "RunFinished")
		m_pRunner->OnTestIterationFinish();
	else if (command == "aborted")
		m_pObserver->test_aborted();
	else if (command == "TestStarted")
	{
		if (auto p = m_pRunner->GetTestUnitPtr(GetArg<unsigned>(ss)))
			m_pRunner->OnTestCaseStart(p->id);
	}
	else if (command == "SuiteStarted")
	{
		if (auto p = m_pRunner->GetTestUnitPtr(GetArg<unsigned>(ss)))
			m_pRunner->OnTestSuiteStart(p->id);
	}
	else if (command == "TestFinished")
	{
		auto id = GetArg<unsigned>(ss);
		auto elapsed = GetArg<unsigned>(ss);
		auto state = GetTestCaseState(ss);
		if (auto p = m_pRunner->GetTestUnitPtr(id))
			m_pRunner->OnTestCaseFinish(p->id, elapsed, state);
	}
	else if (command == "SuiteFinished")
	{
		auto id = GetArg<unsigned>(ss);
		auto elapsed = GetArg<unsigned>(ss);
		if (auto p = m_pRunner->GetTestUnitPtr(id))
			m_pRunner->OnTestSuiteFinish(p->id, elapsed);
	}
	else if (command == "BeginException")
	{
		m_pRunner->OnTestExceptionCaught(GetArg<std::string>(ss));
		m_exception = true;
	}
	else if (command == "EndException")
	{
		m_exception = false;
	}
	else if (command == "Waiting")
	{
		m_pRunner->OnWaiting();
	}
	else
	{
		m_pObserver->test_message(Severity::Info, line);
	}
}

void ArgumentBuilder::FilterMessage(const std::string& msg)
{
	if (msg[0] == '#')
		return HandleClientNotification(msg);

	Severity::type severity = m_exception ? Severity::Fatal : Severity::Info;

//	static const std::regex reError(": Error$");
//	std::smatch sm;
//	if (std::regex_search(msg, sm, reError))
//		severity = Severity::Error;

//	static const std::regex reFailure(": Failure$");
//	if (std::regex_search(msg, sm, reFailure))
//		severity = Severity::Fatal;

	m_pObserver->test_message(severity, msg);
}

} // namespace NUnitTest
} // namespace gj
