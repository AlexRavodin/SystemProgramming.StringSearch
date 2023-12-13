#pragma once

#include <unordered_map>

#include "Windows.h"

#include "Encoding.h"
#include "FileInfo.h"
#include "FileWriter.h"
#include "Writer.h"

class Utility
{
public:
	static std::wstring GetWrongWaitObjectInfo(DWORD newThreadIndex, int maxThreadCount);
	static bool EncodingExists(const std::wstring& encodingString);
	static Encoding getEncoding(const std::wstring& encodingString);
	static std::wstring getEncodingString(Encoding encoding);

	static Encoding determineEncoding(char buffer[], int bufferSize, bool isForced, Encoding forcedEncoding);
	static bool ContainsUtf8Exceptions(char buffer[], int bufferSize);

	static FileInfo getFileInfo(const std::wstring& fileName, Writer* errorWriter);
	static void FreeFileHandler(const FileWriter* writer);

	static Writer* getConsoleWriter(DWORD _stdType);
	static Writer* getFileWriter(const std::wstring& fileName, PCRITICAL_SECTION section);

	static std::vector<char> wstringToUtf8Bytes(const std::wstring& wideCharString);
	static std::vector<char> wstringToUtf16LEBytes(const std::wstring& wideCharString);
	static std::vector<char> wstringToUtf16BEBytes(const std::wstring& wideCharString);
	static std::vector<char> wstringToUtf32LEBytes(const std::wstring& wideCharString);
	static std::vector<char> wstringToUtf32BEBytes(const std::wstring& wideCharString);
	static std::vector<char> wstringToAnsiBytes(const std::wstring& wideCharString);

private:
	static std::vector<char> ConvertVectorFromChar32ToCharLE(std::vector<char32_t> char32s);
	static std::vector<char> ConvertVectorFromChar32ToCharBE(std::vector<char32_t> char32s);

	inline static const std::set<char> impossibleUtf8Values
		{
			'\x80', '\x81', '\x82', '\x83', '\x84', '\x85',
			'\x86', '\x87', '\x88', '\x89', '\x8A', '\x8B', '\x8C',
			'\x8D', '\x8E', '\x8F', '\xF5', '\xF6', '\xF7', '\xF8',
			'\xF9', '\xFA', '\xFB', '\xFC', '\xFD', '\xFE', '\xFF',
			'\xC0', '\xC1', '\xFE', '\xFF',
		};

	inline static const std::unordered_map<std::wstring, Encoding> wstringToEncodingMap
	{
		{ L"UTF8", UTF8 },
		{ L"UTF16BE", UTF16BE },
		{ L"UTF16LE", UTF16LE },
		{ L"UTF32BE", UTF32BE },
		{ L"UTF32LE", UTF32LE },
		{ L"ANSI", ANSI },
	};

	inline static const std::unordered_map<Encoding, std::wstring> encodingToWstringMap
	{
		{ UTF8,  L"UTF8" },
		{ UTF16BE, L"UTF16BE" },
		{ UTF16LE, L"UTF16LE" },
		{ UTF32BE, L"UTF32BE" },
		{ UTF32LE, L"UTF32LE" },
		{ ANSI, L"ANSI" },
	};
};
