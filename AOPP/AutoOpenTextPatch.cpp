#include "AutoOpenTextPatch.h"
#include <headers/CapstoneDisassembler.hpp>
#include "PatternFinder.h"


#include "gumbo.h"


std::string lastMessageTarget;

#pragma region outgoing message section
hookData * outgoingMessageHook = nullptr;

#pragma region Dont look at me like that
#define _DWORD int
#define _BYTE  unsigned char
struct __declspec(align(8)) ChatServerMessageStruct
{
	_DWORD dword0;
	_DWORD dword4;
	_DWORD dword8;
	std::string str1;
	_DWORD dword24;
	std::string str2;
	_DWORD dword38;
	_DWORD dword3C;
	_DWORD dword40;
	_DWORD dword44;
	std::string str3;
	_DWORD dword54;
	_DWORD dword58;
	_DWORD dword5c;
	_DWORD dword60;
	_DWORD dword64;
	_BYTE gap68[12];
	_BYTE byte74;
	_BYTE gap75[15];
	_DWORD dword84;
	_DWORD dword88;
	__declspec(align(8)) _BYTE byte90;
	_BYTE gap91[15];
	_DWORD dwordA0;
	_DWORD dwordA4;
	_BYTE gapA8[28];
	_DWORD dwordC4;
};


#pragma endregion 


char * SendPrivateMessageFuncAdder()
{
	const auto pf = std::make_unique<PatternFinder>("Gui");

	auto pfResults = pf->FindPattern("E8 ? ? ? ? 8D 4D C4 C6 45 FC 0F");

	if (pfResults.size() != 1)
	{
		std::cout << "AutoOpenTextPatch failed to find function to hook" << std::endl;
		return nullptr;
	}

	const auto ptr = reinterpret_cast<int*>(pfResults[0] + 1);
	const auto jumpLoc = (pfResults[0] + 5) + (*ptr);
	//std::cout << "JUMP LOC :" << std::hex << jumpLoc << std::endl;


	return reinterpret_cast<char*>(jumpLoc);
}

__declspec(noinline) int __fastcall OutgoingPrivateMessageFunc(int a1, void* EDX, int a2, ChatServerMessageStruct* a3)
{
	typedef int(__thiscall* OriginalFunctionType)(int,int, ChatServerMessageStruct*);
	//std::cout << "[" << __FUNCTION__ << "] " << std::hex << a1 << "   " << a2 << "   " << a3->str3 << "   " << std::endl;

	if (a3->str3.length() > 0)
	{
		lastMessageTarget.assign(a3->str1);
	}
	
	return reinterpret_cast<OriginalFunctionType>(outgoingMessageHook->originalTramp)(a1, a2, a3);
}


#pragma endregion


#pragma region incoming message
hookData * incomingMessageHook = nullptr;

struct IncomingMessage
{
public:
	char pad_0000[12]; //0x0000
	char Sender[28]; //0x000C
	char* Message; //0x0028
};



typedef void(__cdecl* OpenLinkType)(__in std::string&);
OpenLinkType openlink;

void search_for_links(GumboNode* node, int depth)
{
	if (node->type != GUMBO_NODE_ELEMENT || depth > 5)
		return;

	if (node->v.element.tag == GUMBO_TAG_A)
	{
		GumboAttribute* href = gumbo_get_attribute(&node->v.element.attributes, "href");
		if (href != nullptr)
		{
			std::string asString(href->value);
			if (asString.find("text://") == 0)
			{
				openlink(asString);
			}
			return;
		}
	}

	GumboVector* children = &node->v.element.children;
	for (unsigned int i = 0; i < children->length; ++i)
	{
		search_for_links(static_cast<GumboNode*>(children->data[i]), ++depth);
	}
}

bool icompare_pred(unsigned char a, unsigned char b)
{
	return std::tolower(a) == std::tolower(b);
}

bool icompare(std::string const& a, std::string const& b)
{
	if (a.length() == b.length()) 
	{
		return std::equal(b.begin(), b.end(),a.begin(), icompare_pred);
	}
	return false;
}

__declspec(noinline) static void  __fastcall privmessagefunc(int a1, void* EDX, IncomingMessage* a2)
{
	typedef void(__thiscall* OriginalFunctionType)(int, IncomingMessage*);
	
	if (icompare(lastMessageTarget ,a2->Sender))
	{
		lastMessageTarget = "";
		GumboOutput* output = gumbo_parse(a2->Message);
		search_for_links(output->root, 0);
		gumbo_destroy_output(&kGumboDefaultOptions, output);
	}
	
	return reinterpret_cast<OriginalFunctionType>(incomingMessageHook->originalTramp)(a1, a2);
}

#pragma endregion


bool AutoOpenTextPatch::_ApplyPatch()
{
	openlink = GetAddress<OpenLinkType>("Gui", "?ShowItemRefLink@ChatGUIModule_c@@SAXABVString@@@Z");

	if (outgoingMessageHook == nullptr)
		outgoingMessageHook = new hookData("Gui", "OutgoingPrivateMessageFunc", SendPrivateMessageFuncAdder(), &OutgoingPrivateMessageFunc);

	if (incomingMessageHook == nullptr)
		incomingMessageHook = new hookData("Gui", "?HandlePrivateMessage@ChatGUIModule_c@@AAEXPBUPrivateMessage_t@Client_c@ppj@@@Z", &privmessagefunc);

	
	ApplyHook(outgoingMessageHook);
	ApplyHook(incomingMessageHook);
	return true;
}
