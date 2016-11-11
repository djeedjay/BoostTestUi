// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#include "stdafx.h"
#include <sstream>
#include <regex>
#include "Utilities.h"
#include "ExeRunner.h"
#include "CatchTest.h"

namespace gj {
namespace CatchTest {

class GetEnableArg : public TestTreeVisitor
{
public:
	GetEnableArg() : m_allCases(true)
	{
	}

	bool AllCases() const
	{
		return m_allCases;
	}

	std::string GetArg() const
	{
		return m_arg;
	}

	virtual void VisitTestCase(TestCase& tc) override
	{
		if (tc.enabled)
			m_cases.push_back(tc.name);
		else
			m_allCases = m_allCasesInSuite = false;
	}

	virtual void EnterTestSuite(TestSuite& ts) override
	{
		m_pTestSuite = &ts;
		m_allCasesInSuite = true;
	}

	virtual void LeaveTestSuite() override
	{
		if (m_allCasesInSuite)
		{
			if (!m_cases.empty())
				AddTestCase("*");
		}
		else
		{
			for (auto it = m_cases.begin(); it != m_cases.end(); ++it)
				AddTestCase(*it);
		}
		m_cases.clear();
	}

private:
	void AddTestCase(const std::string& name)
	{
		if (!m_arg.empty())
			m_arg += ' ';
		if (name.find(' ') == name.npos)
			m_arg += name;
		else
			m_arg += Quote(name);
	}

	const TestSuite* m_pTestSuite;
	bool m_allCases;
	bool m_allCasesInSuite;
	std::vector<std::string> m_cases;
	std::string m_arg;
};

ArgumentBuilder::ArgumentBuilder(const std::wstring& fileName, ExeRunner& runner, TestObserver& observer) :
	m_fileName(fileName),
	m_pRunner(&runner),
	m_pObserver(&observer)
{
}

std::wstring ArgumentBuilder::GetExePathName()
{
	return m_fileName;
}

std::wstring ArgumentBuilder::GetListArg()
{
	return L"--list-test-names-only";
}

std::string FullName(const std::string& testCaseName, const std::string& testName)
{
	return testCaseName + '.' + testName;
}

void ArgumentBuilder::LoadTestUnits(TestUnitNode& tree, std::istream& is, const std::string& testName)
{
	m_rootId = GetId(testName);
	tree.children.push_back(TestSuite(m_rootId, testName));
	TestUnitNode& node = tree.children.back();
	std::string line;
	while (std::getline(is, line))
	{
		line = Chomp(line);
		node.children.push_back(TestCase(GetId(line), line));
	}
}

unsigned ArgumentBuilder::GetEnabledOptions(unsigned /*options*/) const
{
	return ExeRunner::Randomize | ExeRunner::WaitForDebugger;
}

std::wstring ArgumentBuilder::BuildArgs(TestRunner& runner, int /*logLevel*/, unsigned& options)
{
	std::wostringstream args;
	args << L"-r teamcity";

	if (options & ExeRunner::Randomize)
		args << L" --order rand";

	if (options & ExeRunner::WaitForDebugger)
		args << L" --gui_wait";

	GetEnableArg getArg;
	runner.TraverseTestTree(getArg);
	if (!getArg.AllCases())
		args << L" " << MultiByteToWideChar(getArg.GetArg());

	return args.str();
}

template <typename T>
T get_arg(const std::string& s)
{
	std::stringstream ss;
	ss << s;
	T value = T();
	ss >> value;
	return value;
}

unsigned ArgumentBuilder::GetId(const std::string& name)
{
	auto it = m_ids.find(name);
	if (it != m_ids.end())
		return it->second;

	unsigned id = m_ids.size();
	m_ids[name] = id;
	return id;
}

void ArgumentBuilder::FilterMessage(const std::string& msg)
{
	static const std::regex reWaiting("^#waiting");
	static const std::regex reStart("^\\[==========\\] Running (\\d+) tests? from \\d+ test cases?.");
	static const std::regex reTest("^\\[----------\\] \\d+ tests? from ([\\w_/]+)( \\((\\d+) ms total\\))?");
	static const std::regex reBegin("^\\[ RUN      \\] ([\\w\\._/]+)");
	static const std::regex reError("\\(\\d+\\): error: ");
//	static const std::regex reEnd("^\\[(       OK )|(  FAILED  )\\] ([\\w\\._/]+) \\((\\d+) ms\\)"); // VC regex bug??
	static const std::regex reEnd("^\\[(       OK |  FAILED  )\\] ([\\w\\._/]+).*\\((\\d+) ms\\)");
	static const std::regex reFinish("^\\[==========\\] \\d+ tests? from \\d+ test cases? ran. \\((\\d+) ms total\\)");
	static const std::regex reAssertion("Assertion failed:");

	Severity::type severity = Severity::Info;
	std::smatch sm;
	if (std::regex_search(msg, sm, reWaiting))
		return m_pRunner->OnWaiting();
	else if (std::regex_search(msg, sm, reStart))
	{
		m_pRunner->OnTestIterationStart(get_arg<unsigned>(sm[1]));
		m_pRunner->OnTestSuiteStart(m_rootId);
	}
	else if (std::regex_search(msg, sm, reTest) && !sm[2].matched)
	{
		m_pRunner->OnTestSuiteStart(GetId(sm[1]));
	}
	else if (std::regex_search(msg, sm, reBegin))
	{
		m_pRunner->OnTestCaseStart(GetId(sm[1]));
	}
	else if (std::regex_search(msg, reError))
	{
		severity = Severity::Error;
		m_pRunner->OnTestAssertion(false);
	}
	else if (std::regex_search(msg, reAssertion))
	{
		severity = Severity::Assertion;
	}

	m_pObserver->test_message(severity, msg);

	if (std::regex_search(msg, sm, reEnd))
	{
		m_pRunner->OnTestCaseFinish(GetId(sm[2]), get_arg<unsigned>(sm[3]));
	}
	else if (std::regex_search(msg, sm, reTest) && sm[2].matched)
	{
		m_pRunner->OnTestSuiteFinish(GetId(sm[1]), get_arg<unsigned>(sm[3]));
	}
	else if (std::regex_search(msg, sm, reFinish))
	{
		m_pRunner->OnTestSuiteFinish(m_rootId, get_arg<unsigned>(sm[1]));
		m_pRunner->OnTestIterationFinish();
	}
}

} // namespace CatchTest
} // namespace gj
