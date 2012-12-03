//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the boost test library home page.

#include "stdafx.h"
#include <boost/iostreams/device/mapped_file.hpp>
#include "GetUnitTestType.h"

namespace gj {

template <typename T>
const T* GetPtr(const void* base, ptrdiff_t offset = 0)
{
	const void* p = static_cast<const char*>(base) + offset;
	return static_cast<const T*>(p);
}

template <typename ImageNtHeaders>
const IMAGE_SECTION_HEADER* GetEnclosingSectionHeader(DWORD rva, const ImageNtHeaders* pNTHeader)
{
	const IMAGE_SECTION_HEADER* section = IMAGE_FIRST_SECTION(pNTHeader);

	for (unsigned i = 0; i < pNTHeader->FileHeader.NumberOfSections; ++i, ++section)
	{
		// Is the RVA within this section?
		if (rva >= section->VirtualAddress && rva < (section->VirtualAddress + section->Misc.VirtualSize))
			return section;
	}

	return nullptr;
}

static const std::string unit_test_type_("unit_test_type_");

template <typename ImageNtHeaders>
std::string GetUnitTestType(const void* base, const ImageNtHeaders* pNTHeader)
{    
	DWORD exportsStartRVA = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

	// Get the IMAGE_SECTION_HEADER that contains the exports.  This is
	// usually the .edata section, but doesn't have to be.
	const IMAGE_SECTION_HEADER* header = GetEnclosingSectionHeader(exportsStartRVA, pNTHeader);
	if (!header)
		return "";

	int delta = static_cast<int>(header->VirtualAddress - header->PointerToRawData);
	const IMAGE_EXPORT_DIRECTORY* exportDir = GetPtr<IMAGE_EXPORT_DIRECTORY>(base, exportsStartRVA - delta);   
	const DWORD* names = GetPtr<DWORD>(base, (DWORD)exportDir->AddressOfNames - delta);

	// See if this function has an associated name exported for it.
	for (DWORD j = 0; j < exportDir->NumberOfNames; ++j)
	{
		std::string name = GetPtr<char>(base, names[j] - delta);
		if (name.substr(0, unit_test_type_.size()) == unit_test_type_)
			return name.substr(unit_test_type_.size());
		if (name == "init_unit_test")
			return "boost";
	}
	return "";
}

std::string GetUnitTestType(const IMAGE_DOS_HEADER* pDosHeader, size_t size)
{
	if (pDosHeader->e_lfanew < 0 || pDosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS32) > size)
		return "";

	auto pNTHeader = GetPtr<IMAGE_NT_HEADERS32>(pDosHeader, pDosHeader->e_lfanew);
	if (pNTHeader->Signature != IMAGE_NT_SIGNATURE)
		return "";

	switch (pNTHeader->FileHeader.Machine)
	{
	case IMAGE_FILE_MACHINE_I386: return GetUnitTestType(pDosHeader, pNTHeader);
	case IMAGE_FILE_MACHINE_AMD64: return GetUnitTestType(pDosHeader, GetPtr<IMAGE_NT_HEADERS64>(pDosHeader, pDosHeader->e_lfanew));
	}
	return "";
}

std::string GetUnitTestType(const std::string& path)
{
	boost::iostreams::mapped_file_source file(path);

	auto pDosHeader = GetPtr<IMAGE_DOS_HEADER>(file.data());
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return "";

	return GetUnitTestType(pDosHeader, file.size());
}

} // namespace gj
