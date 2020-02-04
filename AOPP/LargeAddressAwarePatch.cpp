#include "LargeAddressAwarePatch.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <aclapi.h>


#include <shlwapi.h>
namespace fs = std::filesystem;

bool doFunkyStuff = false;

void SetLAA(const char* FileName, bool state)
{
	std::fstream baseExecutable(FileName, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate);
	if (baseExecutable.is_open())
	{
		const auto size = baseExecutable.tellg();
		auto memblock = new char[size] {};

		baseExecutable.seekg(0, std::ios::beg);
		baseExecutable.read(memblock, size);

		const auto pDosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(memblock);
		if (pDosHeader->e_magic == IMAGE_DOS_SIGNATURE)
		{
			const auto pImageHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<char*>(pDosHeader) + pDosHeader->e_lfanew);
			if (pImageHeader->Signature == IMAGE_NT_SIGNATURE)
			{
				if (state)
				{
					pImageHeader->FileHeader.Characteristics = pImageHeader->FileHeader.Characteristics | IMAGE_FILE_LARGE_ADDRESS_AWARE;
				}
				else
				{
					pImageHeader->FileHeader.Characteristics = pImageHeader->FileHeader.Characteristics & ~IMAGE_FILE_LARGE_ADDRESS_AWARE;
				}

				baseExecutable.seekg(0, std::ios::beg);

				baseExecutable.write(memblock, size);

				baseExecutable.flush();
			}
		}

		baseExecutable.close();

		delete[] memblock;
	}
}



#pragma region Have Write Access
extern  hookData standardExecvHook;
__declspec(noinline)intptr_t __cdecl standardExecv(const char* FileName, const char* const* Arguments)
{
	typedef intptr_t(__cdecl* originalFunctionHeader)(const char*, const char* const*);
	
	if (StrStrIA(FileName, "AnarchyOnline.exe") != nullptr)
	{
		SetLAA(FileName, true);
	}

	return  reinterpret_cast<originalFunctionHeader>(standardExecvHook.originalTramp)(FileName, Arguments);
}
hookData standardExecvHook("msvcr100", "_execv", &standardExecv);


extern  hookData fopenHook;
__declspec(noinline) int __cdecl _fopen(const char* FileName, const char* mode)
{
	typedef int(__cdecl* originalFunctionHeader)(const char* FileName, const char* mode);

	//printf("%s\n", FileName);

	if (StrStrIA(FileName, "AnarchyOnline.exe") != nullptr)
	{
		SetLAA(FileName, false);
	}

	return reinterpret_cast<originalFunctionHeader>(fopenHook.originalTramp)(FileName, mode);
}
hookData fopenHook("msvcr100", "fopen", &_fopen);
#pragma endregion


#pragma region Dont Have Write Access
#include <Shlobj.h>
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
extern hookData funkyExecvHook;
__declspec(noinline)intptr_t __cdecl funkExecv(const char* FileName, const char*  Arguments)
{
	typedef intptr_t(__cdecl* originalFunctionHeader)(const char* , const char* );


	if (StrStrIA(FileName, "AnarchyOnline.exe") != nullptr)
	{
		//printf("Doing funky\n");
		TCHAR szPath[MAX_PATH];
		if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL,0, szPath)))
		{
			//printf("appending path\n");
			PathAppend(szPath, L"AOPP");
			std::filesystem::path p(szPath);
			if (!exists(p))
			{
				create_directories(p);
			}

			if (exists(p))
			{
				auto exe = p / "AnarchyOnline.exe";
				std::error_code ec;
				std::filesystem::copy(FileName, exe,std::filesystem::copy_options::overwrite_existing, ec);
				SetLAA(exe.string().c_str(), true);


				WCHAR   DllPath[MAX_PATH] = { 0 };
				GetModuleFileNameW((HINSTANCE)&__ImageBase, DllPath, _countof(DllPath));

				auto filename = fs::path(DllPath);
				
				auto dll = p / filename.filename();
				
				std::filesystem::copy(DllPath,dll, std::filesystem::copy_options::overwrite_existing, ec);
				
				return  reinterpret_cast<originalFunctionHeader>(funkyExecvHook.originalTramp)(exe.string().c_str(), Arguments);
			}
			
		}	

	}



	return  reinterpret_cast<originalFunctionHeader>(funkyExecvHook.originalTramp)(FileName, Arguments);
}
hookData funkyExecvHook("msvcr100", "_execv", &funkExecv);

#pragma endregion


bool LargeAddressAwarePatch::_ApplyPatch()
{

	//Check if we have write permission for the active directory, we have to-do some janky stuff we if we don't
	//if (std::filesystem::exists("AnarchyOnline.exe"))
	//{
	//	HANDLE fileH = CreateFileA("AnarchyOnline.exe",
	//		GENERIC_WRITE,
	//		FILE_SHARE_READ | FILE_SHARE_WRITE,
	//		nullptr,
	//		OPEN_EXISTING,
	//		FILE_ATTRIBUTE_NORMAL,
	//		nullptr);
	//
	//	if (fileH != INVALID_HANDLE_VALUE)
	//	{
	//		doFunkyStuff = false;
	//		CloseHandle(fileH);
	//	}
	//	else
	//	{
	//		doFunkyStuff = true;
	//	}
	//}

	ApplyHook(&funkyExecvHook);

	/*if (doFunkyStuff)
	{
		//printf("Doing funky stuff\n");
		ApplyHook(&funkyExecvHook);
	}
	else
	{
		printf("Doing normal stuff\n");
		ApplyHook(&standardExecvHook);
		ApplyHook(&fopenHook);
	}*/



	return true;
}