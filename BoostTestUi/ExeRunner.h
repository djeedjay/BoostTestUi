// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#ifndef BOOST_TESTUI_EXERUNNER_H
#define BOOST_TESTUI_EXERUNNER_H

#pragma once

#pragma warning(disable: 4481) // nonstandard extension used: override specifier 'override'

#pragma warning(push, 3) // conversion from 'int' to 'unsigned short', possible loss of data
#include <boost/thread.hpp>
#pragma warning(pop)
#include <boost/noncopyable.hpp>
#include "TestRunner.h"
#include "Process.h"

namespace gj {

struct UnitTestType
{
	enum type { Boost, Catch, Google, NUnit };
};

class NoHeaderError : public std::runtime_error
{
public:
	NoHeaderError(const char* msg, UnitTestType::type idHelp);
	UnitTestType::type GetUnitTestType() const;

private:
	UnitTestType::type m_testType;
};

class ExeRunner :
	boost::noncopyable,
	public TestRunner
{
public:
	ExeRunner(const std::wstring& fileName, TestObserver& observer);
	virtual ~ExeRunner();

	virtual TestSuite& RootTestSuite() override;
	virtual void TraverseTestTree(TestTreeVisitor& v) override;
	virtual void TraverseTestTree(unsigned id, TestTreeVisitor& v) override;

	virtual void EnableTestUnit(unsigned id, bool enable) override;

	virtual unsigned GetEnabledOptions(unsigned options);
	virtual bool IsRunning() const override;
	virtual void SetRepeat(bool repeat) override;
	virtual std::wstring GetCommand(int logLevel, unsigned options, const std::wstring& arguments) override;
	virtual void Run(int logLevel, unsigned options, const std::wstring& arguments) override;
	virtual void Continue() override;
	virtual void Abort() override;
	virtual void Wait() override;

	void OnWaiting();
	void OnTestIterationStart(unsigned count);
	void OnTestSuiteStart(unsigned id);
	void OnTestCaseStart(unsigned id);
	void OnTestAssertion(bool result);
	void OnTestExceptionCaught(const std::string& what);
	void OnTestCaseFinish(unsigned id, unsigned elapsed);
	void OnTestCaseFinish(unsigned id, unsigned elapsed, TestCaseState::type state);
	void OnTestSuiteFinish(unsigned id, unsigned elapsed);
	void OnTestUnitSkipped(unsigned id);
	void OnTestUnitAborted(unsigned id);
	void OnTestIterationFinish();

	virtual TestUnit& GetTestUnit(unsigned id);
	virtual TestUnit* GetTestUnitPtr(unsigned id);

private:
	TestUnitNode& RootTestUnitNode();
	TestUnitNode& GetTestUnitNode(unsigned id);
	void Load();
	void HandleClientNotification(const std::string& line);
	void RunTest();
	void RunTestIteration();
	void StartTestProcess();
	void WaitForTestProcess();

	std::wstring m_fileName;
	std::wstring m_testArgs;
	bool m_repeat;
	TestObserver* m_pObserver;
	TestUnitNode m_tree;
	std::unique_ptr<ArgumentBuilder> m_pArgBuilder;
	std::unique_ptr<Process> m_pProcess;
	bool m_testFinished;
	std::unique_ptr<boost::thread> m_pThread;
	HANDLE m_hStdin;
	HANDLE m_hProcess;
};

} // namespace gj

#endif // BOOST_TESTUI_EXERUNNER_H
