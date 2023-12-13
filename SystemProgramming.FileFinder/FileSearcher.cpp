#include "FileSearcher.h"

#include <filesystem>
#include <iostream>

#include "ScanInfo.h"

using std::wstring;

FileSearcher::FileSearcher(std::wstring _initialFolder, const int _maxThreadCount,
                           const std::set<std::wstring>& _extensions, Writer* _errorWriter) :
	extensions(_extensions), initialFolder(std::move(_initialFolder)), maxThreadCount(_maxThreadCount),
	errorWriter(_errorWriter)
{
	MultithreadSearch();
}

void FileSearcher::MultithreadSearch()
{
    std::vector<ScanInfo*> scanInfos;
    std::vector<HANDLE> threads;

    folders.push_back(initialFolder);

    while (!folders.empty() || !threads.empty())
    {
        if (threads.size() == maxThreadCount || folders.empty())
        {
            DWORD newThreadIndex = WaitForMultipleObjects(threads.size(), threads.data(), FALSE, INFINITE);

            if (!(newThreadIndex >= WAIT_OBJECT_0 && newThreadIndex <= WAIT_OBJECT_0 + maxThreadCount - 1))
            {
                //GetWrongWaitObjectInfo(newThreadIndex, maxThreadCount);
                this->errorWriter->write(L"Wrong wait object.\n");
                continue;
            }

            newThreadIndex -= WAIT_OBJECT_0;

            CloseHandle(threads[newThreadIndex]);

            if (scanInfos[newThreadIndex]->ErrorOccured)
            {
                errorWriter->write(scanInfos[newThreadIndex]->errorMessage);
                threads.erase(threads.begin() + newThreadIndex);
                scanInfos.erase(scanInfos.begin() + newThreadIndex);
                continue;
            }

            for (const auto& folder : scanInfos[newThreadIndex]->folders)
            {
                folders.push_back(folder);
            }
            for (const auto& file : scanInfos[newThreadIndex]->files)
            {
                files.push_back(file);
            }
            scanInfos[newThreadIndex]->folders.clear();
            scanInfos[newThreadIndex]->files.clear();

            threads.erase(threads.begin() + newThreadIndex);
            scanInfos.erase(scanInfos.begin() + newThreadIndex);
        }
        else
        {
	        const int threadIndex = threads.size();

            scanInfos.push_back(new ScanInfo(folders.back(), extensions));
            folders.pop_back();

            threads.push_back(CreateThread(
                NULL,
                0,
                FindFilesAndFolders,
                scanInfos[threadIndex],
                0,
                NULL));

            if (threads.back() == NULL)
            {

                this->errorWriter->write(L"Wrong thread object.\n");
            }
        }
    }
}

DWORD WINAPI FileSearcher::FindFilesAndFolders(const LPVOID parameter)
{
	const auto scanInfo = static_cast<ScanInfo*>(parameter);

    const wstring initialPath = scanInfo->initialPath;

	const wstring startFile = initialPath + L"\\*";

	WIN32_FIND_DATA fileData{};
	const HANDLE hFile = FindFirstFile(startFile.c_str(), &fileData);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		scanInfo->ErrorOccured = true;
		scanInfo->errorMessage = L"Error accessing folder: " + initialPath + L"\n";

		return 1;
	}

	do
	{
		std::wstring currentPath = initialPath + L"\\" + fileData.cFileName;

		if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (wstring(fileData.cFileName) != L"." &&
				wstring(fileData.cFileName) != L"..")
			{
				scanInfo->folders.push_back(currentPath);
			}
		}
		else
		{
			if (scanInfo->filters.contains(std::filesystem::path(currentPath).extension()))
			{
				UINT64 fileSize = fileData.nFileSizeHigh;
				fileSize <<= 31;
				fileSize += fileData.nFileSizeLow;

				scanInfo->files.emplace_back(currentPath, fileSize);
			}
		}
	} while (FindNextFile(hFile, &fileData));

	FindClose(hFile);

	return 0;
}
