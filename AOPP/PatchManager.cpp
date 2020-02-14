#include "PatchManager.h"

#include <Windows.h>
#include <polyhook2/Detour/x86Detour.hpp>
#include <polyhook2/CapstoneDisassembler.hpp>
#include "AutoRunPatch.h"
#include "FramecapPatch.h"
#include "NumpadTypePatch.h"
#include "FlashOnMessagePatch.h"
#include "PatternFinder.h"
#include "XButtonDoubleClickPatch.h"
#include "AutoOpenTextPatch.h"
#include "CommaGMIPatch.h"
#include "TabFilterPatch.h"
#include "SmartReplyPatch.h"

#define params DWORD dwExStyle,LPCSTR lpClassName,LPCSTR lpWindowName,DWORD dwStyle,int X,int Y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam
#define params2 dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam
__declspec(noinline) HWND  __stdcall CreateWindowHook(params);

std::atomic_flag applied = ATOMIC_FLAG_INIT;

PLH::x86Detour * detour;
uint64_t originalTramp;

std::vector<std::unique_ptr<Patch>> Patches;
PatchManager::PatchManager()
{
	PLH::CapstoneDisassembler dis(PLH::Mode::x86);

	
	const char* CreateWindowExW = Patch::GetAddress<const char*>("User32.dll", "CreateWindowExW");
	detour = new PLH::x86Detour(reinterpret_cast<const char*>(CreateWindowExW), reinterpret_cast<char*>(&CreateWindowHook), &originalTramp, dis);

	const auto result = detour->hook();

	if (!result)
	{
		std::cout << "Failed to apply hook " << PLH::ErrorLog::singleton().pop().msg << std::endl;
	}

	Patches.emplace_back(std::make_unique<AutoRunPatch>());
	Patches.emplace_back(std::make_unique<FramecapPatch>());
	Patches.emplace_back(std::make_unique<NumpadTypePatch>());
	Patches.emplace_back(std::make_unique<FlashOnMessagePatch>());
	Patches.emplace_back(std::make_unique<XButtonDoubleClickPatch>());
	Patches.emplace_back(std::make_unique<AutoOpenTextPatch>());
	Patches.emplace_back(std::make_unique<CommaGMIPatch>());
	Patches.emplace_back(std::make_unique<TabFilterPatch>());
	Patches.emplace_back(std::make_unique<SmartReplyPatch>());
	
}




__declspec(noinline) HWND __stdcall CreateWindowHook(params)
{
	typedef HWND(__stdcall* fnction)(params);
	const auto result = reinterpret_cast<fnction>(originalTramp)(params2);

	if (!applied.test_and_set())
	{

		for(auto && patch  : Patches)
		{
			patch->ApplyPatch();
		}
		
		detour->unHook();
		delete detour;
	}
	
	return result;
}

