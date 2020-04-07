// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#ifndef BOOST_TESTUI_BOOSTTEST2_H
#define BOOST_TESTUI_BOOSTTEST2_H

#pragma once

#include "ExeRunner.h"

namespace gj {
namespace BoostTest2 {

class ArgumentBuilder : public gj::ArgumentBuilder
{
public:
	ArgumentBuilder(const std::wstring& fileName, ExeRunner& runner, TestObserver& observer);

	std::wstring GetExePathName() override;
	std::wstring GetListArg() override;
	void LoadTestUnits(TestUnitNode& node, std::istream& is, const std::string& testName) override;

	unsigned GetEnabledOptions(unsigned options) const override;
	std::wstring BuildArgs(TestRunner& runner, int logLevel, unsigned& options) override;
	void FilterMessage(const std::string& msg) override;

private:
	void HandleClientNotification(const std::string& line);

	std::wstring m_fileName;
	ExeRunner* m_pRunner;
	TestObserver* m_pObserver;
};

} // namespace BoostTest2
} // namespace gj

#endif // BOOST_TESTUI_BOOSTTEST2_H
