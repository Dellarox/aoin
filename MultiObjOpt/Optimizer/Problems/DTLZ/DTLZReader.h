#pragma once

#include <string>
#include <fstream>
#include "DTLZParams.h"

class CDTLZReader
{
public:
	bool ReadDefinition(const char* filePath, SDTLZParams& instanceParams) const;

	static const std::string s_Delimiter;
	static const std::string s_FunctionKey;
	static const std::string s_ObjCountKey;
	static const std::string s_DimCountKey;
	static const std::string s_AlphaKey;
};
