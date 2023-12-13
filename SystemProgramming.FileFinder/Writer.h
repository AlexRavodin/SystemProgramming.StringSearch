#pragma once

class Writer
{
public:
	virtual ~Writer() = default;

	virtual void write(std::wstring message) = 0;
};