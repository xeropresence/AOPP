#pragma once
#include <ostream>
#include <utility>


#include "headers/Detour/x86Detour.hpp"

struct hookData
{
	std::string Module;
	std::string FunctionName;


	hookData(std::string module, std::string mangledName, void* replacement);
	hookData(std::string module, std::string DummyName, const char* patchLocation, void* replacement);
	
	const char* patchLocationPtr;
	const char* replacementFunctionPtr;

	
	uint64_t originalTramp;

	PLH::x86Detour* detour;
};

class Patch
{


public:


	[[nodiscard]] bool enabled() const { return this->_enabled; }

	bool ApplyPatch();;
	bool RemovePatch();;

	template <typename T>
	static T GetAddress(const std::string& module, const std::string& functionName)
	{
		const auto moduleHandle = GetModuleHandleA(module.c_str());
		if (moduleHandle == nullptr)
		{
			throw std::runtime_error("Failed to get module handle " + GetLastError());
		}

		auto procAddress = GetProcAddress(moduleHandle, functionName.c_str());
		if (procAddress == nullptr)
		{
			throw std::runtime_error("Failed to get function address " + GetLastError());
		}


		return reinterpret_cast<T>(procAddress);
	}

private:
	bool _enabled = false;

	
	
protected:

	virtual bool _ApplyPatch() { return false; };
	virtual bool _RemovePatch();;



	
	std::list<hookData*> appliedHooks;
	bool ApplyHook(hookData* hook);
	bool RemoveHook(hookData* hook);
};

