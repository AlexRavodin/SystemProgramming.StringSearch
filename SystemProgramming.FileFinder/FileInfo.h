#pragma once

#include <windows.h>

#include <string>

struct FileInfo
{
	std::wstring filePath;
    UINT64 fileSize;

    FileInfo(std::wstring _filePath, UINT64 _fileSize) : filePath(std::move(_filePath)), fileSize(_fileSize) { }
};
