#pragma once

#include "Windows.h"
#include <iostream>

#include "Writer.h"

class FileWriter final : public Writer
{
public:
	FileWriter(HANDLE _file, PCRITICAL_SECTION _section) : file(_file), section(_section) { }

	void write(std::wstring message) override;

	void closeHandle() const;
private:
	HANDLE file;
	PCRITICAL_SECTION section;
};
