// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#ifndef BOOST_TESTUI_CATCHTEST_H
#define BOOST_TESTUI_CATCHTEST_H

#pragma once

#pragma warning(disable: 4481) // nonstandard extension used: override specifier 'override'

#include "TestRunner.h"

namespace gj {
namespace CatchTest {

class ArgumentBuilder : public gj::ArgumentBuilder
{
public:
	explicit ArgumentBuilder(const std::wstring& fileName, ExeRunner& runner, TestObserver& observer);

	virtual std::wstring GetExePathName() override;
	virtual std::wstring GetListArg() override;
	virtual void LoadTestUnits(TestUnitNode& tree, std::istream& is, const std::string& testName) override;

	unsigned GetEnabledOptions(unsigned options) const override;
	virtual std::wstring BuildArgs(TestRunner& runner, int logLevel, unsigned& options) override;
	virtual void FilterMessage(const std::string& msg) override;

private:
	unsigned GetId(const std::string& name);
	void HandleClientNotification(const std::string& line);

	std::wstring m_fileName;
	ExeRunner* m_pRunner;
	TestObserver* m_pObserver;
	unsigned m_rootId;
	std::map<std::string, unsigned> m_ids;
};

} // namespace CatchTest
} // namespace gj

#endif // BOOST_TESTUI_CATCHTEST_H
