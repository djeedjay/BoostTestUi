//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the boost test library home page.

#ifndef BOOST_TESTUI_UTILITIES_H
#define BOOST_TESTUI_UTILITIES_H

#pragma once

#include <sstream>
#include <boost/noncopyable.hpp>

// Alternative to ATL standard BEGIN_MSG_MAP() with try block:
#define BEGIN_MSG_MAP_TRY(theClass) \
	BOOL theClass::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID) \
	try \
	{ \
		BOOL bHandled = TRUE; \
		(hWnd); \
		(uMsg); \
		(wParam); \
		(lParam); \
		(lResult); \
		(bHandled); \
		switch(dwMsgMapID) \
		{ \
		case 0:

#define END_MSG_MAP_CATCH(handler) \
			break; \
		default: \
			ATLTRACE(ATL::atlTraceWindowing, 0, _T("Invalid message map ID (%i)\n"), dwMsgMapID); \
			ATLASSERT(FALSE); \
			break; \
		} \
		return FALSE; \
		} \
		catch (...) \
		{ \
			handler(); \
			return FALSE; \
		}

namespace gj {

std::string chomp(std::string s);
std::wstring chomp(std::wstring s);
std::wstring LoadString(int id);

std::wstring MultiByteToWideChar(const std::string& str);
std::string WideCharToMultiByte(const std::wstring& str);

class Str
{
public:
	explicit Str(const std::string& s) :
		m_str()
	{
	}

	explicit Str(const std::wstring& s) :
		m_str(WideCharToMultiByte(s))
	{
	}

	operator std::string() const
	{
		return m_str;
	}

	operator const char*() const
	{
		return m_str.c_str();
	}

private:
	std::string m_str;
};

class WStr
{
public:
	explicit WStr(const std::string& s) :
		m_str(MultiByteToWideChar(s))
	{
	}

	explicit WStr(const std::wstring& s) :
		m_str(s)
	{
	}

	operator std::wstring() const
	{
		return m_str;
	}

	operator const wchar_t*() const
	{
		return m_str.c_str();
	}

private:
	std::wstring m_str;
};

class Timer
{
public:
	Timer();

	void Reset();
	double Get() const;

private:
	long long GetTicks() const;

	double m_timerUnit;
	long long m_offset;
};

class ScopedCursor : boost::noncopyable
{
public:
	explicit ScopedCursor(HCURSOR cursor) : m_cursor(SetCursor(cursor))
	{
	}

	~ScopedCursor()
	{
		SetCursor(m_cursor);
	}

private:
	HCURSOR m_cursor;
};

void ThrowWin32Error(DWORD error, const std::string& what = "");
void ThrowLastError(const std::string& what);
void ThrowLastError(const std::wstring& what);
void CheckHr(HRESULT hr);

template <typename F>
class scope_guard : boost::noncopyable
{
public: 
	explicit scope_guard(const F& x) :
		m_action(x),
		m_released(false)
	{
	}

	void release()
	{
		m_released = true;
	}

	~scope_guard()
	{
		if (m_released)
			return;

		// Catch any exceptions and continue during guard clean up
		try
		{
			m_action();
		}
		catch (...)
		{
		}
	}

private:
	F m_action;
	bool m_released;
};

template <typename F>
scope_guard<F> make_guard(F f)
{
	return scope_guard<F>(f);
}

template <class CharType, class Traits = std::char_traits<CharType>, class Allocator = std::allocator<CharType>>
class basic_stringbuilder
{
public:
	typedef std::basic_string<CharType, Traits, Allocator> string_type;

	template <typename T>
	basic_stringbuilder& operator<<(const T& t)
	{
		m_ss << t;
		return *this;
	}

	string_type str() const
	{
		return m_ss.str();
	}

	const CharType* c_str() const
	{
		return m_ss.str().c_str();
	}

	operator string_type() const
	{
		return m_ss.str();
	}

private:
	std::basic_ostringstream<CharType, Traits, Allocator> m_ss;
};

typedef basic_stringbuilder<char> stringbuilder;
typedef basic_stringbuilder<wchar_t> wstringbuilder;

std::wstring GetExceptionMessage();

DWORD SetRichEditData(CRichEditCtrl& ctrl, DWORD format, const BYTE* pData, size_t len);
DWORD SetRichEditData(CRichEditCtrl& ctrl, DWORD format, LPWSTR resourcedId);

} // namespace gj

#endif // BOOST_TESTUI_UTILITIES_H
