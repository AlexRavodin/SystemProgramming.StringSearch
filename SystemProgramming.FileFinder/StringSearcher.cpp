#include "StringSearcher.h"

#include "FileInfo.h"
#include "FileWriter.h"
#include "ScanInfo.h"
#include "SearchInfo.h"
#include "Utilty.h"
#include "Encoding.h"
#include "KMP.h"

StringSearcher::StringSearcher(const std::vector<FileInfo>& _files, std::wstring _string,
                               const int _maxThreadCount, Writer* _resultWriter,
								Writer* _errorWriter, Encoding _encoding, bool _isCaseSensitive) :
	files(_files), string(std::move(_string)), maxThreadCount(_maxThreadCount), encoding(_encoding),
	isCaseSensitive(_isCaseSensitive), resultWriter(_resultWriter), errorWriter(_errorWriter)
{
	MultithreadSearch();
}

void StringSearcher::MultithreadSearch()
{
	std::vector<SearchInfo*> searchInfos;
	std::vector<HANDLE> threads;

	while (!files.empty() || !threads.empty())
	{
		if (threads.size() == maxThreadCount || files.empty())
		{
			DWORD newThreadIndex = WaitForMultipleObjects(threads.size(), threads.data(), FALSE, INFINITE);

			if (!(newThreadIndex >= WAIT_OBJECT_0 && newThreadIndex <= WAIT_OBJECT_0 + maxThreadCount - 1))
			{
				this->errorWriter->write(L"Wrong wait object.\n");
				continue;
			}

			newThreadIndex -= WAIT_OBJECT_0;

			CloseHandle(threads[newThreadIndex]);

			threads.erase(threads.begin() + newThreadIndex);
			searchInfos.erase(searchInfos.begin() + newThreadIndex);
		}
		else
		{
			const int threadIndex = threads.size();

			searchInfos.push_back(new SearchInfo(files.back(), string, encoding,
												isCaseSensitive, resultWriter, errorWriter));
			files.pop_back();

			threads.push_back(CreateThread(
				NULL,
				0,
				FindStringInFile,
				searchInfos[threadIndex],
				0,
				NULL));

			if (threads.back() == NULL)
			{
				this->errorWriter->write(L"Wrong thread object.\n");
				return;
			}
		}
	}
}

DWORD WINAPI StringSearcher::FindStringInFile(LPVOID parameter)
{
	const auto searchingData = static_cast<SearchInfo*>(parameter);

	const HANDLE hFile = CreateFile(searchingData->fileInfo.filePath.c_str(),
	                                GENERIC_READ,
	                                FILE_SHARE_READ,
	                                NULL,
	                                OPEN_EXISTING,
	                                FILE_ATTRIBUTE_NORMAL,
	                                NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		searchingData->errorWriter->write(L"Unable to open file " + searchingData->fileInfo.filePath + L" for read.\n");
		return 1;
	}

	constexpr int INITIAL_BUFFER_SIZE = 1024;
	char* initialBuffer = new char[INITIAL_BUFFER_SIZE] { 0 };
	DWORD dwBytesRead = 0;

	bool isFileReadSuccessful = ReadFile(hFile, initialBuffer, INITIAL_BUFFER_SIZE, &dwBytesRead, NULL);

	if (!isFileReadSuccessful)
	{
		searchingData->errorWriter->write(L"Unable to read from file " + searchingData->fileInfo.filePath + L"\n");
		CloseHandle(hFile);

		return 1;
	}

	if (dwBytesRead == 0)
	{
		CloseHandle(hFile);
		return 0;
	}

	if (dwBytesRead < 0 || dwBytesRead > INITIAL_BUFFER_SIZE)
	{
		searchingData->errorWriter->write(L"Unexpected read bytes count in file " + searchingData->fileInfo.filePath + L"\n");
		CloseHandle(hFile);

		return 1;
	}

	const DWORD dwPtr = SetFilePointer(hFile,
	                                   0,
	                                   NULL,
	                                   FILE_BEGIN);

	if (dwPtr == INVALID_SET_FILE_POINTER)
	{
		searchingData->errorWriter->write(L"Unable to move pointer in file " + searchingData->fileInfo.filePath + L"\n");
		CloseHandle(hFile);

		return 1;
	}

	Encoding forcedEncoding = searchingData->encoding;
	Encoding actualEncoding = Utility::determineEncoding(initialBuffer, dwBytesRead, true, forcedEncoding);

	if (!searchingData->isCaseSensitive)
	{
		std::ranges::transform(searchingData->string, searchingData->string.begin(), toupper);
	}

	const std::vector<char> bytesToFind = Utility::wstringToAnsiBytes(searchingData->string);

	if (searchingData->fileInfo.fileSize < bytesToFind.size())
	{
		CloseHandle(hFile);
		return 0;
	}

	if (actualEncoding != ANSI && actualEncoding != UTF8)
	{
		CloseHandle(hFile);
		return 0;
	}

	KMP kmp(bytesToFind);

	DWORD totalBytesRead = 0;
	dwBytesRead = 0;
	int lineNumber = 1;
	int positionNumber = 1;

	constexpr UINT64 BUFFER_SIZE = 2048;
	char* buffer = new char[BUFFER_SIZE] {0};

	do
	{
		isFileReadSuccessful = ReadFile(hFile, buffer, BUFFER_SIZE, &dwBytesRead, NULL);

		if (!isFileReadSuccessful)
		{
			searchingData->errorWriter->write(L"Unable to read from file " + searchingData->fileInfo.filePath);
			CloseHandle(hFile);

			return 1;
		}

		if (dwBytesRead < 0 || dwBytesRead > BUFFER_SIZE)
		{
			searchingData->errorWriter->write(L"Unexpected read bytes count in file: " + searchingData->fileInfo.filePath);
			CloseHandle(hFile);

			return 1;
		}

		for (int i = 0; i < dwBytesRead; i++)
		{
			if (buffer[i] == 0)
			{
				CloseHandle(hFile);

				return 1;
			}
			if (buffer[i] == '\n')
			{
				lineNumber++;
				positionNumber = 1;
			}
			else
			{
				positionNumber++;
			}

			if (!searchingData->isCaseSensitive)
				buffer[i] = toupper(buffer[i]);

			if (kmp.addTextChar(buffer[i]))
			{
				searchingData->positions.emplace_back(lineNumber, positionNumber - searchingData->string.size());
			}
		}

		totalBytesRead += dwBytesRead;
	}
	while (totalBytesRead < searchingData->fileInfo.fileSize);

	CloseHandle(hFile);

	searchingData->PrintResult();

	return 0;
}
