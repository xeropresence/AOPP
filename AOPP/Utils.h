#pragma once

#include <filesystem>
//#include <nlohmann/json.hpp>

class Utils
{
public:
	static std::filesystem::path AppPath();

	static std::filesystem::path LogsFolder();
	
	static std::filesystem::path ConfigFile();
	
	static bool stob(const std::string& a); 
};

 