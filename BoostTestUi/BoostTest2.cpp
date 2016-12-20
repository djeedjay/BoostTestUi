// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#include "stdafx.h"
#include <sstream>
#include <regex>
#include "ExeRunner.h"
#include "Utilities.h"
#include "BoostTest2.h"

namespace gj {
namespace BoostTest2 {

const wchar_t* GetLogLevelArg(int logLevel)
{
	switch (logLevel)
	{
	case 0: return L"error";
	case 1: return L"message";
	case 2:
	default: return L"all";
	}
}

class GetEnableArg2 : public TestTreeVisitor
{
public:
	GetEnableArg2() : m_suites(1, Suite(""))
	{
	}

	bool AllCases() const
	{
		return m_suites[1].all;
	}

	std::wstring GetArg() const
	{
		std::wstring arg;
		const Suite& suite = m_suites[1];
		for (auto it = suite.cases.begin(); it != suite.cases.end(); ++it)
		{
			if (!arg.empty())
				arg += L' ';
			arg += L"--run_test=";
			arg += MultiByteToWideChar(*it);
		}
		return arg;
	}

	virtual void VisitTestCase(TestCase& tc) override
	{
		if (tc.enabled)
			m_suites.back().cases.push_back(tc.name);
		else
			m_suites.back().all = false;
	}

	virtual void EnterTestSuite(TestSuite& ts) override
	{
		m_suites.push_back(Suite(ts.name));
	}

	virtual void LeaveTestSuite() override
	{
		if (m_suites.size() <= 2)
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
				parent.cases.push_back(suite.name + '/' + *it);
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
	return L"--list_content=DOT";
}

std::string normalize_type(const std::string& s)
{
	static const std::regex re1("(class|struct|union|enum) ");
	auto s1 = std::regex_replace(s, re1, std::string());

	// reduce std::basic_stream, std::basic_istream, std::basic_ostream, std::basic_stringstream to their common typedefs:
	static const std::regex re2("std::basic_(string|if?stream|of?stream|([io]?stringstream))<(char|(w)char_t),std::char_traits<\\3>,std::allocator<\\3> >");
	return std::regex_replace(s1, re2, std::string("std::$4$1"));
}

template <typename T>
T GetArg(std::istream& is)
{
	T value = T();
	is >> value;
	return value;
}

void LoadTestUnits(TestUnitNode& node, std::istream& is, TestObserver* pObserver)
{
	static const std::regex re("tu(\\d+)\\[.*color=(\\w+).*label=\"(.*?)(\\|.*)?\"\\];");

	std::string line;
	while (std::getline(is, line))
	{
		line = Chomp(line);
		std::smatch sm;

		if (line == "{")
		{
			LoadTestUnits(node.children.back(), is, pObserver);
		}
		else if (line == "}")
		{
			return;
		}
		else if (std::regex_match(line, sm, re))
		{
			auto id = std::stoi(sm[1]);
			auto color = sm[2];
			auto label = sm[3];
			auto type = id < 65536 ? TestUnit::TestSuite : TestUnit::TestCase;
			auto enable = color == "green";
			node.children.push_back(TestUnit(id, type, label, enable));
		}
		else
		{
			pObserver->test_message(Severity::Info, line);
		}
	}
}

void ArgumentBuilder::LoadTestUnits(TestUnitNode& tree, std::istream& is, const std::string&)
{
	BoostTest2::LoadTestUnits(tree, is, m_pObserver);
}

unsigned ArgumentBuilder::GetEnabledOptions(unsigned /*options*/) const
{
	return ExeRunner::Randomize | ExeRunner::WaitForDebugger;
}

std::wstring ArgumentBuilder::BuildArgs(TestRunner& runner, int logLevel, unsigned& options)
{
	std::wostringstream args;
	args << L"--log_level=" << GetLogLevelArg(logLevel);

	if (options & ExeRunner::Randomize)
		args << L" --random=1";

	if (options & ExeRunner::Repeat)
		options &= ~ExeRunner::WaitForDebugger;
	if (options & ExeRunner::WaitForDebugger)
		args << L" --wait_for_debugger";

	GetEnableArg2 getArg;
	runner.TraverseTestTree(getArg);
	if (!getArg.AllCases())
		args << L" " << getArg.GetArg();
	args << L" -- --gui_run";
	return args.str();
}

void ArgumentBuilder::HandleClientNotification(const std::string& line)
{
	std::istringstream ss(line);
	char c;
	std::string command;
	ss >> c >> command;

	if (command == "start")
		m_pRunner->OnTestIterationStart(GetArg<unsigned>(ss));
	else if (command == "finish")
		m_pRunner->OnTestIterationFinish();
	else if (command == "aborted")
		m_pObserver->test_aborted();
	else if (command == "unit_start")
	{
		if (auto p = m_pRunner->GetTestUnitPtr(GetArg<unsigned>(ss)))
		{
			if (p->type == TestUnit::TestCase)
				m_pRunner->OnTestCaseStart(p->id);
			else
				m_pRunner->OnTestSuiteStart(p->id);
		}
	}
	else if (command == "unit_finish")
	{
		unsigned id = GetArg<unsigned>(ss);
		unsigned elapsed = GetArg<unsigned>(ss);
		if (auto p = m_pRunner->GetTestUnitPtr(id))
		{
			if (p->type == TestUnit::TestCase)
				m_pRunner->OnTestCaseFinish(p->id, elapsed);
			else
				m_pRunner->OnTestSuiteFinish(p->id, elapsed);
		}
	}
	else if (command == "unit_skipped")
		m_pRunner->OnTestUnitSkipped(GetArg<unsigned>(ss));
	else if (command == "unit_aborted")
		m_pRunner->OnTestUnitAborted(GetArg<unsigned>(ss));
	else if (command == "assertion")
		m_pRunner->OnTestAssertion(GetArg<bool>(ss));
	else if (command == "exception")
		m_pRunner->OnTestExceptionCaught(GetArg<std::string>(ss));
	else
		m_pObserver->test_message(Severity::Info, line);
}

void ArgumentBuilder::FilterMessage(const std::string& msg)
{
	if (msg == "Press any key to continue...")
		return m_pRunner->OnWaiting();

	if (msg[0] == '#')
		return HandleClientNotification(msg);

	Severity::type severity = Severity::Info;

	static const std::regex reError("\\): (fatal )?error ");
	std::smatch sm;
	if (std::regex_search(msg, sm, reError))
		severity = sm[1].matched? Severity::Fatal: Severity::Error;

	static const std::regex reAssertion("Assertion failed:");
	if (std::regex_search(msg, reAssertion))
		severity =  Severity::Assertion;

	m_pObserver->test_message(severity, msg);
}

} // namespace BoostTest2
} // namespace gj
