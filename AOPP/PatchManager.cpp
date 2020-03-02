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
#include "Utils.h"

#include <fstream>
#include <Psapi.h>


#include "ConfigurationHolder.h"
#include "LargeAddressAwarePatch.h"
#include "ViewDistancePatch.h"

#define params HWND a1, int a2
#define params2 a1, a2
__declspec(noinline) HWND  __stdcall ShowWindowHook(params);

std::atomic_flag applied = ATOMIC_FLAG_INIT;

PLH::x86Detour * detour;
uint64_t originalTramp;


bool _initialized = false;
bool PatchManager::initialized()
{
	return _initialized;
}

PatchManager::PatchManager()
{
	PLH::CapstoneDisassembler dis(PLH::Mode::x86);
	Configuration = std::make_unique<ConfigurationHolder>();

	CHAR buff[32];
	
	GetModuleBaseNameA(GetCurrentProcess(), nullptr, buff, 32);
	spdlog::info(buff);
	if (_strcmpi(buff, "Anarchy.exe") == 0)
	{
		const auto LAA = std::make_unique<LargeAddressAwarePatch>();
		LAA->Initialize(Configuration->config);
		Configuration->Save();

		if (LAA->enabled())
			LAA->ApplyPatch();
		
		return;
	}
	
	const char* ShowWindow = Patch::GetAddress<const char*>("User32.dll", "ShowWindow");
	detour = new PLH::x86Detour(reinterpret_cast<const char*>(ShowWindow), reinterpret_cast<char*>(&ShowWindowHook), &originalTramp, dis);

	const auto result = detour->hook();

	if (!result)
	{
		spdlog::error("Failed to apply hook initial hook {}", PLH::ErrorLog::singleton().pop().msg);
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
	Patches.emplace_back(std::make_unique<ViewDistancePatch>());
	

	_CommandInterpreter = std::make_unique<CommandInterpreter>();
	
	
	
	for (auto&& patch : Patches)
	{
		patch->Initialize(Configuration->config);
	}

	Configuration->Save();
}




__declspec(noinline) HWND __stdcall ShowWindowHook(params)
{
	typedef HWND(__stdcall* fnction)(params);
	const auto result = reinterpret_cast<fnction>(originalTramp)(params2);
	
	if (!applied.test_and_set())
	{
		for(auto && patch  : PatchManager::getInstance().Patches)
		{
			if (patch->enabled())
			{
				if (patch->ApplyPatch())
				{
					
				}
			}
				
		}
		_initialized = true;
		
		detour->unHook();
		delete detour;
	}
	
	return result;
}

