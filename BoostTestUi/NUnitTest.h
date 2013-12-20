// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#ifndef BOOST_TESTUI_NUNITTEST_H
#define BOOST_TESTUI_NUNITTEST_H

#pragma once

#pragma warning(disable: 4481) // nonstandard extension used: override specifier 'override'

#include "ExeRunner.h"

namespace gj {
namespace NUnitTest {

class ArgumentBuilder : public gj::ArgumentBuilder
{
public:
	ArgumentBuilder(const std::wstring& exeName, const std::wstring& fileName, ExeRunner& runner, TestObserver& observer);

	virtual std::wstring GetExePathName() override;
	virtual std::wstring GetListArg() override;
	virtual void LoadTestUnits(TestUnitNode& node, std::istream& is, const std::string& testName) override;

	virtual unsigned GetEnabledOptions(unsigned options) const override;
	virtual std::wstring BuildArgs(TestRunner& runner, int logLevel, unsigned& options) override;
	virtual void FilterMessage(const std::string& msg) override;

private:
	void HandleClientNotification(const std::string& line);

	std::wstring m_exeName;
	std::wstring m_fileName;
	ExeRunner* m_pRunner;
	TestObserver* m_pObserver;
	bool m_exception;
};

} // namespace NUnitTest
} // namespace gj

#endif // BOOST_TESTUI_NUNITTEST_H
