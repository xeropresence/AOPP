#pragma once
#include <string>

struct Identity_t
{
	unsigned int Type;
	unsigned int Key;
};


//The size of their std::string is actually 28 bytes long
//unsure of what the additional 4 byte difference is
struct AOString
{
	std::string string;
	int unk;

	[[nodiscard]] size_t length() const { return string.length(); }


	operator const char*() const { return string.c_str(); }
	operator const std::string() const  { return string; }
};


struct PrivateMessage_t
{
	unsigned int dword0;
	unsigned int dword4;
	unsigned int dword8;
	AOString Sender;
	AOString Message;
	char* pbyte44;
	unsigned int dword48;

	PrivateMessage_t() = delete;
	
};

