#pragma once
#include <nlohmann/json.hpp>
#include <ostream>
#include <utility>
#include "spdlog/spdlog.h"

#include "polyhook2/Detour/x86Detour.hpp"

#include <variant>

#include "Utils.h"
class Patch;
struct ConfigOption;


using PatchPair = std::pair<ConfigOption, std::function<void(std::variant<bool, int, float>)>>;
using PatchConfig = std::unordered_map<std::string, PatchPair>;

struct ConfigOption
{
	std::variant<bool, int, float> Value;

	template<typename N>
	ConfigOption(N defaultValue)
	{
		//static_assert(std::is_same<N, int>::value || std::is_same<N, float>::value || std::is_same<N, bool>::value, "idiot, we can't take that type");
		Value = defaultValue;
	}

	operator bool() const { return std::get<bool>(Value); }
	operator int() const { return std::get<int>(Value); }
	operator float() const { return std::get<float>(Value); }
};


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


	[[nodiscard]] bool enabled() const { return _configuration.at("enabled").first; }

	bool ApplyPatch();
	bool RemovePatch();

	template <typename T>
	static T GetAddress(const std::string& module, const std::string& functionName)
	{
		const auto moduleHandle = GetModuleHandleA(module.c_str());
		if (moduleHandle == nullptr)
		{
			throw std::runtime_error(std::string("Failed to get module handle " + module));
		}

		auto procAddress = GetProcAddress(moduleHandle, functionName.c_str());
		if (procAddress == nullptr)
		{
			throw std::runtime_error(std::string("Failed to get function address " + functionName));
		}


		return reinterpret_cast<T>(procAddress);
	}

	void Initialize(nlohmann::json & config);

	[[nodiscard]] auto Name() const
	{
		return typeid(*this).name() + 6;
	}
	std::vector<std::pair<std::string, ConfigOption>> getConfigOptions();


	void setConfigOption(const std::string& configOption, const std::string& value);
private:
	bool _enabled = false;
	
protected:

	void setState(bool newState);
	
	virtual bool _ApplyPatch();;
	virtual bool _RemovePatch();

	
	PatchConfig _configuration{ {"enabled",{true,[&] (auto && val){setState(std::get<bool>(val)); }}} };
	
	std::list<hookData*> appliedHooks;
	bool ApplyHook(hookData* hook);
	bool RemoveHook(hookData* hook);
};

