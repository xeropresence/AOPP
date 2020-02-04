#include "XButtonDoubleClickPatch.h"
#include "PatternFinder.h"
#include <Psapi.h>


void SetAddress(int * address,int value)
{
	DWORD old = 0;
	VirtualProtect(address, 1, PAGE_READWRITE, &old);
	*address = value;
	VirtualProtect(address, 1, old, &old);
}



bool XButtonDoubleClickPatch::_ApplyPatch()
{
	char buff[32];
	GetModuleBaseNameA(GetCurrentProcess(), nullptr, buff, 32);
	
	const auto pf = std::make_unique<PatternFinder>(buff);

	auto pfResults = pf->FindPattern("C7 45 ? ? ? ? ? EB AE");
	if (pfResults.size() != 1)
	{
		std::cout << "XButtonDoubleClickPatch 1 failed to find the correct address" << std::endl;
		return false;
	}

	patch1 = reinterpret_cast<int*>(pfResults[0] + 3);
	SetAddress(patch1, 121);

	pfResults = pf->FindPattern("C7 45 ? ? ? ? ? EB 9F");

	if (pfResults.size() != 1)
	{
		std::cout << "XButtonDoubleClickPatch 2 failed to find the correct address" << std::endl;
		return false;
	}

	patch2 = reinterpret_cast<int*>(pfResults[0] + 3);
	SetAddress(patch2, 122);

	return true;
}

bool XButtonDoubleClickPatch::_RemovePatch()
{
	SetAddress(patch1, 125);
	SetAddress(patch2, 126);
	return true;
}


