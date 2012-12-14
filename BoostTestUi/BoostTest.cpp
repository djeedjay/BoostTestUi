//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the boost test library home page.

#include "stdafx.h"
#include <sstream>
#include <regex>
#include "ExeRunner.h"
#include "BoostTest.h"

namespace gj {
namespace BoostTest {

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
		return m_suites[1].all;
	}

	std::string GetArg() const
	{
		std::string arg;
		const Suite& suite = m_suites[1];
		for (auto it = suite.cases.begin(); it != suite.cases.end(); ++it)
		{
			if (!arg.empty())
				arg += ',';
			arg += *it;
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


ArgumentBuilder::ArgumentBuilder(ExeRunner& runner, TestObserver& observer) :
	m_pRunner(&runner),
	m_pObserver(&observer)
{
}

std::wstring ArgumentBuilder::GetListArg()
{
	return L"--gui_list";
}

std::string chomp(std::string s)
{
	size_t n = s.size();
	while (n > 0 && s[n - 1] < ' ')
		--n;
	s.resize(n);
	return s;
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
T get_arg(std::istream& is)
{
	T value = T();
	is >> value;
	return value;
}

std::string get_name(std::istream& is)
{
	std::string name;
	while (is)
	{
		int c = is.get();
		if (c < ' ')
			break;
		name += static_cast<char>(c);
	}
	return name;
}

std::string LoadTestUnits(TestUnitNode& node, std::istream& is, int indent = 0)
{
	std::string line;
	std::getline(is, line);
	while (is)
	{
		line = chomp(line);
		int lineIndent = static_cast<int>(line.find_first_not_of(' '));
		if (lineIndent < 0)
		{
			std::getline(is, line);
			continue;
		}
		if (lineIndent < indent)
			return line;

		std::istringstream ss(line.substr(lineIndent));
		TestUnit::Type type;
		bool enable = true;
		switch (get_arg<char>(ss))
		{
		case 'c': enable = false;
		case 'C': type = TestUnit::TestCase; break;
		case 's': enable = false;
		case 'S': type = TestUnit::TestSuite; break;
		default: return "";
		}
		unsigned id = get_arg<unsigned>(ss);
		if (get_arg<char>(ss) != ':')
		{
			std::getline(is, line);
			continue;
		}

		node.children.push_back(TestUnit(id, type, normalize_type(get_name(ss)), enable));
		if (type == TestUnit::TestSuite)
			line = LoadTestUnits(node.children.back(), is, lineIndent + 1);
		else
			std::getline(is, line);
	}
	return "";
}

void ArgumentBuilder::LoadTestUnits(TestUnitNode& tree, std::istream& is, const std::string&)
{
	BoostTest::LoadTestUnits(tree, is);
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
		args << L" --gui_wait";

/* Cannot enable disabled test cases:
	GetEnableArg2 getArg2;
	runner.TraverseTestTree(getArg2);
	if (!getArg2.AllCases())
		m_pObserver->test_message(Severity::Info, getArg2.GetArg());
*/
	GetEnableArg getArg;
	runner.TraverseTestTree(getArg);
	args << L" --gui_run=" << getArg.GetArg();
	return args.str();
}

void ArgumentBuilder::HandleClientNotification(const std::string& line)
{
	std::istringstream ss(line);
	char c;
	std::string command;
	ss >> c >> command;

	if (command == "start")
		m_pObserver->test_iteration_start(get_arg<unsigned>(ss));
	else if (command == "finish")
		m_pObserver->test_iteration_finish();
	else if (command == "aborted")
		m_pObserver->test_aborted();
	else if (command == "unit_start")
	{
		if (auto p = m_pRunner->GetTestUnitPtr(get_arg<unsigned>(ss)))
		{
			if (p->type == TestUnit::TestCase)
				m_pRunner->OnTestCaseStart(p->id);
			else
				m_pRunner->OnTestSuiteStart(p->id);
		}
	}
	else if (command == "unit_finish")
	{
		unsigned id = get_arg<unsigned>(ss);
		unsigned elapsed = get_arg<unsigned>(ss);
		if (auto p = m_pRunner->GetTestUnitPtr(id))
		{
			if (p->type == TestUnit::TestCase)
				m_pRunner->OnTestCaseFinish(p->id, elapsed);
			else
				m_pRunner->OnTestSuiteFinish(p->id, elapsed);
		}
	}
	else if (command == "unit_skipped")
		m_pRunner->OnTestUnitSkipped(get_arg<unsigned>(ss));
	else if (command == "unit_aborted")
		m_pRunner->OnTestUnitAborted(get_arg<unsigned>(ss));
	else if (command == "assertion")
		m_pRunner->OnTestAssertion(get_arg<bool>(ss));
	else if (command == "exception")
		m_pRunner->OnTestExceptionCaught(get_arg<std::string>(ss));
	else if (command == "waiting")
		m_pRunner->OnWaiting();
	else
		m_pObserver->test_message(Severity::Info, line);
}

void ArgumentBuilder::FilterMessage(const std::string& msg)
{
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

} // namespace BoostTest
} // namespace gj
