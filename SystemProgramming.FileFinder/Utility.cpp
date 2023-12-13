#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include <codecvt>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <locale>
#include <set>
#include <vector>

#include "ConsoleWriter.h"
#include "Encoding.h"
#include "FileWriter.h"
#include "Utilty.h"
#include "Writer.h"

std::vector<char> ConvertVectorFromChar32ToCharLE(std::vector<char32_t> char32s);
std::vector<char> ConvertVectorFromChar32ToCharBE(std::vector<char32_t> char32s);

bool Utility::ContainsUtf8Exceptions(char buffer[], int bufferSize)
{
    for (int i = 0; i < bufferSize; i++)
    {
        if (impossibleUtf8Values.contains(buffer[i]) || buffer[i] > 127)
            return true;
    }

    return false;
}

std::wstring Utility::GetWrongWaitObjectInfo(DWORD newThreadIndex, const int maxThreadCount)
{
    if (newThreadIndex == WAIT_TIMEOUT)
        return L"Timeout";

    if (newThreadIndex == WAIT_FAILED)
        return L"Failed";

    if (newThreadIndex >= WAIT_ABANDONED_0 && newThreadIndex <= WAIT_ABANDONED_0 + maxThreadCount - 1)
        return L"Abandoned";

    return L"Other";
}

bool Utility::EncodingExists(const std::wstring& encodingString)
{
    return wstringToEncodingMap.contains(encodingString);
}

Encoding Utility::getEncoding(const std::wstring& encodingString)
{
    return wstringToEncodingMap.at(encodingString);
}

std::wstring Utility::getEncodingString(Encoding encoding)
{
    return encodingToWstringMap.at(encoding);
}

Encoding Utility::determineEncoding(char buffer[], int bufferSize, bool isForced, Encoding forcedEncoding)
{
    if (bufferSize < 2)
    {
        return isForced ? forcedEncoding : UTF8;
    }

    if (buffer[0] == '\xFE' && buffer[1] == '\xFF')
        return UTF16BE;
    if (buffer[0] == '\xFF' && buffer[1] == '\xFE')
        return UTF16LE;

    if (bufferSize < 3)
        return isForced ? forcedEncoding : UTF8;

    if (buffer[0] == '\xEF' && buffer[1] == '\xBB' && buffer[2] == '\xBF')
        return UTF8;

    if (bufferSize < 4)
        return isForced ? forcedEncoding : UTF8;

    if (buffer[0] == '\x00' && buffer[1] == '\x00' && buffer[2] == '\xFE' && buffer[3] == '\xEF')
        return UTF32BE;
    if (buffer[0] == '\xFF' && buffer[1] == '\xFE' && buffer[2] == '\x00' && buffer[3] == '\x00')
        return UTF32LE;

    return isForced ? forcedEncoding : UTF8;
}

FileInfo Utility::getFileInfo(const std::wstring& fileName, Writer* errorWriter)
{
	const HANDLE hFile = CreateFile(fileName.c_str(),
	                                GENERIC_READ,
	                                FILE_SHARE_READ,
	                                NULL,
	                                OPEN_EXISTING,
	                                FILE_ATTRIBUTE_NORMAL,
	                                NULL);


    if (hFile == INVALID_HANDLE_VALUE)
    {
        errorWriter->write(L"Unable to open: " + fileName + L"\n");
        return { fileName, 0};
    }

    LARGE_INTEGER fileSize;

    if (!GetFileSizeEx(hFile, &fileSize))
    {
        errorWriter->write(L"Unable to get size of file: " + fileName + L"\n");

        return { fileName, 0 };
    }

    uint64_t resultFileSize = fileSize.QuadPart;

    return { fileName, resultFileSize };
}

void Utility::FreeFileHandler(const FileWriter* writer)
{
    writer->closeHandle();
}

Writer* Utility::getConsoleWriter(DWORD _stdType)
{
	const auto consoleWriter = new ConsoleWriter(_stdType);

    return consoleWriter;
}

