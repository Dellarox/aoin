#pragma once
#include <map>
#include <string>

struct CConfigLines
{
public:

    bool TakeName(const std::string& paramKey, std::string& outValue);
	bool TakeValue(const std::string& paramKey, int& outValue);
	bool TakeValue(const std::string& paramKey, size_t& outValue);
	bool TakeValue(const std::string& paramKey, float& outValue);
	bool TakeValue(const std::string& paramKey, double& outValue);
	bool TakeValue(const std::string& paramKey, std::string& outValue);

	void Clear();
	bool AddLine(const std::string& keyString, const std::string& valueString);

private:

	std::map<std::string, std::string> m_ConfigMap;
};