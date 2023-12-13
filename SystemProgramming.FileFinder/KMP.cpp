#include "KMP.h"

#include "cassert"

KMP::KMP(const std::vector<char>& _string) :
	stringToSearch(_string), stringLength(stringToSearch.size()),
	prefixFunction(stringLength + 1),
	functionValue(0), textPosition(0)
{
	stringToSearch.push_back(0);

	for (int i = 1; i <= stringLength; i++)
	{
		prefixFunctionStep(stringToSearch[i]);
		prefixFunction[i] = functionValue;
	}

	functionValue = 0;
}

void KMP::prefixFunctionStep(const char newChar)
{
	while (functionValue && newChar != stringToSearch[functionValue])
	{
		functionValue = prefixFunction[functionValue - 1];
	}

	if (newChar == stringToSearch[functionValue])
		functionValue++;
}

bool KMP::addTextChar(const char newChar)
{
	//assert(newChar != 0);
	textPosition++;
	prefixFunctionStep(newChar);
	return functionValue  == stringLength;
}
