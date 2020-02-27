#include "ViewDistancePatch.h"

#include "PatternFinder.h"

namespace
{
	typedef int(__cdecl* IndependentPrefs_t_GetInstance)();
	typedef double(__thiscall* IndependentPrefs_t_GetPrefFloat)(int, const char*, int);
	typedef double(__thiscall* IndependentPrefs_t_SetPrefFloat)(int, const char*, float, int, int);

	IndependentPrefs_t_GetInstance GetInstance = nullptr;
	IndependentPrefs_t_GetPrefFloat GetPrefFloat = nullptr;
	IndependentPrefs_t_SetPrefFloat SetPrefFloat = nullptr;
}





ViewDistancePatch::ViewDistancePatch()
{
	_configuration.at("enabled").first = false;
	PatchPair viewDistancePair = {2000.0f, [&](auto&& val) { updateViewDistance(std::get<float>(val)); }};
	_configuration.emplace("viewDistance", viewDistancePair);

}



void ViewDistancePatch::updateViewDistance(double newValue) const
{
	if (viewDistance == nullptr)
		return;
	
	DWORD old = 0;
	VirtualProtect(viewDistance, 8, PAGE_READWRITE, &old);
	*viewDistance = newValue;
	VirtualProtect(viewDistance, 8, old, &old);

	//Get the current setting for the view distance, set it to 0 then swap it back to the previous value
	//this causes the game to use the new maximum distance value
	
	const auto instance = GetInstance();
	const auto pref = GetPrefFloat(instance, "ViewDistance", 0);
	
	SetPrefFloat(instance, "ViewDistance", 0, 0, 1);
	SetPrefFloat(instance, "ViewDistance", pref, 0, 1);
	
}

bool ViewDistancePatch::_RemovePatch()
{
	if (viewDistance != nullptr)
	{
		updateViewDistance(defaultViewDistance);
	}
	return true;
}


bool ViewDistancePatch::_ApplyPatch()
{
	const auto pf = std::make_unique<PatternFinder>("N3");

	auto pfResults = pf->FindPattern("DC 0D ? ? ? ? D9 5D 08 D9 45 08 D9 9E ? ? ? ? D9 86 ? ? ? ?");

	if (pfResults.size() != 1)
	{
		spdlog::error("ViewDistancePatch failed to find the proper memory address");
		return false;
	}

	GetInstance = GetAddress<IndependentPrefs_t_GetInstance>("InstanceManager", "?GetInstance@IndependentPrefs_t@@SAPAV1@XZ");
	GetPrefFloat = GetAddress<IndependentPrefs_t_GetPrefFloat>("InstanceManager", "?GetPrefFloat@IndependentPrefs_t@@QAEMPBDW4PrefType_e@@@Z");
	SetPrefFloat = GetAddress<IndependentPrefs_t_SetPrefFloat>("InstanceManager", "?SetPrefFloat@IndependentPrefs_t@@QAEMPBDMW4PrefType_e@@_N@Z");

	const auto ptr = reinterpret_cast<int*>(pfResults[0] + 2);
	viewDistance = reinterpret_cast<double*>(*ptr);
	defaultViewDistance = *viewDistance;
	updateViewDistance(_configuration.at("viewDistance").first);

	return viewDistance != nullptr;
}
