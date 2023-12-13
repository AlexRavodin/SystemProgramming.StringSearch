#ifndef UNICODE
#define UNICODE
#endif

#include <string>
#include <filesystem>
#include <iostream>
#include <vector>

#include "FileSearcher.h"
#include "ArgumentsParser.h"
#include "StringSearcher.h"
#include "Utilty.h"

using std::wstring;

int wmain(int argc, const wchar_t** argv)
{
    setlocale(LC_ALL, "ru_ru.UTF8");

    const ArgumentsParser argumentsParser(argc, argv);

    if (argumentsParser.errorOccured)
    {
		std::wcout << argumentsParser.resultMessage;
        return 1;
    }

    CRITICAL_SECTION section;
    bool isCriticalSectionNeeded = false;
    Writer* resultWriter;

    if (argumentsParser.isFileOutput)
    {
        isCriticalSectionNeeded = true;
        resultWriter = Utility::getFileWriter(argumentsParser.outputFilePath, &section);

        if (resultWriter == nullptr)
        {
            return 1;
        }
    }
    else
    {
        resultWriter = Utility::getConsoleWriter(STD_OUTPUT_HANDLE);
    }

    Writer* errorWriter;

    if (argumentsParser.isFileErrorOutput)
    {
        isCriticalSectionNeeded = true;
        errorWriter = Utility::getFileWriter(argumentsParser.errorFilePath, &section);

        if (errorWriter == nullptr)
        {
            return 1;
        }
    }
    else
    {
        errorWriter = Utility::getConsoleWriter(STD_ERROR_HANDLE);
    }

    if (isCriticalSectionNeeded)
        InitializeCriticalSectionAndSpinCount(&section, 0x400);

    std::vector<FileSearcher> searchers;
    searchers.reserve(argumentsParser.folders.size());

    std::wcout << "Searching for files." << std::endl;

    for (const std::wstring& folder : argumentsParser.folders)
    {
        searchers.emplace_back(folder, argumentsParser.maxThreadCount, argumentsParser.extensions, errorWriter);
    }

    std::vector<FileInfo> files;

    for (const auto& file : argumentsParser.files)
    {
        FileInfo fileInfo = Utility::getFileInfo(file, errorWriter);

        if (fileInfo.fileSize != 0)
            files.push_back(fileInfo);
    }

    for (auto searcher : searchers)
    {
        files.insert(files.end(), searcher.files.begin(), searcher.files.end());
    }

    std::wcout << "Searching for string." << std::endl;

    StringSearcher stringSearcher(files, argumentsParser.searchString, argumentsParser.maxThreadCount,
        resultWriter, errorWriter, argumentsParser.encoding, argumentsParser.isCaseSensitive);

    if (isCriticalSectionNeeded)
        DeleteCriticalSection(&section);

    if (argumentsParser.isFileErrorOutput)
	    dynamic_cast<FileWriter*>(errorWriter)->closeHandle();

    if (argumentsParser.isFileOutput)
	    dynamic_cast<FileWriter*>(resultWriter)->closeHandle();

    delete resultWriter;
    delete errorWriter;

	std::wcout << "Everything is done." << std::endl;

    return 0;
}
