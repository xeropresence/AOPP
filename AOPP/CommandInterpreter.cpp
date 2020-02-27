#include "CommandInterpreter.h"

#include <memory>
#include <spdlog/spdlog.h>



#include "DataTypes.h"
#include "Patch.h"
#include "PatchManager.h"
#include "PatternFinder.h"
#include "Utils.h"

hookData* cGenericInitializationHook = nullptr;


typedef int(__thiscall* RegisterCommandFunc)(int a1, AOString* a2, int a3, char a4, char a5);
typedef int(__thiscall* RegisterHandlerFunc)(int, int, void*);
typedef int(__cdecl * CompareNoCase)(const std::string *, const std::string *, int);

CompareNoCase cnc;

struct unkCarrier
{
	DWORD dword0{};
	DWORD dword4{};
	AOString str{};
};


typedef int(__thiscall* AppendTabResult)(int, unkCarrier*);
AppendTabResult _AppendTabResult = nullptr;


struct something
{
	AOString aostring;
	int offset;

};

struct words
{
	something* dword0;
	int dword4;

	[[nodiscard]] int count() const
	{
		return static_cast<signed int>(dword4 - reinterpret_cast<unsigned int>(dword0)) >> 5;
	}
};



__declspec(noinline) void __stdcall HandleCommand(int a1, std::string* a2, words* a3)
{
	//std::cout << std::hex << a1 << " " << *a2 << " " << a3->count() << std::endl;

	if (a3->count() == 4)
	{

		const auto patchName = a3->dword0[1].aostring.string;
		const auto variable = a3->dword0[2].aostring.string;
		const auto value = a3->dword0[3].aostring.string;
		for (auto&& patch : PatchManager::getInstance().Patches)
		{
			if (strcmp(patch->Name(), patchName.c_str()) == 0)
			{
				for (auto&& config : patch->getConfigOptions())
				{
					if (strcmp(config.first.c_str(), variable.c_str()) == 0)
					{
						patch->setConfigOption(config.first, value);
						break;
					}
				}
				break;
			}
		}
		
	}
	
}

__declspec(noinline) void __stdcall HandleTabPress(int a1, unsigned char* a2, int a3, int a4, words* a5, int index, AOString* a7, int a8, int a9)// , int a10, int a11)
{

	unsigned char* result = nullptr; // eax
	//std::cout << std::hex << a1 << " " << (int)(*a2) << " " << a3 << " " << a4 << " " << a5 << " " << index << " " << a7->string << " " << a8 << " " << a9 << std::endl;// << " " << a10 << " " << a11 << std::endl;


	if (index == 1)
	{
		for (auto&& patch : PatchManager::getInstance().Patches)
		{
			std::string tmp(patch->Name());
			if (!cnc(&a7->string,&tmp,a7->length()))
			{
				unkCarrier test;
				test.dword0 = a8;
				test.dword4 = a9;
				test.str.string = patch->Name();
				_AppendTabResult(a1, &test);
			}
		}
		*a2 = 1;
		return;
	}

	if (index == 2)
	{
		const auto previousWord = a5->dword0[index - 1].aostring.string;

		for (auto&& patch : PatchManager::getInstance().Patches)
		{	
			if (strcmp(patch->Name(), previousWord.c_str()) == 0)
			{
				for (auto&& config : patch->getConfigOptions())
				{
					if (!cnc(&a7->string, &config.first, a7->length()))
					{
						unkCarrier test;
						test.dword0 = a8;
						test.dword4 = a9;
						test.str.string = config.first;
						_AppendTabResult(a1, &test);
					}
				}
			}
		}
		*a2 = 1;
		return;
	}


	if (index == 3)
	{
		const auto firstWord = a5->dword0[index - 2].aostring.string;
		const auto secondWord = a5->dword0[index - 1].aostring.string;
		
		for (auto&& patch : PatchManager::getInstance().Patches)
		{
			if (strcmp(patch->Name(), firstWord.c_str()) == 0)
			{
				for (auto&& config : patch->getConfigOptions())
				{
					if (config.first == secondWord)
					{
						if (const auto pBool = std::get_if<bool>(&config.second.Value))
						{
							unkCarrier bFalse;
							bFalse.dword0 = a8;
							bFalse.dword4 = a9;
							bFalse.str.string = "false";
							unkCarrier bTrue;
							bTrue.dword0 = a8;
							bTrue.dword4 = a9;
							bTrue.str.string = "true";
							_AppendTabResult(a1, &bFalse);
							_AppendTabResult(a1, &bTrue);
						}
						else
						{
							std::stringstream ss;
							std::visit([&ss](auto&& arg) {ss << arg; }, config.second.Value);
							unkCarrier currentValue;
							currentValue.dword0 = a8;
							currentValue.dword4 = a9;
							currentValue.str.string = ss.str();
							_AppendTabResult(a1, &currentValue);
						}
					}

					
				}
			}
		}
		*a2 = 1;
	}


}


