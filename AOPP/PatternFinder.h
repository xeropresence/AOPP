#pragma once
#include <functional>
#include <string>
#include <vector>


class PatternFinder
{
public:
	PatternFinder(const std::string& module);
	[[nodiscard]] std::vector<intptr_t> FindPattern(const std::string& pattern) const;

	
	[[nodiscard]] std::vector<intptr_t> FindPattern(const std::string& pattern, const std::function<intptr_t(intptr_t)>&
	                                                visitor) const;

private:
	int _baseAddress = 0;
	int _dataLength = 0;
	int SizeOfCode = 0;
	int CodeBase = 0;
	
	
	signed char* _data;
	std::vector<intptr_t> Find(std::vector<int>&& pattern) const;

	static bool ByteMatch(signed char* bytes, int start, std::vector<int>& pattern);
};

