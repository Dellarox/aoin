#pragma once
#include <string>

namespace ReadUtils
{
	bool GotoReadSizeByKey(std::ifstream& fileStream, const std::string& lineKey, const std::string& delimiter, size_t& val);
	bool GotoReadIntegerByKey(std::ifstream& fileStream, const std::string& lineKey, const std::string& delimiter, int& val);
	bool GotoReadFloatByKey(std::ifstream& fileStream, const std::string& lineKey, const std::string& delimiter, float& val);
	bool GotoReadStringByKey(std::ifstream& fileStream, const std::string& lineKey, const std::string& delimiter, std::string& val);
	bool GotoLineByKey(std::ifstream& fileStream, const std::string& lineKey, std::string& line);

	bool ReadSizeByKey(const std::string& line, const std::string& lineKey, const std::string& delimiter, size_t& val);
	bool ReadIntegerByKey(const std::string& line, const std::string& lineKey, const std::string& delimiter, int& val);
	bool ReadFloatByKey(const std::string& line, const std::string& lineKey, const std::string& delimiter, float& val);
	bool ReadStringByKey(const std::string& line, const std::string& lineKey, const std::string& delimiter, std::string& val);

	bool ReadKeyValueString(const std::string& line, const std::string& delimiter, std::string& keyString, std::string& valueString);
}