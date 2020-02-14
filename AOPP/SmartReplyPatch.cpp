#include "SmartReplyPatch.h"
#include "PatternFinder.h"
#include "DataTypes.h"



bool startsWithFont(PrivateMessage_t* message)
{
	//std::cout << message->Sender.string << "==>" << message->Message.string << std::endl;
	return message->Message.string.find("<font") == 0;
}



int emptySenderPtr;
int validSenderPtr;
__declspec(naked) void hoooook()
{
	PrivateMessage_t* pmPnt;
	bool isbot;
	__asm PUSHAD
	__asm PUSHFD

	__asm mov pmPnt, esi

	isbot = startsWithFont(pmPnt);

	__asm POPFD
	__asm POPAD



	if (isbot)
	{
		__asm jmp emptySender;
	}

	__asm cmp[esi + 0x1C], ebx;
	__asm je emptySender;

	__asm jmp[validSenderPtr];

emptySender:
	__asm jmp[emptySenderPtr];

}


inline void SetAddress(intptr_t address, int value)
{
	DWORD old = 0;
	const auto lp_address = reinterpret_cast<int*>(address);
	VirtualProtect(lp_address, 1, PAGE_READWRITE, &old);
	*lp_address = value;
	VirtualProtect(lp_address, 1, old, &old);
}

intptr_t hookLocation = 0;
bool SmartReplyPatch::_ApplyPatch()
{
	const auto pf = std::make_unique<PatternFinder>("GUI");

	auto pfResults = pf->FindPattern("39 5E 1C 74 6E");

	if (pfResults.size() != 1)
	{
		std::cout << "SmartReplyPatch failed to find function to hook" << std::endl;
		return false;
	}


	//std::cout << std::hex << "hoooook is at " << std::addressof(hoooook) << std::endl;
	//std::cout << std::hex << "jmp is at " << pfResults[0] << std::endl;

	hookLocation = pfResults[0];

	SetAddress(hookLocation, 0xe9);
	const auto ptr2 = pfResults[0] + 1;
	SetAddress(ptr2, reinterpret_cast<int>(std::addressof(hoooook)) - pfResults[0] - 5);


	emptySenderPtr = hookLocation + 5 + 0x6E;
	validSenderPtr = hookLocation + 5;


	
	return true;
}

bool SmartReplyPatch::_RemovePatch()
{

	SetAddress(hookLocation, 0x1C5e39);
	SetAddress(hookLocation + 3, 0x6e74);
	
	return true;
}
