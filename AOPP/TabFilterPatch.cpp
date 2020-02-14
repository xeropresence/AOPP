#include "TabFilterPatch.h"
#include <unordered_set>

#include "DataTypes.h"

hookData* GetCloseTargetHook = nullptr;



//

typedef int* (__cdecl* _GetDynel)(__in Identity_t*);
typedef int(__cdecl* _GetInstance)();
typedef char* (__thiscall* _GetName)(int, __in Identity_t*, __in Identity_t*);

_GetDynel GetDynel = nullptr; 
_GetInstance GetInstance = nullptr; 
_GetName GetName = nullptr; 



int* GetDynelByIdentity(Identity_t* identity)
{
	int* result;
	if (identity->Type == 50000)
		result = GetDynel(identity);
	else
		result = nullptr;
	return result;
}

char* getName(Identity_t* who)
{
	const auto dynel = GetDynelByIdentity(who);

	if (dynel != nullptr)
	{
		return GetName(GetInstance(), who, who);
	}

	return nullptr;
}


std::unordered_set<std::string> stuff_that_ruins_your_day
{
	"Unicorn Service Tower Alpha",
	"Unicorn Service Tower Delta",
	"Unicorn Service Tower Gamma",
	"Rookie Alien Hunter",
	"Altar of Torture",
	"Altar of Purification",
	"Guardian Spirit of Purification",
};



__declspec(noinline) int __fastcall GetCloseTarget(int a1, void* EDX, Identity_t* a2, bool forward, bool friendly)
{
	typedef int(__thiscall* OriginalFunctionType)(int, Identity_t*, int, int);
	char* name = nullptr;
	Identity_t starting{};

	memcpy(&starting, a2, sizeof(Identity_t));

	int result;
	while(true)
	{
		result = reinterpret_cast<OriginalFunctionType>(GetCloseTargetHook->originalTramp)(a1, a2, forward, friendly);

		name = getName(a2);
		if (name == nullptr || starting.Key == a2->Key ||  stuff_that_ruins_your_day.find(name) == stuff_that_ruins_your_day.end())
		{
			break;
		}
	}

	return result;
}


bool TabFilterPatch::_ApplyPatch()
{

	 GetDynel =  GetAddress<_GetDynel>("N3", "?GetDynel@n3Dynel_t@@SAPAV1@ABVIdentity_t@@@Z");
	 GetInstance =GetAddress<_GetInstance>("N3", "?GetInstance@n3Engine_t@@SAPAV1@XZ");
	 GetName = GetAddress<_GetName>("Gamecode", "?N3Msg_GetName@n3EngineClientAnarchy_t@@QBEPBDABVIdentity_t@@0@Z");

	
	if (GetCloseTargetHook == nullptr)
		GetCloseTargetHook = new hookData("Gamecode", "?N3Msg_GetCloseTarget@n3EngineClientAnarchy_t@@QBE_NAAVIdentity_t@@_N1@Z", &GetCloseTarget);

	ApplyHook(GetCloseTargetHook);
	return true;
}
