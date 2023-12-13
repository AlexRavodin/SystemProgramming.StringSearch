#pragma once

#include <string>
#include "Windows.h"

#include "Writer.h"

class ConsoleWriter final : public Writer
{
public:
	explicit ConsoleWriter(DWORD _stdType) : stdType(_stdType) { }

	void write(std::wstring message) override;

private:
	DWORD stdType;
};

