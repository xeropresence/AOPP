#include <windows.h>
#include <Psapi.h>
#include "PatchManager.h"
#include "LargeAddressAwarePatch.h"
#include "mswsock.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"

#include "nlohmann/json.hpp"

#include "Utils.h"


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{

		time_t ltm = time(nullptr);
		tm now{};

		localtime_s(&now, &ltm);

		CHAR buffer[32];
		GetModuleBaseNameA(GetCurrentProcess(), nullptr, buffer, 32);
			
		std::vector<spdlog::sink_ptr> sinks;
		auto logFilePath = Utils::LogsFolder() / buffer;
		logFilePath += ".log";
			
		sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logFilePath.string(), 1048576, 3));
			
		auto combined_logger = std::make_shared<spdlog::logger>("", begin(sinks), end(sinks));
		combined_logger->flush_on(spdlog::level::info);
		spdlog::set_default_logger(combined_logger);
		spdlog::info(buffer);

		spdlog::info("Starting up AOPP");





#define OpenConsole 0
#if OpenConsole
		AllocConsole();
		freopen_s(reinterpret_cast<FILE**>(stdin), "conin$", "r", stdin);
		freopen_s(reinterpret_cast<FILE**>(stdout), "conout$", "w", stdout);
		freopen_s(reinterpret_cast<FILE**>(stderr), "conout$", "w", stderr);
		combined_logger->sinks().push_back(std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>());

		//sinks.push_back(std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>());

#endif


		bool waitForDebugger = false;
		if (waitForDebugger)
		{
			spdlog::info("Waiting for debugger to attach");
			while (!::IsDebuggerPresent())
				::Sleep(100);
		}

		PLH::ErrorLog::singleton().setLogLevel(PLH::ErrorLevel::NONE);

		if (!mswsock::Attach())
		{
			return FALSE;
		}

		std::exception_ptr eptr;
		try
		{
			const auto& pm = PatchManager::getInstance();
		}
		catch (std::runtime_error & err)
		{
			spdlog::error(err.what());
		}

		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		mswsock::Dettach();
		break;
	default:;
	}
	return TRUE;
}
