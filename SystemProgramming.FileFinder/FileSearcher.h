#pragma once

#include <set>
#include <string>
#include <vector>

#include "FileInfo.h"
#include "Writer.h"

struct FileSearcher
{
public:
	FileSearcher(std::wstring _initialFolder, int _maxThreadCount,
		const std::set<std::wstring>& extensions, Writer* _errorWriter);

	std::vector<FileInfo> files;
	std::set<std::wstring> extensions;
private:
	std::wstring initialFolder;
	int maxThreadCount;
	Writer* errorWriter;

	std::vector<std::wstring> folders;

	void MultithreadSearch();

	static DWORD WINAPI FindFilesAndFolders(LPVOID parameter);
};