void InitializeOurCommands(int cgenericHandler)
{
	const auto pf = std::make_unique<PatternFinder>("Gui");

	auto getFunctionAddress = [](int address)
	{
		const auto ptr = reinterpret_cast<int*>(address + 1);
		return (address + 5) + (*ptr);
	};

	auto pfResults = pf->FindPattern("E8 ? ? ? ? 53 6A 01 8D 4D CC 89 45 F0 C6 45 FC 03 E8 ? ? ? ? 8B 4D F0 68 ? ? ? ?", getFunctionAddress);

	if (pfResults.size() > 1)
	{
		for (unsigned i = 0; i < pfResults.size(); i++)
		{
			if (pfResults[i] != pfResults[0])
			{
				spdlog::error("CommandInterpreter could not determine if we found the correct RegisterCommand function");
				return;
			}
		}
	}
	else
	{
		spdlog::error("CommandInterpreter failed to find RegisterCommandHandler");
		return;
	}

	const auto RegisterCommand = reinterpret_cast<RegisterCommandFunc>(pfResults[0]);

	pfResults = pf->FindPattern("E8 ? ? ? ? 8B 4D F0 68 ? ? ? ? 57 83 C1 04", getFunctionAddress);

	if (pfResults.size() > 1)
	{

		for (unsigned i = 0; i < pfResults.size(); i++)
		{
			if (pfResults[i] != pfResults[0])
			{
				spdlog::error("CommandInterpreter could not determine if we found the correct RegisterCommandHandler function");
				return;
			}
		}
	}
	else
	{
		spdlog::error("CommandInterpreter failed to find RegisterCommandHandler");
		return;
	}

	const int RegisterCommandHandler = pfResults[0];

	int RegisterTabHandler_Generic = 0;

	pfResults = pf->FindPattern("E8 ? ? ? ? 6A 05 68 ? ? ? ? 8D 4D CC 89 75 E0 89 5D DC 88 5D CC", getFunctionAddress);

	if (pfResults.size() > 1)
	{
		for (int pfResult : pfResults)
		{
			if (pfResult != RegisterCommandHandler)
			{
				RegisterTabHandler_Generic = pfResult;
				break;
			}
		}


		if (RegisterTabHandler_Generic == 0)
		{
			spdlog::error("CommandInterpreter could not determine if we found the correct RegisterTabHandler_Generic function");
			return;
		}

	}
	else
	{
		spdlog::error("CommandInterpreter failed to find RegisterTabHandler_Generic");
		return;
	}

	const auto ChatGUIModule_Instance = Patch::GetAddress<int*>("Gui", "?s_pcInstance@ChatGUIModule_c@@0PAV1@A");



	pfResults = pf->FindPattern("E8 ? ? ? ? 53 6A 01 8D 4D 9C 88 5D FC E8 ? ? ? ? 83 C7 70 3B 7D E4 75 98", getFunctionAddress);
	if (pfResults.size() != 1)
	{
		spdlog::error("CommandInterpreter failed to find _AppendTabResult");
		return;
	}
	_AppendTabResult = reinterpret_cast<AppendTabResult>(pfResults[0]);


	auto readInt = [](int address)
	{
		return *reinterpret_cast<int*>(address + 2);
	};

	pfResults = pf->FindPattern("83 25 ? ? ? ? ? 8B 46 0C 8B 38 C7 45 ? ? ? ? ? 89 7D F0 3B F8 74 25", readInt);
	if (pfResults.size() != 1)
	{
		spdlog::error("CommandInterpreter failed to find CommandInterpreter_c");
		return;
	}



	const auto CommandInterpreter_c = *reinterpret_cast<int*>(pfResults[0]);



	AOString command;
	command.string = "/aopp";
	command.unk = 0;

	const auto result = RegisterCommand(CommandInterpreter_c, &command, 4, 0, 0);


	reinterpret_cast<RegisterHandlerFunc>(RegisterCommandHandler)(result, cgenericHandler, std::addressof(HandleCommand));
	reinterpret_cast<RegisterHandlerFunc>(RegisterTabHandler_Generic)(result + 4, cgenericHandler, std::addressof(HandleTabPress));

	spdlog::info("Registered /aopp command");
}





__declspec(noinline) int __fastcall initializeGenericCommandInterpreter(void* a1, void* EDX)
{
	typedef int(__thiscall* originalFunction)(void*);
	const auto result = reinterpret_cast<originalFunction>(cGenericInitializationHook->originalTramp)(a1);

	InitializeOurCommands(result);
	cGenericInitializationHook->detour->unHook();

	return result;
}

char* cGenericInitializationAdder()
{
	const auto pf = std::make_unique<PatternFinder>("Gui");

	auto pfResults = pf->FindPattern("B8 ? ? ? ? E8 ? ? ? ? 83 EC 28 53 56 57 8B F9 89 7D E8 FF 15 ? ? ? ?");

	if (pfResults.size() != 1)
	{
		spdlog::error("CommandInterpreter failed to find cGenericInitialization function");
		return nullptr;
	}

	//const auto ptr = reinterpret_cast<int*>(pfResults[0] + 1);
	//const auto jumpLoc = (pfResults[0] + 5) + (*ptr);
	//std::cout << "JUMP LOC :" << std::hex << jumpLoc << std::endl;


	return reinterpret_cast<char*>(pfResults[0]);
}

CommandInterpreter::CommandInterpreter()
{
	 cnc = GetAddress<CompareNoCase>("Utils", "?CompareNoCase@String@@SAHABV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@0H@Z");

	
	const auto hookAddress = cGenericInitializationAdder();
	spdlog::info("CommandInterpreter hook address {:x}", (int)hookAddress);
	if (cGenericInitializationHook == nullptr && hookAddress != nullptr)
		cGenericInitializationHook = new hookData("Gui", "cGenericInitialization", hookAddress, &initializeGenericCommandInterpreter);

	ApplyHook(cGenericInitializationHook);
}
