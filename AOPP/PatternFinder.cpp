#include "PatternFinder.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

PatternFinder::PatternFinder(const std::string& module)
{

	const auto handle = GetModuleHandleA(module.c_str());
	if (handle == nullptr)
		return;
	
	_data = reinterpret_cast<signed char*>(handle);

	_baseAddress = reinterpret_cast<int>(_data);
	
	PIMAGE_DOS_HEADER pDosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(_data);
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		throw std::runtime_error("Image is not a valid PE file");

	auto pImageHeader = reinterpret_cast<PIMAGE_NT_HEADERS>((char*)pDosHeader + pDosHeader->e_lfanew);
	if (pImageHeader->Signature != IMAGE_NT_SIGNATURE)
		throw std::runtime_error("Image is not a valid NT PE file");

	auto pSecHeader = IMAGE_FIRST_SECTION(pImageHeader);
	
	for (size_t i = 0; i < pImageHeader->FileHeader.NumberOfSections; i++)
	{
		if (!strcmp(reinterpret_cast<const char*>(pSecHeader->Name), ".text"))
		{
			SizeOfCode = pSecHeader->Misc.VirtualSize;
			CodeBase = pSecHeader->VirtualAddress;
			_dataLength = CodeBase + SizeOfCode;
			return;
		}
		pSecHeader++;
		std::cout << i << std::endl;
	}

}

std::vector<int> HexToBytes(const std::string& hex)
{
	std::vector<int> bytes;

	for (unsigned int i = 0; i < hex.length(); )
	{
		if (hex[i] == '?')
		{
			if (hex[i + 1] == '?')
				i++;
			i++;
			bytes.push_back(-1);
			continue;
		}
		if (hex[i] == ' ')
		{
			i++;
			continue;
		}

		std::string byteString = hex.substr(i, 2);
		const char byte = static_cast<char>(strtol(byteString.c_str(), nullptr, 16));
		bytes.push_back(byte);
		i += 2;
	}

	return bytes;
}


std::vector<intptr_t> PatternFinder::FindPattern(const std::string& pattern) const
{
	std::vector<intptr_t> results;

	auto res = Find(HexToBytes(pattern));
	for (int& re : res)
	{
		re = re +_baseAddress;
	}
	return res;
}

std::vector<intptr_t> PatternFinder::Find(std::vector<int> &&  pattern) const
{
	std::vector<intptr_t> ret;
	int plen = pattern.size();
	int dataLength = _dataLength - plen;
	for (int i = 0; i < dataLength; i++)
	{
		if (ByteMatch(_data, i, pattern))
			ret.push_back(i);
	}
	return ret;
}

bool PatternFinder::ByteMatch(signed char* bytes, int start, std::vector<int>& pattern)
{
	for (int i = start, j = 0; j < pattern.size(); i++, j++)
	{
		if (pattern[j] == -1)
			continue;

		if (bytes[i] != pattern[j])
			return false;
	}
	return true;
}
