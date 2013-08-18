//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the boost test library home page.

#include "stdafx.h"
#include "TestStateStorage.h"

namespace gj {

void TestStateStorage::Clear()
{
	m_tests.clear();
}

void TestStateStorage::SaveState(const TestUnit& tu, bool enable)
{
	m_tests[tu.fullName] = enable;
}

void TestStateStorage::RestoreState(TestUnit& tu) const
{
	auto it = m_tests.find(tu.fullName);
	if (it != m_tests.end())
		tu.enabled = it->second;
}

} // namespace gj
