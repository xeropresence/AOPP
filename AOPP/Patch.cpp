#include "Patch.h"

#include "dbghelp.h"
#pragma comment(lib, "dbghelp.lib")

#include "polyhook2/CapstoneDisassembler.hpp"


#include <variant>

#include "PatchManager.h"

static PLH::CapstoneDisassembler dis(PLH::Mode::x86);

hookData::hookData(std::string module, std::string mangledName, void* replacement)
{
	Module = std::move(module);
	FunctionName = std::move(mangledName);
	replacementFunctionPtr = reinterpret_cast<char*>(replacement);
	detour = nullptr;
	originalTramp = 0;

	patchLocationPtr = Patch::GetAddress<const char*>(Module, FunctionName);

	
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
	if (!_enabled)
	{
		spdlog::info("Activating {}", Name());
		if (_ApplyPatch())
		{
			spdlog::info("{} Activated", Name());
			_enabled = true;
			return true;
		}
	}
	return false;
}

bool Patch::RemovePatch()
{
	if (_enabled)
	{
		spdlog::info("Deactivating {}", Name());
		if (_RemovePatch())
		{
			spdlog::info("{} Deactivated", Name());
			_enabled = false;
			return true;
		}
	}
	return false;
}

void Patch::Initialize(nlohmann::json & config)
{
	if (config.count(Name()) == 0)
		config[Name()] = nlohmann::json::object();

	nlohmann::json& localConfig = config[Name()];

	for (auto&& [key, value] : _configuration)
	{
		std::visit([&](auto&& arg)
		{
			//spdlog::info("Visiting {}", key);
			if (localConfig.count(key) == 0)
			{
				localConfig[key] = arg;
			}
			else
			{
				arg = localConfig[key];
			}
		}, value.first.Value);
		//
		//if (localConfig.count(key) == 0)
		//{
		//	if (const auto pint = std::get_if<int>(&value.Value))
		//		localConfig[key] = *pint;
		//	else if (const auto pFloat = std::get_if<float>(&value.Value))
		//		localConfig[key] = *pFloat;
		//	if (const auto pBool = std::get_if<bool>(&value.Value))
		//		localConfig[key] = *pBool;		
		//}
		//else
		//{
		//	if (const auto pint = std::get_if<int>(&value.Value))
		//		*pint = localConfig[key];
		//	else if (const auto pFloat = std::get_if<float>(&value.Value))
		//		*pFloat = localConfig[key];
		//	if (const auto pBool = std::get_if<bool>(&value.Value))
		//		*pBool = localConfig[key];
		//}
			
		//spdlog::info("key: {} val:{}", key, std::get<int>(value.Value));
	}
}

std::vector<std::pair<std::string, ConfigOption>> Patch::getConfigOptions()
{
	std::vector<std::pair<std::string, ConfigOption>> res;
	for (auto&& kvp : _configuration)
		res.emplace_back(kvp.first, kvp.second.first);

	return res;
}

void Patch::setConfigOption(const std::string& configOption, const std::string& value)
{
	auto& kvp = _configuration.at(configOption);


	if (const auto pint = std::get_if<int>(&kvp.first.Value))
	{
		try
		{
			const auto conversionResult = std::stoi(value);
			kvp.first = conversionResult;
			kvp.second(conversionResult);
			PatchManager::getInstance().Configuration->setValue(Name(), configOption, conversionResult);
		}
		catch (std::invalid_argument&)
		{
			spdlog::error("Failure to convert {} to int", value);
		}
	}
	else if (const auto pFloat = std::get_if<float>(&kvp.first.Value))
	{
		try
		{
			const auto conversionResult = std::stof(value);
			kvp.first = conversionResult;
			kvp.second(conversionResult);
			PatchManager::getInstance().Configuration->setValue(Name(), configOption, conversionResult);

		}
		catch (std::invalid_argument&)
		{
			spdlog::error("Failure to convert {} to float", value);
		}
	}
	if (const auto pBool = std::get_if<bool>(&kvp.first.Value))
	{
		try
		{
			const auto conversionResult = Utils::stob(value);
			kvp.first = conversionResult;
			kvp.second(conversionResult);
			PatchManager::getInstance().Configuration->setValue(Name(), configOption, conversionResult);

		}
		catch (std::invalid_argument&)
		{
			spdlog::error("Failure to convert {} to bool", value);
		}
	}
}

void Patch::setState(bool newState)
{
	if (newState)
	{
		ApplyPatch();
	}
	else
	{
		RemovePatch();
	}
}

bool Patch::_ApplyPatch()
{
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

	char bufout[8192]{};
	DWORD ret = UnDecorateSymbolName(hook->FunctionName.c_str(), bufout, 8192, UNDNAME_NAME_ONLY);
	
	if (!result)
	{
		spdlog::error("Failed to apply hook for {}[{},{}] {}", Name(),hook->Module, bufout, PLH::ErrorLog::singleton().pop().msg);
		return false;
	}

	spdlog::info("Installed hook for {}[{},{}] {:x}-->{:x}", Name(), hook->Module, bufout, reinterpret_cast<int>(hook->patchLocationPtr), hook->originalTramp);
	
	appliedHooks.push_back(hook);

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
