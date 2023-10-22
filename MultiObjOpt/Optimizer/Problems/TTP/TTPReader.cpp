#include "TTPReader.h"
#include <regex>
#include <string>
#include <vector>
#include "../../Utils/ReadUtils.h"

#define READ_TTP 1

const std::string CTTPReader::s_Delimiter = "\t";
const std::string CTTPReader::s_DimensionKey = "DIMENSION:";
const std::string CTTPReader::s_ItemsCountKey = "NUMBER OF ITEMS:";
const std::string CTTPReader::s_CapacityKey = "CAPACITY OF KNAPSACK:";
const std::string CTTPReader::s_MinSpeedKey = "MIN SPEED:";
const std::string CTTPReader::s_MaxSpeedKey = "MAX SPEED:";
const std::string CTTPReader::s_RentingRatioKey = "RENTING RATIO:";

const std::string CTTPReader::s_CitiesSectionKey = "NODE_COORD_SECTION	(INDEX, X, Y):";
const std::string CTTPReader::s_ItemsSectionKey = "ITEMS SECTION	(INDEX, PROFIT, WEIGHT, ASSIGNED NODE NUMBER):";

bool CTTPReader::ReadDefinition(const char* problemDefinitionPath, CTTP& ttp) const
{
	ttp.Clear();

	std::ifstream readFileStream(problemDefinitionPath);

	int dim = 0;
	ReadUtils::GotoReadIntegerByKey(readFileStream, s_DimensionKey, s_Delimiter, dim);
	//printf("dim %d\n", dim);
#if READ_TTP
	int itemCount = 0;
	ReadUtils::GotoReadIntegerByKey(readFileStream, s_ItemsCountKey, s_Delimiter, itemCount);
	//printf("itenCount %d\n", itemCount);
	int capacity = 0;
	ReadUtils::GotoReadIntegerByKey(readFileStream, s_CapacityKey, s_Delimiter, capacity);
	//printf("capacity %d\n", capacity);

	float minSpeed = 0.f;
	ReadUtils::GotoReadFloatByKey(readFileStream, s_MinSpeedKey, s_Delimiter, minSpeed);
	//printf("minSpeed %f\n", minSpeed);
	float maxSpeed = 0.f;
	ReadUtils::GotoReadFloatByKey(readFileStream, s_MaxSpeedKey, s_Delimiter, maxSpeed);
	//printf("maxSpeed %f\n", maxSpeed);
	float rentRatio = 0.f;
	ReadUtils::GotoReadFloatByKey(readFileStream, s_RentingRatioKey, s_Delimiter, rentRatio);
	//printf("rentRatio %f\n", rentRatio);
#endif

	std::vector<SCity> cities;
	ReadCities(readFileStream, dim, cities);
	//printf("citiesRead %d\n", cities.size());

	std::vector<SItem> items;
#if READ_TTP
	ReadItems(readFileStream, itemCount, items);
#endif

#if USE_EOK
	// Add extra EOK flag item
	items.emplace_back(0, 0, 0, 0);
#endif

	//printf("itemsRead %d\n", items.size());

	std::string pathString(problemDefinitionPath);
	size_t fileNameStartPos = pathString.rfind("/") + 1;
	size_t fileNameEndPos = pathString.rfind(".");
	ttp.SetFileName(pathString.substr(fileNameStartPos, fileNameEndPos - fileNameStartPos));
#if READ_TTP
	ttp.SetData(cities, items, capacity, minSpeed, maxSpeed, rentRatio);
#else
	ttp.SetData(cities, items, 0, 0, 1, 1);
#endif

	readFileStream.close();

	return true;
}

void CTTPReader::ReadCities(std::ifstream& fileStream, int dimension, std::vector<SCity>& cities) const
{
	std::string line;
	if (ReadUtils::GotoLineByKey(fileStream, s_CitiesSectionKey, line))
	{
		for (int i = 0; i < dimension; ++i)
		{
			if (std::getline(fileStream, line))
			{
				//auto const re = std::regex{ R"(\t)" };
				auto const re = std::regex{ R"(\s+)" };
				auto const vec = std::vector<std::string>(std::sregex_token_iterator{ line.begin(), line.end(), re, -1 }, std::sregex_token_iterator{});
				cities.emplace_back(std::stoi(vec[0]), std::stof(vec[1]), std::stof(vec[2]));
			}
		}
	}
}

void CTTPReader::ReadItems(std::ifstream& fileStream, int itemCount, std::vector<SItem>& items) const
{
	std::string line;
	if (ReadUtils::GotoLineByKey(fileStream, s_ItemsSectionKey, line))
	{
		for (int i = 0; i < itemCount; ++i)
		{
			if (std::getline(fileStream, line))
			{
				auto const re = std::regex{ R"(\t)" };
				auto const vec = std::vector<std::string>(std::sregex_token_iterator{ line.begin(), line.end(), re, -1 }, std::sregex_token_iterator{});
				items.emplace_back(std::stoi(vec[0]), std::stoi(vec[1]), std::stoi(vec[2]), std::stoi(vec[3]));
			}
		}
	}
}
