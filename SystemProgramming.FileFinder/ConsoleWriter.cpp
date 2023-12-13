#include "ConsoleWriter.h"

void ConsoleWriter::write(std::wstring message)
{
	DWORD writtenCount;

	const HANDLE stream = GetStdHandle(stdType);

	WriteConsole(stream, message.c_str(), static_cast<DWORD>(message.size()), &writtenCount, NULL);
}
