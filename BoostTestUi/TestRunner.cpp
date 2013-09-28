//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://boosttestui.wordpress.com/ for the boosttestui home page.

#include "stdafx.h"
#include "TestRunner.h"

namespace gj {

void TestTreeVisitor::VisitTestCase(TestCase&)
{
}

void TestTreeVisitor::EnterTestSuite(TestSuite&)
{
}

void TestTreeVisitor::LeaveTestSuite()
{
}

TestTreeVisitor::~TestTreeVisitor()
{
}

TestObserver::~TestObserver()
{
}

TestRunner::~TestRunner()
{
}

ArgumentBuilder::~ArgumentBuilder()
{
}

void ArgumentBuilder::FilterMessage(const std::string& /*msg*/)
{
}

} // namespace gj
