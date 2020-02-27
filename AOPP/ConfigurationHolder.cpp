#include "ConfigurationHolder.h"
#include "Utils.h"
#include <fstream>

ConfigurationHolder::ConfigurationHolder()
{
	std::ifstream i(Utils::ConfigFile());
	try
	{
		i >> config;
	}
	catch (nlohmann::detail::parse_error & e)
	{
		config = nlohmann::json::object();
	}
}

void ConfigurationHolder::Save() const
{
	std::ofstream test(Utils::ConfigFile());
	test << std::setw(4) << config;
	test.close();
}
