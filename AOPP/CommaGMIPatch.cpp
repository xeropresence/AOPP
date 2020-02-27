#include "CommaGMIPatch.h"
#include "PatternFinder.h"


hookData* DoLoopHook = nullptr;


#pragma region Headers

#define __int8 char
#define __int16 short
#define __int32 int
#define __int64 long long
enum iostate
{
	STATE_NONE = 0x0,
	STATE_SENDING_HEADERS = 0x1,
	STATE_SENDING_BODY = 0x2,
	STATE_REQUEST_SENT = 0x3,
	STATE_READ_HEADERS = 0x4,
	STATE_READ_HEADERS_COMPLETE = 0x5,
	STATE_BODY_PENDING = 0x6,
	STATE_READ_BODY = 0x7,
	STATE_READ_BODY_COMPLETE = 0x8,
	STATE_DONE = 0x9,
};

struct __declspec(align(4)) read_buf
{
	__int32 dword0;
	__int32 dword4;
	char* data;
	char* StartOfBuffer;
	__int32 capacity_;
	__int32 offset_;
};

struct __declspec(align(8)) HttpStreamParser
{
	iostate io_state_;
	__int32 dword4;
	__int32 dword8;
	__int32 dwordC;
	read_buf* read_buf_;
	__int32 read_buf_unused_offset_;
	__int32 dword18;
	__declspec(align(8)) __int32 dword20;
	__int32 dword24;
	__int32 dword28;
	__int32 dword2C;
	__int32 chunked_decoder_;
	read_buf* user_read_buf_;
	__int32 dword38;
	__int32 user_callback_;
	__int32 gap40;
	__int32 dword44;
	__int32 dword48;
	__int32 dword4C;
	__int32 dword50;
	__int32 dword54;
};


#pragma endregion

char* HttpStreamParser__DoLoopFuncAdder()
{
	const auto pf = std::make_unique<PatternFinder>("Awesomium");

	auto pfResults = pf->FindPattern("E8 ? ? ? ? 83 F8 FF 75 0A 8B 8C 24 ? ? ? ? 89 4D 3C ");

	if (pfResults.size() != 1)
	{
		spdlog::error("CommaGMIPatch failed to find function to hook");
		return nullptr;
	}

	const auto ptr = reinterpret_cast<int*>(pfResults[0] + 1);
	const auto jumpLoc = (pfResults[0] + 5) + (*ptr);
	//std::cout << "JUMP LOC :" << std::hex << jumpLoc << std::endl;

	return reinterpret_cast<char*>(jumpLoc);
}

const std::string replacement(".val().replace(/,/g, '')");
const std::string target(".val()");
__declspec(noinline) int __fastcall HttpStreamParser__DoLoop(HttpStreamParser* a1, void* EDX, int a2)
{
	typedef int(__thiscall* OriginalFunctionType)(HttpStreamParser*, int);


	auto result = reinterpret_cast<OriginalFunctionType>(DoLoopHook->originalTramp)(a1, a2);

	if (a1->io_state_ == STATE_DONE && result > 0)
	{
		std::string tmp;
		tmp.assign(a1->user_read_buf_->data, result);
		const auto search = tmp.find(target);

		if (search != std::string::npos)
		{
			const auto lengthDelta = replacement.length() - target.length();
			tmp.replace(search, target.length(), replacement);
			memcpy(reinterpret_cast<char*>(a1->user_read_buf_->data), tmp.c_str(), tmp.length());
			result += lengthDelta;
		}
		
	}
	return result;
}


bool CommaGMIPatch::_ApplyPatch()
{
	if (DoLoopHook == nullptr)
		DoLoopHook = new hookData("Awesomium", "HttpStreamParser::DoLoopFuncAdder", HttpStreamParser__DoLoopFuncAdder(), &HttpStreamParser__DoLoop);

	ApplyHook(DoLoopHook);
	return true;
}
