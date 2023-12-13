#include "SearchInfo.h"

#include "string"

SearchInfo::SearchInfo(FileInfo _fileInfo, std::wstring _string, Encoding _encoding,
						bool _isCaseSensitive, Writer* _resultWriter, Writer* _errorWriter):
	fileInfo(std::move(_fileInfo)), string(std::move(_string)), encoding(_encoding),
	isCaseSensitive(_isCaseSensitive), resultWriter(_resultWriter), errorWriter(_errorWriter)
{ }

void SearchInfo::CreateResult()
{
	result = fileInfo.filePath + L":\n" + L"Line: " + std::to_wstring(positions[0].first) + L" Column: " + std::to_wstring(positions[0].second);

	for (int i = 1; i < positions.size(); i++)
	{
		if (positions.at(i).first != positions.at(i - 1).first)
		{
			result += L".\nLine: " + std::to_wstring(positions.at(i).first) + L" Column: " +
				std::to_wstring(positions.at(i).second);
		}
		else
		{
			result += L", " + std::to_wstring(positions.at(i).second);
		}
	}

	result += L"\n\n";
}

void SearchInfo::PrintResult()
{
	if (positions.empty())
		return;

	CreateResult();

	resultWriter->write(result);
}
