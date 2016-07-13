// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#include "stdafx.h"
#include "DevEnv.h"

namespace {

template <typename Itf>
CComPtr<Itf> GetActiveObject(const CLSID& clsid)
{
	CComPtr<IUnknown> pIUnk;
	HRESULT hr = ::GetActiveObject(clsid, nullptr, &pIUnk);
	CComPtr<Itf> pItf;
	if (SUCCEEDED(hr))
		hr = pIUnk->QueryInterface(&pItf);
	return pItf;
}

CLSID ClsidFromProgId(const std::wstring& progId)
{
	CLSID clsid = CLSID_NULL;
	CLSIDFromProgID(progId.c_str(), &clsid);
	return clsid;
}

} // namespace

#pragma warning(disable: 4278)
#pragma warning(disable: 4146)
//The following #import imports EnvDTE based on its LIBID.
#import "libid:80cc9f66-e7d8-4ddd-85b6-d9e6cd0e93e2" version("8.0") lcid("0") raw_interfaces_only named_guids
#pragma warning(default: 4146)
#pragma warning(default: 4278)

namespace gj {

class DevEnv::Impl
{
public:
	void ShowSourceLine(const std::string& fileName, int lineNr);
};

void DevEnv::Impl::ShowSourceLine(const std::string& fileName, int lineNr)
{
	const wchar_t* progIds[] =
	{
		L"VisualStudio.DTE",
		L"VisualStudio.DTE.10.0",
		L"VisualStudio.DTE.9.0",
		L"VisualStudio.DTE.8.0",
		L"VisualStudio.DTE.7.1",
		L"VisualStudio.DTE.7.0"
	};

	for (auto it = std::begin(progIds); it != std::end(progIds); ++it)
	{
		auto pIDte = GetActiveObject<EnvDTE::_DTE>(ClsidFromProgId(*it));
		if (!pIDte)
			continue;

		CComPtr<EnvDTE::ItemOperations> pIOps;
		CComPtr<EnvDTE::Window> pWindow;

		HRESULT hr = pIDte->get_ItemOperations(&pIOps);
		if (SUCCEEDED(hr))
			hr = pIOps->OpenFile(CComBSTR(fileName.c_str()), CComBSTR(EnvDTE::vsViewKindTextView), &pWindow);

		CComPtr<EnvDTE::Document> pDocument;
		CComPtr<IDispatch> pIDisp;
		if (SUCCEEDED(hr))
			hr = pIDte->get_ActiveDocument(&pDocument);
		if (SUCCEEDED(hr))
			hr = pDocument->get_Selection(&pIDisp);
		CComPtr<EnvDTE::TextSelection> pSelection = com_cast<EnvDTE::TextSelection>(pIDisp);
		if (SUCCEEDED(hr) && pSelection)
			hr = pSelection->GotoLine(lineNr, VARIANT_FALSE);

		if (SUCCEEDED(hr))
			break;
	}
}


DevEnv::DevEnv()
	: m_pImpl(new Impl)
{
}

DevEnv::~DevEnv()
{
	delete m_pImpl;
}

void DevEnv::ShowSourceLine(const std::string& fileName, int lineNr)
{
	m_pImpl->ShowSourceLine(fileName, lineNr);
}

} // namespace gj

