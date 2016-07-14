// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#include "stdafx.h"
#include "DevEnv.h"

#pragma warning(disable: 4278)
#pragma warning(disable: 4146)
//The following #import imports EnvDTE based on its LIBID.
#import "libid:80cc9f66-e7d8-4ddd-85b6-d9e6cd0e93e2" version("8.0") lcid("0") raw_interfaces_only named_guids
#pragma warning(default: 4146)
#pragma warning(default: 4278)

typedef CComPtr<EnvDTE::_DTE> DTEPtr;
typedef std::pair<std::wstring, DTEPtr> DTEInfo;
typedef std::vector<DTEInfo> DTEInfos;

namespace {

DTEInfos getDTEInfos()
{
	DTEInfos dteInfos;
	CComPtr<IRunningObjectTable> pRot;
	CComPtr<IEnumMoniker> pEnumMoniker;
	CComPtr<IMoniker> pMoniker;

	HRESULT hr = GetRunningObjectTable(0, &pRot);
	if (SUCCEEDED(hr) && pRot)
		hr = pRot->EnumRunning(&pEnumMoniker);

	if (SUCCEEDED(hr) && pEnumMoniker)
		while (pEnumMoniker->Next(1, &pMoniker, nullptr) == S_OK)
		{
			CComPtr<IUnknown> pObj;
			pRot->GetObject(pMoniker, &pObj);
			pMoniker.Release();

			DTEPtr pDte = com_cast<EnvDTE::_DTE>(pObj);

			if (!pDte)
				continue;

			bool isNew = true;

			for (auto it = std::begin(dteInfos); isNew && it != std::end(dteInfos); ++it)
				isNew &= !it->second.IsEqualObject(pDte);

			if (!isNew)
				continue;

			CComBSTR name;
			CComBSTR version;
			CComBSTR solutionName;
			CComPtr<EnvDTE::_Solution> pSolution;

			hr = pDte->get_Name(&name);
			if (SUCCEEDED(hr))
				hr = pDte->get_Version(&version);
			if (SUCCEEDED(hr))
				hr = pDte->get_Solution(&pSolution);
			if (SUCCEEDED(hr) && pSolution)
				hr = pSolution->get_FullName(&solutionName);
			if (SUCCEEDED(hr))
			{
				if (solutionName.Length() == 0)
					solutionName = "<none>";

				std::wstring description;
				description += std::wstring(name, name.Length());
				description += L" ";
				description += std::wstring(version, version.Length());
				description += L" (";
				description += std::wstring(solutionName, solutionName.Length());
				description += L")";

				dteInfos.push_back(DTEInfo(description, pDte));
			}
		}

	return dteInfos;
}

DTEPtr SelectDTE(const std::function<int(const std::vector<std::wstring>& names, int indexHint)>& select, DTEPtr pIDteHint = DTEPtr())
{
	auto dteInfos = getDTEInfos();
	std::vector<std::wstring> names;
	int indexHint = -1;

	for (size_t i = 0; i < dteInfos.size(); ++i)
	{
		auto& dteInfo(dteInfos[i]);

		names.push_back(dteInfo.first);

		if (dteInfo.second.IsEqualObject(pIDteHint))
			indexHint = i;
	}

	int index = select ? select(names, indexHint) : indexHint;

	if (0 <= index && size_t(index) < dteInfos.size())
		return dteInfos[index].second;
	else
		return DTEPtr();
}

HRESULT ShowSourceLine(DTEPtr pIDte, const std::string& fileName, int lineNr)
{
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

	return hr;
}

HRESULT AttachDebugger(DTEPtr pIDte, long testProcessID)
{
	CComPtr<EnvDTE::Debugger> pDebugger;
	CComPtr<EnvDTE::Processes> pProcesses;

	HRESULT hr = pIDte->get_Debugger(&pDebugger);
	if (SUCCEEDED(hr) && pDebugger)
		hr = pDebugger->get_LocalProcesses(&pProcesses);
	if (SUCCEEDED(hr) && pProcesses)
	{
		long count = 0;
		hr = pProcesses->get_Count(&count);

		if (SUCCEEDED(hr))
		{
			for (long i = 1; i <= count; ++i)
			{
				CComPtr<EnvDTE::Process> pProcess;
				long processID = 0;

				hr = pProcesses->Item(_variant_t(i), &pProcess);

				if (SUCCEEDED(hr) && pProcess)
					hr = pProcess->get_ProcessID(&processID);
				if (SUCCEEDED(hr) && processID == testProcessID)
				{
					return pProcess->Attach();
				}
			}

			return E_FAIL;
		}
	}

	return hr;
}

DTEPtr activate(CComPtr<IUnknown>& pUnk)
{
	DTEPtr pIDte = com_cast<EnvDTE::_DTE>(pUnk);
	DTEPtr pDummy;

	if (pIDte && FAILED(pIDte->get_DTE(&pDummy)))
		pUnk = pIDte = DTEPtr();

	return pIDte;
}

} // namespace


namespace gj {

DevEnv::DevEnv(const std::function<int(const std::vector<std::wstring>& names, int indexHint)>& selectDte)
	: m_selectDte(selectDte)
{
}

DevEnv::~DevEnv()
{
}

bool DevEnv::HasSelection()
{
	return activate(m_pIDte) != nullptr;
}

void DevEnv::ClearSelection()
{
	m_pIDte = nullptr;
}

void DevEnv::SelectDte()
{
	m_pIDte = ::SelectDTE(m_selectDte, activate(m_pIDte));
}

void DevEnv::ShowSourceLine(const std::string& fileName, int lineNr)
{
	DTEPtr pIDte = activate(m_pIDte);

	if (!pIDte)
		pIDte = ::SelectDTE(m_selectDte);

	if (pIDte && SUCCEEDED(::ShowSourceLine(pIDte, fileName, lineNr)))
		m_pIDte = pIDte;
}

bool DevEnv::AttachDebugger(long testProcessID)
{
	DTEPtr pIDte = activate(m_pIDte);

	if (!pIDte)
		pIDte = ::SelectDTE(m_selectDte);

	if (pIDte && SUCCEEDED(::AttachDebugger(pIDte, testProcessID)))
	{
		m_pIDte = pIDte;
		return true;
	}
	else
		return false;
}

} // namespace gj

