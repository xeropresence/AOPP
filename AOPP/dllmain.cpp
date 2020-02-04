#include <windows.h>
#include <Psapi.h>
#include "PatchManager.h"
#include "LargeAddressAwarePatch.h"
#include "mswsock.h"


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{

#define OpenConsole 0
#if OpenConsole
		AllocConsole();
		freopen_s(reinterpret_cast<FILE**>(stdin), "conin$", "r", stdin);
		freopen_s(reinterpret_cast<FILE**>(stdout), "conout$", "w", stdout);
		freopen_s(reinterpret_cast<FILE**>(stderr), "conout$", "w", stderr);
#endif




		if (!mswsock::Attach())
		{
			return FALSE;
		}

	
		char buff[32];
		GetModuleBaseNameA(GetCurrentProcess(), nullptr, buff, 32);



		if (!_stricmp(buff, "Anarchy.exe"))
		{
			printf("launcher\n");
			LargeAddressAwarePatch laap;
			laap.ApplyPatch();
		}
		else
		{
			const auto & pm = PatchManager::getInstance();
		}

		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		mswsock::Dettach();
		break;
	default:;
	}
	return TRUE;
}
