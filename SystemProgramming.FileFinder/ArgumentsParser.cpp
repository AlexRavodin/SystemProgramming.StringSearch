#include "ArgumentsParser.h"

#include "Utilty.h"

ArgumentsParser::ArgumentsParser(int _argumentCount, const wchar_t* _argumentString[]) :
		maxThreadCount(8), errorOccured(false), isFileErrorOutput(false),
		isFileOutput(false), encoding(UTF8), isCaseSensitive(false), argumentCount(_argumentCount)
{
for (int i = 0; i < argumentCount; i++)
		arguments.emplace_back(_argumentString[i]);

	if (argumentCount < 3)
	{
		errorOccured = true;
		resultMessage = helpString;
	}

	searchString = arguments[1];

	for (int i = 2; i < argumentCount - 1 && !errorOccured; i++)
	{
		if (i % 2 != 0)
		{
			errorOccured = true;
			resultMessage = L"Wrong argument format.\n";
			break;
		}

		if (arguments[i] == std::wstring(L"-t"))
		{
			i++;
			try
			{
				maxThreadCount = std::stoi(arguments[i]);
			}
			catch (...)
			{
				errorOccured = true;
				resultMessage = L"Wrong thread count argument.\n";
			}
			if (!errorOccured && (maxThreadCount > 16 || maxThreadCount < 1))
			{
				errorOccured = true;
				resultMessage = L"Wrong thread count argument.\n";
			}
		}
		else if (arguments[i] == L"-d")
		{
			i++;
			folders.push_back(arguments[i]);
		}
		else if (arguments[i] == L"-f")
		{
			i++;
			files.push_back(arguments[i]);
		}
		else if (arguments[i] == L"-o")
		{
			i++;
			isFileOutput = true;
			outputFilePath = arguments[i];
		}
		else if (arguments[i] == L"-l")
		{
			i++;
			isFileErrorOutput = true;
			errorFilePath = arguments[i];
		}
		else if (arguments[i] == L"-e")
		{
			i++;
			extensions.insert(arguments[i]);
		}
		else if (arguments[i] == L"-c")
		{
			i++;
			if (!Utility::EncodingExists(arguments[i]))
			{
				errorOccured = true;
				resultMessage = L"Wrong encoding name.\n";
			}
			else
			{
				encoding = Utility::getEncoding(arguments[i]);
			}
		}
		else if (arguments[i] == L"-i")
		{
			i++;
			if (arguments[i] == L"sens")
			{
				isCaseSensitive = true;
			}
			else if (arguments[i] == L"nonsens")
			{
				isCaseSensitive = false;
			}
			else
			{
				errorOccured = true;
				resultMessage = L"Wrong sensitivity parameter.\n";
			}
		}
		else
		{
			errorOccured = true;
			resultMessage = L"Wrong arguments.\n";
		}
	}
}
