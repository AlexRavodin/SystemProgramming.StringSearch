#include "FileWriter.h"

void FileWriter::write(std::wstring message)
{
    DWORD writtenBytesCount;
    const DWORD requiredBytesCount = message.size() * sizeof(wchar_t);

    EnterCriticalSection(section);

    const bool result = WriteFile(file, message.data(), requiredBytesCount, &writtenBytesCount, NULL);

    LeaveCriticalSection(section);

    if (!result || requiredBytesCount != writtenBytesCount)
    {
        DWORD writtenCount;
        const std::wstring errorMessage = L"Can not write to the file.\n" + message;

        const HANDLE stream = GetStdHandle(STD_ERROR_HANDLE);

        WriteConsole(stream, errorMessage.c_str(), static_cast<DWORD>(errorMessage.size()), &writtenCount, NULL);
    }

}

void FileWriter::closeHandle() const
{
    CloseHandle(file);
}
