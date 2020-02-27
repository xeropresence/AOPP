#pragma once
#include <nlohmann/json.hpp>

class ConfigurationHolder
{
public:
	nlohmann::json config;

	ConfigurationHolder();

	template<typename N>
	void setValue(const std::string & patch,const std::string& key, N value)
	{
		config[patch][key] = value;

		Save();
	}
	
	void Save() const;
};

