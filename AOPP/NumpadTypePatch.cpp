#include "NumpadTypePatch.h"


#include <fstream>

typedef int* (__thiscall* GetKeyboardFocusType)(__in int*);
typedef int* (__cdecl* WindowController_c_GetInstanceIfAnyType)();
__declspec(noinline) LRESULT __fastcall MessageHandler(void* a1, void* EDX, HWND hWnd, int msg, WPARAM WideCharStr, HRAWINPUT hRawInput);


GetKeyboardFocusType GetKeyboardFocus = nullptr;
WindowController_c_GetInstanceIfAnyType WindowController_c_GetInstanceIfAny = nullptr;



hookData* numpadHook = nullptr;

bool NumpadTypePatch::_ApplyPatch()
{
	GetKeyboardFocus = GetAddress< GetKeyboardFocusType>("Gui", "?GetKeyboardFocus@WindowController_c@@QBEPAVView@@XZ");
	WindowController_c_GetInstanceIfAny = GetAddress<WindowController_c_GetInstanceIfAnyType>("Gui", "?GetInstanceIfAny@WindowController_c@@SAPAV1@XZ");

	
	WNDCLASSEX wce;
	GetClassInfoEx(GetModuleHandle(nullptr), L"Anarchy client", &wce);


	if (numpadHook == nullptr)
		numpadHook = new  hookData("AnarchyOnline.exe", "Window class", reinterpret_cast<const char*>(wce.lpfnWndProc), &MessageHandler);

	ApplyHook(numpadHook);
	
	return true;
}




__declspec(noinline) LRESULT __fastcall MessageHandler(void* a1,void *EDX, HWND hWnd, int msg, WPARAM WideCharStr, HRAWINPUT hRawInput)
{
	typedef LRESULT(__thiscall* originalFunctionHeader)(void*, HWND, int, WPARAM, HRAWINPUT);
	
	if ((msg == WM_KEYDOWN || msg == WM_KEYUP) && (WideCharStr >= 0x60 && WideCharStr <= 0x69))
	{
		const auto WindowController = WindowController_c_GetInstanceIfAny();
		if (WindowController != nullptr)
		{
			const auto currentlyTyping = GetKeyboardFocus(WindowController) != nullptr;
			if (currentlyTyping)
			{
				//Subtract 0x30 from the numpad keycode to get the 0-9 keycodes
				WideCharStr = WideCharStr - 0x30;
			}
		}
	
	}
	return  reinterpret_cast<originalFunctionHeader>(numpadHook->originalTramp)(a1, hWnd, msg, WideCharStr, hRawInput);
}