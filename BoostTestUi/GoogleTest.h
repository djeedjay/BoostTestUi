//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the boost test library home page.

#ifndef BOOST_TESTUI_GOOGLETEST_H
#define BOOST_TESTUI_GOOGLETEST_H

#pragma once

#include "TestRunner.h"

namespace gj {
namespace GoogleTest {

class ArgumentBuilder : public gj::ArgumentBuilder
{
public:
	explicit ArgumentBuilder(ExeRunner& runner, TestObserver& observer);

	virtual std::wstring GetListArg();
	virtual void LoadTestUnits(TestUnitNode& tree, std::istream& is, const std::string& testName);

	virtual std::wstring BuildArgs(TestRunner& runner, int logLevel, unsigned options);
	virtual void FilterMessage(const std::string& msg);

private:
	unsigned GetId(const std::string& name);

	ExeRunner* m_pRunner;
	TestObserver* m_pObserver;
	std::map<std::string, unsigned> m_ids;
};

} // namespace GoogleTest
} // namespace gj

#endif // BOOST_TESTUI_GOOGLETEST_H
