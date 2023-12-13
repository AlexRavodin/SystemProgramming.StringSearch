#pragma once

#include <queue>
#include <set>
#include <string>

#include "FileInfo.h"

struct ScanInfo {
	std::wstring initialPath;

	std::vector<std::wstring> folders;
    std::vector<FileInfo> files;

	std::set<std::wstring> filters;

    bool ErrorOccured;
    std::wstring errorMessage;

    ScanInfo(std::wstring _initialPath, const std::set<std::wstring>& _filters) : initialPath(std::move(_initialPath)),
	filters(_filters), ErrorOccured(false) { }
};
