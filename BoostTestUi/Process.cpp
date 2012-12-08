//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the boost test library home page.

#include "stdafx.h"
#include <string>
#include <iostream>
#include <vector>
#include <boost/filesystem.hpp>
#include "Utilities.h"
#include "Process.h"

namespace gj {

template <class Elem, class Tr = std::char_traits<Elem>, class Alloc = std::allocator<Elem> >
class basic_handlebuf : public std::basic_streambuf<Elem, Tr>
{
public:
	basic_handlebuf(HANDLE handle, std::size_t buff_sz = 256, std::size_t put_back = 8) :
		m_handle(handle),
		m_put_back(std::max<std::size_t>(put_back, 1)),
		m_buffer(std::max(buff_sz, m_put_back) + m_put_back)
	{
		Elem* end = &m_buffer.front() + m_buffer.size();
		setg(end, end, end);
	}

protected:
	int_type underflow()
	{
		if (gptr() < egptr()) // buffer not exhausted
			return traits_type::to_int_type(*gptr());

		Elem* base = &m_buffer.front();
		Elem* start = base;

		if (eback() == base) // true when this isn't the first fill
		{
			// Make arrangements for putback characters
			std::memmove(base, egptr() - m_put_back, m_put_back);
			start += m_put_back;
		}

		// start is now the start of the buffer, proper.
		// Read from fptr_ in to the provided buffer
		DWORD read;
		if (!ReadFile(m_handle, start, (m_buffer.size() - (start - base))*sizeof(Elem), &read, nullptr) || read == 0)
			return traits_type::eof();

		// Set buffer pointers
		setg(base, start, start + read/sizeof(Elem));

		return traits_type::to_int_type(*gptr());
	}

private:
	std::basic_string<Elem, Tr, Alloc> m_buf;

	HANDLE m_handle;
	const std::size_t m_put_back;
	std::vector<Elem> m_buffer;
};

template <class Elem, class Tr = std::char_traits<Elem> >
class basic_handlestream : public std::basic_istream<Elem, Tr>
{
public:
	basic_handlestream(HANDLE handle) :
		std::basic_istream<Elem, Tr>(&m_buf),
		m_buf(handle)
	{
	}

private:
	basic_handlebuf<Elem, Tr> m_buf;
};

typedef basic_handlestream<char> hstream;
typedef basic_handlestream<wchar_t> whstream;

Process::Process(const std::wstring& pathName, const std::vector<std::wstring>& args)
{
	std::wstring commandLine;
	auto it = args.begin();
	while (it != args.end())
	{
		if (it != args.begin())
			commandLine += L" ";
		commandLine += *it;
		++it;
	}

	Run(pathName, commandLine);
}

Process::Process(const std::wstring& pathName, const std::wstring& args)
{
	Run(pathName, args);
}

void Process::Run(const std::wstring& pathName, const std::wstring& args)
{
	m_name = boost::filesystem::wpath(pathName).filename();

	std::wstring commandLine;
	commandLine += L"\"";
	commandLine += pathName;
	commandLine += L"\"";

	if (!args.empty())
	{
		commandLine += L" ";
		commandLine += args;
	}

   	SECURITY_ATTRIBUTES saAttr; 
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = true; 
	saAttr.lpSecurityDescriptor = nullptr; 

	HANDLE stdInRd, stdInWr;
	if (!CreatePipe(&stdInRd, &stdInWr, &saAttr, 0))
		ThrowLastError("CreatePipe");
	CHandle stdInRd2(stdInRd);
	m_stdIn.Attach(stdInWr);
	if (!SetHandleInformation(stdInWr, HANDLE_FLAG_INHERIT, 0))
		ThrowLastError("SetHandleInformation");

	HANDLE stdOutRd, stdOutWr;
	if (!CreatePipe(&stdOutRd, &stdOutWr, &saAttr, 0))
		ThrowLastError("CreatePipe");
	CHandle stdOutWr2(stdOutWr);
	m_stdOut.Attach(stdOutRd);
	if (!SetHandleInformation(stdOutRd, HANDLE_FLAG_INHERIT, 0))
		ThrowLastError("SetHandleInformation");

	STARTUPINFO startupInfo;
	startupInfo.cb = sizeof(startupInfo);
	startupInfo.lpReserved = nullptr;
	startupInfo.lpDesktop = nullptr;
	startupInfo.lpTitle = nullptr;
	startupInfo.dwX = 0;
	startupInfo.dwY = 0;
	startupInfo.dwXSize = 0;
	startupInfo.dwYSize = 0;
	startupInfo.dwXCountChars = 0;
	startupInfo.dwYCountChars = 0;
	startupInfo.dwFillAttribute = 0;
	startupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	startupInfo.wShowWindow = SW_HIDE;
	startupInfo.cbReserved2 = 0;
	startupInfo.lpReserved2 = nullptr;
	startupInfo.hStdInput = stdInRd2;
	startupInfo.hStdOutput = 
	startupInfo.hStdError = stdOutWr2;

	PROCESS_INFORMATION processInformation;

	if (!CreateProcess(
		nullptr,
		const_cast<wchar_t*>(commandLine.c_str()),
		nullptr,
		nullptr,
		true,
		0,
		nullptr,
		nullptr,
		&startupInfo,
		&processInformation))
		ThrowLastError("SetHandleInformation");

	m_hProcess.Attach(processInformation.hProcess);
	m_hThread.Attach(processInformation.hThread);
	m_processId = processInformation.dwProcessId;
	m_threadId = processInformation.dwThreadId;
}

std::wstring Process::GetName() const
{
	return m_name;
}

HANDLE Process::GetStdIn() const
{
	return m_stdIn;
}

HANDLE Process::GetStdOut() const
{
	return m_stdOut;
}

HANDLE Process::GetProcessHandle() const
{
	return m_hProcess;
}

HANDLE Process::GetThreadHandle() const
{
	return m_hThread;
}

unsigned Process::GetProcessId() const
{
	return m_processId;
}

unsigned Process::GetThreadId() const
{
	return m_threadId;
}

bool Process::IsRunning() const
{
	DWORD exitCode;
	if (!GetExitCodeProcess(m_hProcess, &exitCode))
		ThrowLastError("process exit");

	return exitCode == STILL_ACTIVE;
}

void Process::Wait() const
{
	if (WaitForSingleObject(m_hProcess, INFINITE) != WAIT_OBJECT_0)
		ThrowLastError("process exit");
}

} // namespace gj
