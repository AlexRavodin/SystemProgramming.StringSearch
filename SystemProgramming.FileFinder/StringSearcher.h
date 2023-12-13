#pragma once

#include <vector>

#include "Encoding.h"
#include "FileInfo.h"
#include "SearchInfo.h"
#include "Writer.h"

struct StringSearcher
{
public:
	StringSearcher(const std::vector<FileInfo>& _files, std::wstring _string,
		int _maxThreadCount, Writer* _resultWriter, Writer* _errorWriter,
		Encoding encoding, bool _isCaseSensitive);

private:
	std::vector<FileInfo> files;
	std::wstring string;
	int maxThreadCount;
	Encoding encoding;
	bool isCaseSensitive;

	Writer* resultWriter;
	Writer* errorWriter;

	std::wstring result;


	void MultithreadSearch();

	static DWORD WINAPI FindStringInFile(LPVOID parameter);
};
