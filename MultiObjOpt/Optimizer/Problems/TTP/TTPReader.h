#pragma once

#include <fstream>
#include <vector>
#include "TTP.h"

class CTTPReader
{
public:
	bool ReadDefinition(const char* problemDefinitionPath, CTTP& ttp) const;

private:

	void ReadCities(std::ifstream& fileStream, int dimension, std::vector<SCity>& cities) const;
	void ReadItems(std::ifstream& fileStream, int itemCount, std::vector<SItem>& items) const;

	static const std::string s_Delimiter;
	static const std::string s_DimensionKey;
	static const std::string s_ItemsCountKey;
	static const std::string s_CapacityKey;
	static const std::string s_MinSpeedKey;
	static const std::string s_MaxSpeedKey;
	static const std::string s_RentingRatioKey;

	static const std::string s_CitiesSectionKey;
	static const std::string s_ItemsSectionKey;
};
