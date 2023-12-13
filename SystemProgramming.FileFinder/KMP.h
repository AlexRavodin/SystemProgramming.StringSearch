#pragma once
#include <vector>

struct KMP
{
	explicit KMP(const std::vector<char>& _string);

	void prefixFunctionStep(const char newChar);

	bool addTextChar(const char newChar);
private:
	std::vector<char> stringToSearch;
	int stringLength;

	std::vector<int> prefixFunction;
	int functionValue;

	int textPosition;
};
