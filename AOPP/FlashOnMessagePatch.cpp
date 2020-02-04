#include "FlashOnMessagePatch.h"
#include <unordered_set>
#include "FlashManager.h"


hookData * DialogConstructorHook = nullptr;
std::unordered_set<std::string> titles{"Join Team?" };
std::unordered_set<std::string> messages{ "There is now an opening for you to fight at the battle station. Do you want to join the fight?" };




//Holy fuck this sucks,varargs? Is there a better way to handle this?
__declspec(noinline) int* __cdecl DialogConstructorHookFunc(int* a1, int* a2, char* title, char* text, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12, int a13)
{
	typedef int* (__cdecl* thingu)(int* a1, int* a2, char* a3, char* a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12, int a13);
	//std::cout << "[" << __FUNCTION__ << "] " << std::hex << a1 << "   " << a2 << "  a3:" << title << "   " << text << "   " << a5 << "   " << a6 << "   " << a7 << "   " << a8 << "   " << a9 << "   " << a10 << "   " << a11 << "   " << a12 << "   " << a13 << std::endl;

	if (std::find(titles.begin(),titles.end(),title) != std::end(titles) || std::find(messages.begin(), messages.end(), text) != std::end(messages))
    {
		FlashManager::getInstance().Flash();
	}
	
	const auto res = reinterpret_cast<thingu>(DialogConstructorHook->originalTramp)(a1, a2, title, text, a5, a6, a7, a8, a9, a10, a11, a12, a13);
	return res;
}

bool FlashOnMessagePatch::_ApplyPatch() 
{
	if (DialogConstructorHook == nullptr)
		DialogConstructorHook = new hookData("Gui", "??0DialogBox_c@@QAA@PAVWindow@@ABV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V23@ZZ", &DialogConstructorHookFunc);

	ApplyHook(DialogConstructorHook);

	return true;
}
