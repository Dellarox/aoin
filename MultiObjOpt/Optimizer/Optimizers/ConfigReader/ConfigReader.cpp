#include "ConfigReader.h"
#include "ConfigLines.h"
#include "Utils/ReadUtils.h"
#include <fstream>
#include <iostream>

const std::string CConfigReader::s_Delimiter = " ";

bool CConfigReader::ReadConfig(const char* configPath, CConfigLines& configLines)
{
	configLines.Clear();

	std::ifstream readFileStream(configPath);

	std::string line;
	while (std::getline(readFileStream, line))
	{
		std::string keyString, valueString;
		if (ReadUtils::ReadKeyValueString(line, s_Delimiter, keyString, valueString))
		{
			if (!configLines.AddLine(keyString, valueString))
			{
				std::cerr << "Key: " << keyString << "is already present in the config: " << configPath << std::endl;
				return false;
			}
		}
		else
		{
			std::cerr << "Cannot parse line: " << line << " while reading config: " << configPath << std::endl;
			return false;
		}
	}

	readFileStream.close();

	return true;
}
