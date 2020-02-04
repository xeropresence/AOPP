#include "AutoRunPatch.h"

hookData * forwardHook = nullptr;
hookData * autoruntoggleHook = nullptr;

__declspec(noinline) void __fastcall forwardfunc(void* a1, void* EDX, unsigned char a2);
__declspec(noinline) void __fastcall autorunhook(void* a1, void* EDX, bool a2);

bool keydown = false;
bool autorunDuringForward = false;
bool currentlyRunning = false;

__declspec(noinline) void __fastcall forwardfunc(void* a1, void* EDX,unsigned char a2)
{
	typedef void(__thiscall* originalFunction)(void* a1, bool a2);
	//std::cout << "[" << __FUNCTION__ << "] " << std::dec << a1 << "   " << (int)a2 << std::endl;

	if (a2 == 2)
	{
		a2 = keydown;
		keydown = !keydown;

		if (currentlyRunning)
			autorunDuringForward = true;
	}
	else
	{
		if (a2 == 1)
		{
			if (autorunDuringForward)
			{
				autorunDuringForward = false;
				return;
			}
			currentlyRunning = false;
		}

		if (a2 == 0)
		{
			currentlyRunning = true;
		}
		keydown = false;
	}

	reinterpret_cast<originalFunction>(forwardHook->originalTramp)(a1, a2);
}


__declspec(noinline) void __fastcall autorunhook(void* a1, void* EDX, bool a2)
{
	if (a2 == 0)
	{
		forwardfunc(a1, 0, 2);
	}
}


bool AutoRunPatch::_ApplyPatch()
{

	if (autoruntoggleHook == nullptr)
		autoruntoggleHook = new hookData ("Gui", "?SlotMovementAutoRun@FlowControlModule_t@@AAEX_N@Z", &autorunhook);

	if (forwardHook == nullptr)
		forwardHook = new hookData("Gui", "?SlotMovementForward@FlowControlModule_t@@AAEX_N@Z", &forwardfunc);

	
	if (ApplyHook(forwardHook))
	{
		if (ApplyHook(autoruntoggleHook))
		{
			return true;
		}
	}
	return false;
}
