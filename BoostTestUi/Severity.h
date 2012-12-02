//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the boost test library home page.

#ifndef BOOST_TESTUI_SEVERITY_H
#define BOOST_TESTUI_SEVERITY_H

#pragma once

namespace gj {

struct Severity
{
	enum type
	{
		Info = 0,
		Error,
		Fatal,
		Assertion
	};
};

} // namespace gj

#endif // BOOST_TESTUI_SEVERITY_H
