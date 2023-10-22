#pragma once
#include <string>

struct CConfigLines;

class CConfigReader
{
public:

	bool ReadConfig(const char* configPath, CConfigLines& configLines);

private:

	static const std::string s_Delimiter;
};