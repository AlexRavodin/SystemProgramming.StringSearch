#pragma once

#include <list>
#include <vector>

#include "Encoding.h"
#include "FileInfo.h"
#include "Writer.h"

struct SearchInfo {
public:
    SearchInfo(FileInfo _fileInfo, std::wstring _string, Encoding _encoding, bool _isCaseSensitive,
        Writer* _resultWriter, Writer* _errorWriter);

    FileInfo fileInfo;
    std::wstring string;
    Encoding encoding;
    bool isCaseSensitive;

    std::vector<std::pair<int, int>> positions;
    std::wstring result;

    Writer* resultWriter;
    Writer* errorWriter;

    void PrintResult();
private:
    void CreateResult();
};
