#include "ReadUtils.h"
#include <fstream>

bool ReadUtils::GotoReadSizeByKey(std::ifstream& fileStream, const std::string& lineKey, const std::string& delimiter, size_t& val)
{
	std::string line;
	if (ReadUtils::GotoLineByKey(fileStream, lineKey, line))
	{
		return ReadUtils::ReadSizeByKey(line, lineKey, delimiter, val);
	}
	return false;
}

bool ReadUtils::GotoReadIntegerByKey(std::ifstream& fileStream, const std::string& lineKey, const std::string& delimiter, int& val)
{
	std::string line;
	if (ReadUtils::GotoLineByKey(fileStream, lineKey, line))
	{
		return ReadUtils::ReadIntegerByKey(line, lineKey, delimiter, val);
	}
	return false;
}

bool ReadUtils::GotoReadFloatByKey(std::ifstream& fileStream, const std::string& lineKey, const std::string& delimiter, float& val)
{
	std::string line;
	if (ReadUtils::GotoLineByKey(fileStream, lineKey, line))
	{
		return ReadUtils::ReadFloatByKey(line, lineKey, delimiter, val);
	}
	return false;
}

bool ReadUtils::GotoReadStringByKey(std::ifstream& fileStream, const std::string& lineKey, const std::string& delimiter, std::string& val)
{
	std::string line;
	if (ReadUtils::GotoLineByKey(fileStream, lineKey, line))
	{
		return ReadUtils::ReadStringByKey(line, lineKey, delimiter, val);
	}
	return false;
}

bool ReadUtils::GotoLineByKey(std::ifstream& fileStream, const std::string& lineKey, std::string& line)
{
	while (std::getline(fileStream, line))
	{
		if (line.rfind(lineKey, 0) == 0)
		{
			return true;
		}
	}
	return false;
}

bool ReadUtils::ReadSizeByKey(const std::string& line, const std::string& lineKey, const std::string& delimiter, size_t& val)
{
	int integerVal = 0;
	if (ReadIntegerByKey(line, lineKey, delimiter, integerVal))
	{
		if (integerVal >= 0)
		{
			val = (size_t)integerVal;
			return true;
		}
	}
	return false;
}

bool ReadUtils::ReadIntegerByKey(const std::string& line, const std::string& lineKey, const std::string& delimiter, int& val)
{
	if (line.rfind(lineKey, 0) == 0)
	{
		size_t delimiterPos = line.find(delimiter);
		if (delimiterPos >= 0)
		{
			std::string stringVal = line.substr(delimiterPos + 1, line.length() - delimiterPos);
			val = std::stoi(stringVal);
			return true;
		}
	}
	return false;
}

bool ReadUtils::ReadFloatByKey(const std::string& line, const std::string& lineKey, const std::string& delimiter, float& val)
{
	if (line.rfind(lineKey, 0) == 0)
	{
		size_t delimiterPos = line.find(delimiter);
		if (delimiterPos >= 0)
		{
			std::string stringVal = line.substr(delimiterPos + 1, line.length() - delimiterPos);
			val = std::stof(stringVal);
			return true;
		}
	}
	return false;
}

bool ReadUtils::ReadStringByKey(const std::string& line, const std::string& lineKey, const std::string& delimiter, std::string& val)
{
	if (line.rfind(lineKey, 0) == 0)
	{
		size_t delimiterPos = line.find(delimiter);
		if (delimiterPos >= 0)
		{
			val = line.substr(delimiterPos + 1, line.length() - delimiterPos);
			return true;
		}
	}
	return false;
}

bool ReadUtils::ReadKeyValueString(const std::string& line, const std::string& delimiter, std::string& keyString, std::string& valueString)
{
	size_t delimiterPos = line.find(delimiter);
	if (delimiterPos > 0 && delimiterPos < line.length())
	{
		keyString = line.substr(0, delimiterPos);
		valueString = line.substr(delimiterPos + 1, line.length() - delimiterPos);
		return true;
	}
	return false;
}