Writer* Utility::getFileWriter(const std::wstring& fileName, CRITICAL_SECTION* section)
{
    const HANDLE hFile = CreateFile(fileName.c_str(),
        GENERIC_WRITE,
        FILE_SHARE_WRITE,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    const DWORD errorCode = GetLastError();

    if (errorCode == ERROR_ALREADY_EXISTS)
    {
        std::wcerr << L"File already exists: " << fileName << "\n";
        return nullptr;
    }

    if (hFile == INVALID_HANDLE_VALUE)
    {
        std::wcerr << L"Can not create file: " << fileName << "\n";
        return nullptr;
    }

    const auto fileWriterWriter = new FileWriter(hFile, section);

    return fileWriterWriter;
}

std::vector<char> Utility::wstringToUtf8Bytes(const std::wstring& wideCharString)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

    std::string utf8String = converter.to_bytes(wideCharString);

    std::vector utf8Bytes(utf8String.begin(), utf8String.end());

    return utf8Bytes;
}

std::vector<char> Utility::wstringToUtf16LEBytes(const std::wstring& wideCharString)
{
    std::wstring_convert<std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>, wchar_t> converter;

    std::string utf16LEString = converter.to_bytes(wideCharString);

    std::vector utf16LEBytes(utf16LEString.begin(), utf16LEString.end());

    return utf16LEBytes;
}

std::vector<char> Utility::wstringToUtf16BEBytes(const std::wstring& wideCharString)
{
    std::wstring_convert<std::codecvt_utf16<wchar_t, 0x10ffff>, wchar_t> converter;
    //?

    std::string utf16BEString = converter.to_bytes(wideCharString);

    std::vector utf16BEBytes(utf16BEString.begin(), utf16BEString.end());

    return utf16BEBytes;
}

std::vector<char> Utility::wstringToUtf32LEBytes(const std::wstring& wideCharString)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string utf8_string = converter.to_bytes(wideCharString);

    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf32_converter;
    std::u32string utf32BEString = utf32_converter.from_bytes(utf8_string);

    auto x = utf32BEString.data();
    std::vector<char32_t> utf32BEOctets(utf32BEString.begin(), utf32BEString.end());

    std::vector<char> utf32BEBytes = ConvertVectorFromChar32ToCharLE(utf32BEOctets);

    return utf32BEBytes;
}

std::vector<char> Utility::wstringToUtf32BEBytes(const std::wstring& wideCharString)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string utf8_string = converter.to_bytes(wideCharString);

    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf32_converter;
    std::u32string utf32BEString = utf32_converter.from_bytes(utf8_string);

    auto x = utf32BEString.data();
    std::vector<char32_t> utf32BEOctets(utf32BEString.begin(), utf32BEString.end());

    std::vector<char> utf32BEBytes = ConvertVectorFromChar32ToCharBE(utf32BEOctets);

    return utf32BEBytes;
}

std::vector<char> Utility::wstringToAnsiBytes(const std::wstring& wideCharString)
{
    const int ansiStringLength = WideCharToMultiByte(CP_ACP, 0, wideCharString.c_str(), -1, NULL, 0, NULL, NULL);

    std::string ansiString(ansiStringLength - 1, 0);

    WideCharToMultiByte(CP_ACP, 0, wideCharString.c_str(), -1, ansiString.data(), ansiStringLength - 1, NULL, NULL);

    std::vector ansiBytes(ansiString.begin(), ansiString.end());

    return ansiBytes;
}

std::vector<char> Utility::ConvertVectorFromChar32ToCharLE(std::vector<char32_t> char32s)
{
    std::vector<char> chars(char32s.size() * 4);

    char* tempPointer = reinterpret_cast<char*>(char32s.data());

    std::copy_n(tempPointer, char32s.size() * 4, chars.begin());

    return chars;
}

std::vector<char> Utility::ConvertVectorFromChar32ToCharBE(std::vector<char32_t> char32s)
{
    std::vector<char> chars(char32s.size() * 4);

    char* tempPointer = reinterpret_cast<char*>(char32s.data());

    std::copy_n(tempPointer, char32s.size() * 4, chars.begin());

    auto iterator = chars.begin();
    while (iterator != chars.end())
    {
        std::reverse(iterator, iterator + 4);

        iterator += 4;
    }

    return chars;
}