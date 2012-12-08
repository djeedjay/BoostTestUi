//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the boost test library home page.

#ifndef BOOST_TESTUI_TESTRUNNER_H
#define BOOST_TESTUI_TESTRUNNER_H

#pragma once

#include <string>
#include <vector>
#include "Severity.h"

namespace gj {

struct TestUnit
{
	enum Type { TestSuite, TestCase };

	TestUnit(unsigned id, Type type, const std::string& name, bool enabled = true) :
		id(id), type(type), name(name), enabled(enabled)
	{
	}

	unsigned id;
	Type type;
	std::string name;
	bool enabled;
};

struct TestUnitNode
{
	TestUnitNode(const TestUnit& data) :
		data(data)
	{
	}

	TestUnit data;
	std::vector<TestUnitNode> children;
};

struct TestSuite : TestUnit
{
	TestSuite(unsigned id, const std::string& name, bool enabled = true) :
		TestUnit(id, TestUnit::TestSuite, name, enabled)
	{
	}
};

struct TestCase : TestUnit
{
	TestCase(unsigned id, const std::string& name, bool enabled = true) :
		TestUnit(id, TestUnit::TestCase, name, enabled)
	{
	}
};

class TestTreeVisitor
{
public:
	virtual void VisitTestCase(TestCase& tc);
	virtual void EnterTestSuite(TestSuite& ts);
	virtual void LeaveTestSuite();

	virtual ~TestTreeVisitor();
};

class TestObserver
{
public:
	virtual void test_message(Severity::type severity, const std::string& msg) = 0;

	virtual void test_waiting(const std::wstring& processName, unsigned processId) = 0;
	virtual void test_start() = 0;
	virtual void test_finish() = 0;
	virtual void test_aborted() = 0;
	virtual void test_iteration_start(unsigned test_cases_amount) = 0;
	virtual void test_iteration_finish() = 0;
	virtual void test_unit_start(const TestUnit& tu) = 0;
	virtual void test_unit_finish(const TestUnit& tu, unsigned long elapsed) = 0;
	virtual void test_unit_skipped(const TestUnit& tu) = 0;
	virtual void test_unit_aborted(const TestUnit& tu) = 0;

	virtual void assertion_result(bool passed) = 0;
	virtual void exception_caught(const std::string& what) = 0;

	virtual void TestFinished() = 0;

protected:
	~TestObserver();
};

struct TestRunner
{
	enum Options
	{
		Randomize = 1 << 0,
		WaitForDebugger = 1 << 1,
		Repeat = 1 << 2,
	};
	virtual TestSuite& RootTestSuite() = 0;
	virtual void TraverseTestTree(TestTreeVisitor& v) = 0;
	virtual void TraverseTestTree(unsigned id, TestTreeVisitor& v) = 0;

	virtual void EnableTestUnit(unsigned id, bool enable) = 0;

	virtual bool IsRunning() const = 0;
	virtual void Run(int logLevel, unsigned options) = 0;
	virtual void Continue() = 0;
	virtual void Abort() = 0;
	virtual void Wait() = 0;

	virtual ~TestRunner();
};

struct ArgumentBuilder
{
	virtual std::wstring GetListArg() = 0;
	virtual void LoadTestUnits(TestUnitNode& node, std::istream& is, const std::string& testName) = 0;

	virtual std::wstring BuildArgs(TestRunner& runner, int logLevel, unsigned& options) = 0;
	virtual void FilterMessage(const std::string& msg);

	virtual ~ArgumentBuilder();
};

} // namespace gj

#endif // BOOST_TESTUI_TESTRUNNER_H
