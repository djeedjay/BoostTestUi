// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#include "stdafx.h"
#include <regex>
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

LineNumberInfo::LineNumberInfo() :
	m_valid(false)
{
}

LineNumberInfo::LineNumberInfo(std::string filename, int lineNumber) :
	m_valid(true),
	m_filename(filename),
	m_lineNumber(lineNumber)
{
}

LineNumberInfo::operator bool() const
{
	return m_valid;
}

std::string LineNumberInfo::Filename() const
{
	return m_filename;
}

int LineNumberInfo::LineNumber() const
{
	return m_lineNumber;
}

TestRunner::~TestRunner()
{
}

ArgumentBuilder::~ArgumentBuilder()
{
}

std::wstring ArgumentBuilder::BuildPublicArgs(TestRunner& runner, int logLevel, unsigned options)
{
	return BuildArgs(runner, logLevel, options);
}

void ArgumentBuilder::FilterMessage(const std::string& /*msg*/)
{
}

LineNumberInfo ArgumentBuilder::FindLineNumberInfo(const std::string& s) const
{
	static const std::regex re1("(.+)\\((\\d+)\\).*");
	static const std::regex re2("file (.+), line (\\d+)");
	static const std::regex re3(" in (.+):line (\\d+)");

	std::smatch sm;
	if (std::regex_match(s, sm, re1) ||
		std::regex_search(s, sm, re2) ||
		std::regex_search(s, sm, re3))
		return LineNumberInfo(sm[1], std::stoi(sm[2]));
	return LineNumberInfo();
}

} // namespace gj

