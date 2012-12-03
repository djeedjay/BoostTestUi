//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the boost test library home page.

#include "stdafx.h"
#include <stdexcept>
#include <boost/filesystem.hpp>
#include "Utilities.h"
#include "hstream.h"
#include "GetUnitTestType.h"
#include "BoostTest.h"
#include "GoogleTest.h"
#include "ExeRunner.h"

namespace gj {

void ExeRunner::Load()
{
	Process proc(m_fileName, m_pArgBuilder->GetListArg());

	hstream hs(proc.GetStdOut());
	m_tree.children.clear();
	m_pArgBuilder->LoadTestUnits(m_tree, hs, m_processName);
	if (m_tree.children.empty())
		throw std::runtime_error("No test cases");
}

std::unique_ptr<ArgumentBuilder> CreateArgumentBuilder(const std::wstring& fileName, ExeRunner& runner, TestObserver& observer)
{
	std::string type = GetUnitTestType(WideCharToMultiByte(fileName));
	if (type == "boost")
		return std::unique_ptr<ArgumentBuilder>(new BoostTest::ArgumentBuilder(runner, observer));
	if (type == "google")
		return std::unique_ptr<ArgumentBuilder>(new GoogleTest::ArgumentBuilder(runner, observer));
	throw std::runtime_error("This is not a boost.test executable");
}

ExeRunner::ExeRunner(const std::wstring& fileName, TestObserver& observer) :
	m_fileName(fileName),
	m_processName(boost::filesystem::path(WideCharToMultiByte(fileName)).filename()),
	m_pObserver(&observer),
	m_tree(TestUnit(0, TestUnit::TestSuite, "root")),
	m_pArgBuilder(CreateArgumentBuilder(fileName, *this, observer))
{
	Load();
}

ExeRunner::~ExeRunner()
{
	Abort();
	Wait();
}

void TraverseTestTreeNode(TestUnitNode& node, TestTreeVisitor& v)
{
	if (node.data.type == TestUnit::TestCase)
	{
		v.VisitTestCase(static_cast<TestCase&>(node.data));
	}
	else
	{
		v.EnterTestSuite(static_cast<TestSuite&>(node.data));
		for (auto it = node.children.begin(); it != node.children.end(); ++it)
			TraverseTestTreeNode(*it, v);
		v.LeaveTestSuite();
	}
}

TestUnitNode& ExeRunner::RootTestUnitNode()
{
	return m_tree.children.front();
}

TestSuite& ExeRunner::RootTestSuite()
{
	return static_cast<TestSuite&>(m_tree.children.front().data);
}

void ExeRunner::TraverseTestTree(TestTreeVisitor& v)
{
	TraverseTestTreeNode(m_tree.children.front(), v);
}

void ExeRunner::TraverseTestTree(unsigned id, TestTreeVisitor& v)
{
	TraverseTestTreeNode(GetTestUnitNode(id), v);
}

void ExeRunner::EnableTestUnit(unsigned id, bool enable)
{
	GetTestUnit(id).enabled = enable;
}

bool ExeRunner::IsRunning() const
{
	return m_pThread.get() != nullptr;
}

void ExeRunner::Run(int logLevel, unsigned options)
{
	if (m_pThread)
		return;

	m_pProcess.reset(new Process(m_fileName, m_pArgBuilder->BuildArgs(*this, logLevel, options)));
	m_pObserver->test_message(Severity::Info, stringbuilder() << "Process " << m_pProcess->GetProcessId() << ": " << m_processName << ", started");
	m_pThread.reset(new boost::thread([this]() { RunTest(); }));
}

void ExeRunner::Continue()
{
	if (!m_pProcess)
		return;

	hstream hs(m_pProcess->GetStdIn());
	hs.put('\n');
}

void ExeRunner::Abort()
{
	if (!m_pThread)
		return;

	TerminateProcess(m_pProcess->GetProcessHandle(), static_cast<unsigned>(-1));
}

void ExeRunner::Wait()
{
	if (!m_pThread)
		return;

	unsigned id = m_pProcess->GetProcessId();
	m_pThread->join();
	m_pProcess.reset();
	m_pThread.reset();
	m_pObserver->test_message(Severity::Info, stringbuilder() << "Process " << id << ": " << m_processName << ", finished");
}

void ExeRunner::OnWaiting()
{
	m_pObserver->test_waiting(m_processName, m_pProcess->GetProcessId());
}

TestUnitNode* GetTestUnitNodePtr(TestUnitNode& node, unsigned id)
{
	if (node.data.id == id)
		return &node;

	for (auto it = node.children.begin(); it != node.children.end(); ++it)
	{
		if (auto p = GetTestUnitNodePtr(*it, id))
			return p;
	}
	return nullptr;
}

TestUnitNode& ExeRunner::GetTestUnitNode(unsigned id)
{
	if (auto p = GetTestUnitNodePtr(m_tree, id))
		return *p;
	throw std::invalid_argument("invalid TestUnit id");
}

TestUnit& ExeRunner::GetTestUnit(unsigned id)
{
	return GetTestUnitNode(id).data;
}

void ExeRunner::RunTest()
try
{
	hstream hs(m_pProcess->GetStdOut());
	std::string line;
	while (std::getline(hs, line))
	{
		m_pArgBuilder->FilterMessage(line);
	}

	m_pObserver->TestFinished();
}
catch (std::exception& e)
{
	m_pObserver->exception_caught(e.what());
	m_pObserver->TestFinished();
}

} // namespace gj
