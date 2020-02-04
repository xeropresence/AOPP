#include "Patch.h"


#include "headers/CapstoneDisassembler.hpp"

static PLH::CapstoneDisassembler dis(PLH::Mode::x86);

hookData::hookData(std::string module, std::string mangledName, void* replacement)
{
	Module = std::move(module);
	FunctionName = std::move(mangledName);
	replacementFunctionPtr = reinterpret_cast<char*>(replacement);
	detour = nullptr;
	originalTramp = 0;

	const auto moduleHandle = GetModuleHandleA(Module.c_str());

	if (moduleHandle == nullptr)
	{
		std::cout << "Failed to find module " << Module << std::endl;
		return;
	}

	patchLocationPtr = reinterpret_cast<const char*>(GetProcAddress(moduleHandle, FunctionName.c_str()));

	if (patchLocationPtr == nullptr)
	{
		std::cout << "Failed to find function " << FunctionName << std::endl;
		return;
	}
	
}

hookData::hookData(std::string module, std::string DummyName, const char* patchLocation, void* replacement)
{
	Module = std::move(module);
	FunctionName = std::move(DummyName);
	replacementFunctionPtr = reinterpret_cast<char*>(replacement);
	detour = nullptr;
	originalTramp = 0;

	patchLocationPtr = patchLocation;
	
}


bool Patch::ApplyPatch()
{
	if (!_enabled && _ApplyPatch())
	{
		_enabled = true;
		return true;
	}
	return false;
}

bool Patch::RemovePatch()
{
	if (_enabled && _RemovePatch())
	{
		_enabled = false;
		return true;
	}
	return false;
}

bool Patch::_RemovePatch()
{
	auto i = std::begin(appliedHooks);

	while (i != std::end(appliedHooks))
	{
		const auto last = *i;
		++i;
		RemoveHook(last);
	}
	return appliedHooks.empty();
}

bool Patch::ApplyHook(hookData* hook)
{

	if (hook->patchLocationPtr == nullptr)
		return false;
	
	hook->detour = new PLH::x86Detour(hook->patchLocationPtr, hook->replacementFunctionPtr, &hook->originalTramp, dis);

	const auto result = hook->detour->hook();

	if (!result)
	{
		std::cout << "Failed to apply hook " << PLH::ErrorLog::singleton().pop().msg << std::endl;
	}
	else
	{
		appliedHooks.push_back(hook);
	}

	std::cout << "[" << typeid(*this).name() << "] " << std::hex << reinterpret_cast<int>(hook->patchLocationPtr) << "--->" << hook->originalTramp << std::endl;
	return true;
}

bool Patch::RemoveHook(hookData* hook)
{
	if (hook->detour->unHook())
	{
		appliedHooks.remove(hook);

		delete hook->detour;
		hook->detour = nullptr;
		
		return true;
	}
	return false;
}
