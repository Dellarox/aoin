#pragma once

#include "Schedule.h"
#include <string>
#include <fstream>

class CMSRCPSPReader
{
public:
	bool ReadDefinition(const char* filePath, CSchedule& scheduleTemplate) const;

private:
	void ReadResources(std::ifstream& fileStream, int resourcesNumber, std::vector<CResource>& resources) const;
	void ReadTasks(std::ifstream& fileStream, int tasksNumber, std::vector<CTask>& tasks) const;

	static const std::string s_Delimiter;
	static const std::string s_TasksKey;
	static const std::string s_ResourcesKey;
	static const std::string s_ResourcesSectionKey;
	static const std::string s_TasksSectionKey;
};
