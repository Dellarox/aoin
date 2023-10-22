#include "MSRCPSPReader.h"
#include <regex>
#include "../../Utils/ReadUtils.h"

const std::string CMSRCPSPReader::s_Delimiter = " ";
const std::string CMSRCPSPReader::s_TasksKey = "Tasks:";
const std::string CMSRCPSPReader::s_ResourcesKey = "Resources:";
const std::string CMSRCPSPReader::s_ResourcesSectionKey = "ResourceID";
const std::string CMSRCPSPReader::s_TasksSectionKey = "TaskID";

bool CMSRCPSPReader::ReadDefinition(const char* filePath, CSchedule& scheduleTemplate) const
{
	scheduleTemplate.Clear();

	std::ifstream readFileStream(filePath);

	int tasksNumber = 0;
	ReadUtils::GotoReadIntegerByKey(readFileStream, s_TasksKey, s_Delimiter, tasksNumber);

	int resourcesNumber = 0;
	ReadUtils::GotoReadIntegerByKey(readFileStream, s_ResourcesKey, s_Delimiter, resourcesNumber);

	std::vector<CResource> resources;
	ReadResources(readFileStream, resourcesNumber, resources);
	std::vector<CTask> tasks;
	ReadTasks(readFileStream, tasksNumber, tasks);

	readFileStream.close();

	// Setup successors
	for (const CTask& task : tasks)
	{
		for (TTaskID predecessorTaskId : task.GetPredecessors())
		{
			tasks[(size_t)predecessorTaskId - 1].SetHasSuccessors(true);
		}
	}

	scheduleTemplate.SetResources(resources);
	scheduleTemplate.SetTasks(tasks);
	scheduleTemplate.Reset();

	return true;
}

void CMSRCPSPReader::ReadResources(std::ifstream& fileStream, int resourcesNumber, std::vector<CResource>& resources) const
{
	std::string line;
	if (ReadUtils::GotoLineByKey(fileStream, s_ResourcesSectionKey, line))
	{
		for (int i = 0; i < resourcesNumber; ++i)
		{
			if (std::getline(fileStream, line))
			{
				auto const re = std::regex{ R"(\s+)" };
				auto const vec = std::vector<std::string>(std::sregex_token_iterator{ line.begin(), line.end(), re, -1 }, std::sregex_token_iterator{});

				std::vector<SSkill> skills;
				for (size_t i = 2; i + 1 < vec.size(); i += 2)
				{
					skills.emplace_back(std::stoi(vec[i].substr(1, vec[i].length() - 1)), std::stoi(vec[i + 1]));
				}
				resources.emplace_back(std::stoi(vec[0]), std::stof(vec[1]), skills);
			}
		}
	}
}

void CMSRCPSPReader::ReadTasks(std::ifstream& fileStream, int tasksNumber, std::vector<CTask>& tasks) const
{
	std::string line;
	if (ReadUtils::GotoLineByKey(fileStream, s_TasksSectionKey, line))
	{
		for (int i = 0; i < tasksNumber; ++i)
		{
			if (std::getline(fileStream, line))
			{
				auto const re = std::regex{ R"(\s+)" };
				auto const vec = std::vector<std::string>(std::sregex_token_iterator{ line.begin(), line.end(), re, -1 }, std::sregex_token_iterator{});

				size_t tokenIndex = 2;
				std::vector<SSkill> skills;
				while (tokenIndex + 1 < vec.size() && vec[tokenIndex].substr(0, 1) == "Q")
				{
					skills.emplace_back(std::stoi(vec[tokenIndex].substr(1, vec[tokenIndex].length() - 1)), std::stoi(vec[tokenIndex + 1]));
					tokenIndex += 2;
				}

				std::vector<TTaskID> predecessors;
				while (tokenIndex < vec.size())
				{
					predecessors.push_back(std::stoi(vec[tokenIndex]));
					++tokenIndex;
				}
				tasks.emplace_back(std::stoi(vec[0]), skills, std::stoi(vec[1]), predecessors);
			}
		}
	}
}
