#pragma once

#include <set>
#include <string>
#include <vector>

#include "Encoding.h"

//GOOD:
// hello -d D:\downloads -d D:\q -f D:\q.rtf -f D:\Curse\text.txt -o D:\res.txt -l D:\err.txt -t 7 -c UTF8 -e .txt

//ERROR:
// hello -d D:\downloads -d D:\q -f D:\q.rtf -f D:\Curse\text.txt -o D:\res.txt -l D:\err.txt -t 7 -c UTF8 -e .txt

// hello -d C:\Users -t 7 -e .txt

//a - f D : \downloads\q.txt - t 1 - e.txt

struct ArgumentsParser
{
public:
	ArgumentsParser(int _argumentCount, const wchar_t* _argumentString[]);

	std::vector<std::wstring> folders;
	std::vector<std::wstring> files;
	std::wstring searchString;
	int maxThreadCount;
	std::set<std::wstring> extensions;

	bool errorOccured;
	std::wstring resultMessage;

	bool isFileErrorOutput;
	std::wstring errorFilePath;

	bool isFileOutput;
	std::wstring outputFilePath;

	Encoding encoding;
	bool isCaseSensitive;
private:
	std::vector<std::wstring> arguments;
	int argumentCount;

	std::wstring helpString = std::wstring(L"Format: <string> [-d directory path]\n") +
		std::wstring(L"[-d directory path]\n") +
		std::wstring(L"[-o file path] [-f file path] [-l file path] [-i sensitivity(sens,nonsens)]\n") +
		std::wstring(L"[-e file extension(f.e. .txt] [-t thread count]\n\n") +
		std::wstring(L"Options:\n") +
		std::wstring(L"-d\tadd directory to scan.\n") +
		std::wstring(L"-f\tadd file to scan.\n") +
		std::wstring(L"-o\tredirect result output to file.\n") +
		std::wstring(L"-l\tredirect logs output to file.\n") +
		std::wstring(L"-e\tset extension to search.\n") +
		std::wstring(L"-t\tset max thread count.\n") +
		std::wstring(L"-i\tspecify case-sensitivity.\n");
};
