#include "Utils.h"

#include <fstream>
#include <sstream>
#include <shlwapi.h>
#include <shlobj_core.h>

std::filesystem::path Utils::AppPath()
{
	static std::filesystem::path path;

	if (!path.empty())
		return path;
	
	TCHAR szPath[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath)))
	{
		path.assign(szPath);
		path /= "AOPP";

		if (!exists(path))
		{
			create_directories(path);
		}
		
	}
	return path;
}

std::filesystem::path Utils::LogsFolder()
{
	static std::filesystem::path path;

	if (!path.empty())
		return path;

	path = AppPath() /= "Logs";

	if (!exists(path))
	{
		create_directories(path);
	}

	return path;
}


std::filesystem::path Utils::ConfigFile()
{
	static std::filesystem::path path;

	if (!path.empty())
		return path;

	path = AppPath() /= "config.json";

	if (!exists(path))
	{
		std::ofstream config(path);
		config << "{}";
	}

	return path;
}

bool Utils::stob(const std::string& a) 
{
	std::string cpy = a;
	std::transform(a.begin(), a.end(), cpy.begin(), ::tolower);

	bool result{};

	std::istringstream is(a);
	is >> result;
	if (is.fail())
	{
		is.clear();
		is >> std::boolalpha >> result;
	}

	if (is.fail())
	{
		throw std::invalid_argument(std::string("Invalid value " + a));
	}
	
	return result;
}
