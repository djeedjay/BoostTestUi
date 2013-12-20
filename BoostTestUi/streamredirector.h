// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#ifndef BOOST_TESTUI_STREAMREDIRECTOR_H
#define BOOST_TESTUI_STREAMREDIRECTOR_H

#pragma once

#include <streambuf>
#include <string>
#include "windows.h"


namespace gj {

template <class StringT>
struct StreamSink
{
	virtual void Send(const StringT& msg) = 0;
};

template <class Elem, class Tr = std::char_traits<Elem>, class Alloc = std::allocator<Elem> >
class basic_redirectorbuf : public std::basic_streambuf<Elem, Tr>
{
public:
	typedef typename std::basic_string<Elem, Tr, Alloc> string_type;
	typedef typename StreamSink<string_type> sink_type;

	explicit basic_redirectorbuf(sink_type& sink) :
		m_pSink(&sink)
	{
	}

protected:
	int sync()
	{
		if (!m_buf.empty())
		{
			m_pSink->Send(m_buf);
			m_buf.clear();
		}
		return 0;
	}

	int_type overflow(int_type c)
	{
		if (c == traits_type::eof())
			return 0;

		m_buf += traits_type::to_char_type(c);
		if (c == '\n')
			sync();
		return c;
	}

private:
	sink_type* m_pSink;
	string_type m_buf;
};

template <class Elem, class Tr = std::char_traits<Elem>, class Alloc = std::allocator<Elem> >
class basic_streamredirector : public std::basic_ostream<Elem, Tr>
{
public:
	typedef typename std::basic_string<Elem, Tr, Alloc> string_type;
	typedef typename StreamSink<string_type> sink_type;

	explicit basic_streamredirector(sink_type& sink) :
		std::basic_ostream<Elem, Tr>(&buf),
		buf(sink)
	{
	}

private:
	basic_redirectorbuf<Elem, Tr> buf;
};

} // namespace gj

#endif // BOOST_TESTUI_STREAMREDIRECTOR_H
