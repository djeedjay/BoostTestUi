//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the boost test library home page.

#ifndef BOOST_TESTUI_EXERUNNER_H
#define BOOST_TESTUI_EXERUNNER_H

#pragma once

#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include "TestRunner.h"
#include "Process.h"

namespace gj {

class ExeRunner :
	boost::noncopyable,
	public TestRunner
{
public:
	ExeRunner(const std::wstring& fileName, TestObserver& observer);
	virtual ~ExeRunner();

	virtual TestSuite& RootTestSuite();
	virtual void TraverseTestTree(TestTreeVisitor& v);
	virtual void TraverseTestTree(unsigned id, TestTreeVisitor& v);

	virtual void EnableTestUnit(unsigned id, bool enable);

	virtual bool IsRunning() const;
	virtual void Run(int logLevel, unsigned options);
	virtual void Continue();
	virtual void Abort();
	virtual void Wait();

	void OnWaiting();
	TestUnit& GetTestUnit(unsigned id);

private:
	TestUnitNode& RootTestUnitNode();
	TestUnitNode& GetTestUnitNode(unsigned id);
	void Load();
	void HandleClientNotification(const std::string& line);
	void RunTest();

	std::wstring m_fileName;
	std::string m_processName;
	TestObserver* m_pObserver;
	TestUnitNode m_tree;
	std::unique_ptr<ArgumentBuilder> m_pArgBuilder;
	std::unique_ptr<Process> m_pProcess;
	std::unique_ptr<boost::thread> m_pThread;
};

} // namespace gj

#endif // BOOST_TESTUI_EXERUNNER_H
