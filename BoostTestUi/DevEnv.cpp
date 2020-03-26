// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#include "stdafx.h"
#include <algorithm>
#include <atlsafe.h>
#include "SelectDevEnvDlg.h"
#include "SelectDebugDlg.h"
#include "Utilities.h"
#include "DevEnv.h"

namespace gj {

DteInstance::DteInstance(const std::wstring& description, EnvDTE::_DTE* pIDte) :
	description(description), pIDte(pIDte)
{
}

std::vector<DteInstance> GetDteInstances()
{
	std::vector<DteInstance> instances;

	CComPtr<IRunningObjectTable> pRot;
	HRESULT hr = GetRunningObjectTable(0, &pRot);
	if (FAILED(hr))
		return instances;

	CComPtr<IEnumMoniker> pEnumMoniker;
	hr = pRot->EnumRunning(&pEnumMoniker);
	if (FAILED(hr))
		return instances;

	for (;;)
	{
		CComPtr<IMoniker> pIMoniker;
		if (pEnumMoniker->Next(1, &pIMoniker, nullptr) != S_OK)
			break;

		CComPtr<IUnknown> pIObj;
		pRot->GetObject(pIMoniker, &pIObj);
		CComPtr<EnvDTE::_DTE> pIDte = com_cast<EnvDTE::_DTE>(pIObj);
		if (!pIDte)
			continue;

		if (std::find_if(std::begin(instances), std::end(instances), [&pIDte](const DteInstance& dte) { return pIDte.IsEqualObject(dte.pIDte); }) != std::end(instances))
			continue;

		CComBSTR name;
		hr = pIDte->get_Name(&name);
		if (FAILED(hr))
			continue;

		CComBSTR version;
		hr = pIDte->get_Version(&version);
		if (FAILED(hr))
			continue;

		CComPtr<EnvDTE::_Solution> pISolution;
		hr = pIDte->get_Solution(&pISolution);
		if (FAILED(hr))
			continue;

		CComBSTR solutionName;
		hr = pISolution->get_FullName(&solutionName);
		if (FAILED(hr))
			continue;

		if (solutionName.Length() == 0)
			solutionName = "<none>";

		std::wstring description = gj::wstringbuilder() << name.m_str << L" " << version.m_str << L" (" << solutionName.m_str << L")";
		instances.push_back(DteInstance(description, pIDte));
	}

	return instances;
}

DevEnv::DevEnv() :
	m_index(-1),
	m_autoSelectEngine(true),
	m_engineSelection({L"Native"})
{
}

bool DevEnv::ShowSourceLine(const std::string& fileName, int lineNr)
{
	auto pIDte = GetDte();
	if (!pIDte)
		return false;

	CComPtr<EnvDTE::ItemOperations> pIOps;
	HRESULT hr = pIDte->get_ItemOperations(&pIOps);
	if (FAILED(hr))
		return false;

	CComPtr<EnvDTE::Window> pIWindow;
	hr = pIOps->OpenFile(CComBSTR(fileName.c_str()), CComBSTR(EnvDTE::vsViewKindTextView), &pIWindow);
	if (FAILED(hr))
		return false;

	CComPtr<EnvDTE::Document> pIDocument;
	hr = pIDte->get_ActiveDocument(&pIDocument);
	if (FAILED(hr))
		return false;

	CComPtr<IDispatch> pIDisp;
	hr = pIDocument->get_Selection(&pIDisp);
	if (FAILED(hr))
		return false;

	CComPtr<EnvDTE::TextSelection> pISelection = com_cast<EnvDTE::TextSelection>(pIDisp);
	if (!pISelection)
		return false;
	hr = pISelection->GotoLine(lineNr, VARIANT_FALSE);
	if (FAILED(hr))
		return false;

	m_pIDte = pIDte;
	return true;
}

bool DevEnv::Attach(EnvDTE80::Process2* pIProcess2)
{
	CComPtr<EnvDTE80::Transport> pITransport;
	HRESULT hr = pIProcess2->get_Transport(&pITransport);
	if (FAILED(hr))
		return false;

	CComPtr<EnvDTE80::Engines> pIEngines;
	hr = pITransport->get_Engines(&pIEngines);
	if (FAILED(hr))
		return false;

	long count = 0;
	hr = pIEngines->get_Count(&count);
	if (FAILED(hr))
		return false;

	std::vector<std::wstring> debugEngines;

	for (long i = 1; i <= count; ++i)
	{
		CComPtr<EnvDTE80::Engine> pIEngine;
		hr = pIEngines->Item(CComVariant(i), &pIEngine);
		if (FAILED(hr))
			continue;

		CComBSTR name;
		hr = pIEngine->get_Name(&name);
//		hr = pIEngine->get_ID(&name);
		if (FAILED(hr) || name.Length() == 0)
			continue;

		debugEngines.push_back(name.m_str);
	}

	std::sort(debugEngines.begin(), debugEngines.end());
	SelectDebugDlg dlg(debugEngines, m_autoSelectEngine, m_engineSelection);
	if (dlg.DoModal() != IDOK)
		return false;

	m_autoSelectEngine = dlg.GetAutoSelect();
	m_engineSelection = dlg.GetSelection();

	if (dlg.GetAutoSelect())
	{
		hr = pIProcess2->Attach();
		if (FAILED(hr))
			return false;

		return true;
	}

	auto selection = dlg.GetSelection();
	CComSafeArray<BSTR> coll(selection.size());
	int index = 0;
	for (auto& item : selection)
	{
		coll[index] = item.c_str();
		++index;
	}

	hr = pIProcess2->Attach2(CComVariant(coll));
	if (FAILED(hr))
		return false;

	return true;
}

bool DevEnv::AttachDebugger(unsigned processId)
{
	auto pIDte = GetDte();
	if (!pIDte)
		return false;

	CComPtr<EnvDTE::Debugger> pIDebugger;
	HRESULT hr = pIDte->get_Debugger(&pIDebugger);
	if (FAILED(hr))
		return false;

	CComPtr<EnvDTE::Processes> pIProcesses;
	hr = pIDebugger->get_LocalProcesses(&pIProcesses);
	if (FAILED(hr))
		return false;

	long count = 0;
	hr = pIProcesses->get_Count(&count);
	if (FAILED(hr))
		return false;

	for (long i = 1; i <= count; ++i)
	{
		CComPtr<EnvDTE::Process> pIProcess;
		hr = pIProcesses->Item(CComVariant(i), &pIProcess);
		if (FAILED(hr))
			continue;

		long id = 0;
		hr = pIProcess->get_ProcessID(&id);
		if (FAILED(hr))
			continue;

		if (static_cast<unsigned>(id) != processId)
			continue;

		if (auto pIProcess2 = com_cast<EnvDTE80::Process2>(pIProcess))
		{
			if (!Attach(pIProcess2))
				return false;
		}
		else
		{
			hr = pIProcess->Attach();
			if (FAILED(hr))
				return false;
		}
		break;
	}

	m_pIDte = pIDte;
	return true;
}

CComPtr<EnvDTE::_DTE> DevEnv::GetDte()
{
	CComPtr<EnvDTE::_DTE> pIDte2;
	if (m_pIDte && SUCCEEDED(m_pIDte->get_DTE(&pIDte2)))
		return m_pIDte;

	auto instances = GetDteInstances();
	if (instances.empty())
		return nullptr;
	if (instances.size() == 1)
		return instances[0].pIDte;

	SelectDevEnvDlg dlg(instances, m_index);
	if (dlg.DoModal() != IDOK)
		return nullptr;

	m_index = dlg.GetIndex();
	return m_index >= 0 && static_cast<size_t>(m_index) < instances.size() ? instances[m_index].pIDte : nullptr;
}

} // namespace gj
