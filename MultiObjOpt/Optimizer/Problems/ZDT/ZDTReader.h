#pragma once

#include <string>
#include <fstream>
#include "ZDTParams.h"

class CZDTReader
{
public:
	bool ReadDefinition(const char* filePath, SZDTParams& instanceParams) const;

	static const std::string s_Delimiter;
	static const std::string s_FunctionKey;
	static const std::string s_DimCountKey;
};
