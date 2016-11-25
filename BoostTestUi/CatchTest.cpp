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
			m_arg += ',';
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
	return L"--list-tests";
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
		auto b = line.c_str();
		auto p = b;
		while (*p && std::isspace(*p))
			++p;
		if (p == b || *p == '\0')
			continue;

		if (!node.children.empty() && *p == '[')
		{
			b = nullptr;
			while (*p)
			{
				if (*p == '[')
					b = p + 1;
				else if (b && *p == ']' && p != b)
				{
					std::string tag(b, p);
					if (tag == "." || tag == "hide")
						node.children.back().data.enabled = false;
					else if (std::isalnum(tag[0]))
						node.children.back().data.categories.push_back(tag);
					b = nullptr;
				}
				++p;
			}
		}
		else
		{
			std::string name(p);
			node.children.push_back(TestCase(GetId(name), name));
		}
	}
}

unsigned ArgumentBuilder::GetEnabledOptions(unsigned /*options*/) const
{
	return ExeRunner::Randomize | ExeRunner::WaitForDebugger;
}

std::wstring ArgumentBuilder::BuildArgs(TestRunner& runner, int logLevel, unsigned& options)
{
	std::wostringstream args;

	args << L"-r boosttestui";

	if (logLevel > 1)
		args << " --success";

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

template <typename T>
T GetArg(std::istream& is)
{
	T value = T();
	is >> value;
	return value;
}

std::string GetArg(std::istream& is)
{
	while (std::isspace(is.peek()))
		is.get();
	std::string value;
	std::getline(is, value);
	return value;
}

void ArgumentBuilder::HandleClientNotification(const std::string& line)
{
	std::istringstream ss(line);
	char c;
	std::string command;
	ss >> c >> command;

	if (command == "RunStarted")
	{
		m_pRunner->OnTestIterationStart(GetArg<unsigned>(ss));
		m_pRunner->OnTestSuiteStart(m_rootId);
	}
	else if (command == "RunFinished")
	{
		m_pRunner->OnTestSuiteFinish(m_rootId, 0);
		m_pRunner->OnTestIterationFinish();
	}
	else if (command == "TestIgnored")
		m_pRunner->OnTestUnitSkipped(GetId(GetArg(ss)));
	else if (command == "TestStarted")
		m_pRunner->OnTestCaseStart(GetId(GetArg(ss)));
	else if (command == "SuiteStarted")
		m_pRunner->OnTestSuiteStart(GetId(GetArg(ss)));
	else if (command == "Assertion")
		m_pRunner->OnTestAssertion(GetArg<bool>(ss));
	else if (command == "TestFinished")
	{
		auto state = GetArg<bool>(ss) ? TestCaseState::Success : TestCaseState::Failed;
		auto id = GetId(GetArg(ss));
		m_pRunner->OnTestCaseFinish(id, 0, state);
	}
	else if (command == "SuiteFinished")
	{
		m_pRunner->OnTestSuiteFinish(GetId(GetArg(ss)), 0);
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

//	static const std::regex reError(": Error$");
//	std::smatch sm;
//	if (std::regex_search(msg, sm, reError))
//		severity = Severity::Error;

//	static const std::regex reFailure(": Failure$");
//	if (std::regex_search(msg, sm, reFailure))
//		severity = Severity::Fatal;

	m_pObserver->test_message(Severity::Info, msg);
}

} // namespace CatchTest
} // namespace gj
