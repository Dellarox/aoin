#include "ProblemMSRCPSP_TaskAssign.h"
#include "MSRCPSPReader.h"
#include <algorithm>

bool CProblemMSRCPSP_TaskAssign::ReadDefinition(const char* filePath)
{
	CMSRCPSPReader problemReader;

	if (problemReader.ReadDefinition(filePath, m_ScheduleTemplate))
	{
		CalculateUpperBounds();
		return true;
	}

	return false;
}

bool CProblemMSRCPSP_TaskAssign::Evaluate(const TSolution& solution, TEvaluation& evaluation) const
{
	CSchedule problemCopy = m_ScheduleTemplate;
	for (size_t i = 0; i < solution.size(); ++i)
	{
		TResourceID selectedResourceId = m_CapableResources[i][(size_t)solution[i]];
		problemCopy.Assign(i, selectedResourceId);
	}

	problemCopy.BuildTimestamps();

	// We assume this is 5 dim problem
	evaluation = 
	{ 
		problemCopy.EvaluateDuration(), 
		problemCopy.EvaluateCost(),
		problemCopy.EvaluateAvgCashFlowDev(),
		problemCopy.EvaluateSkillOveruse(),
		problemCopy.EvaluateAvgUseOfResTime()
	};

	return true;
}

size_t CProblemMSRCPSP_TaskAssign::GetObjCount() const
{
	return 5;
}

const SProblemEncoding& CProblemMSRCPSP_TaskAssign::GetProblemEncoding() const
{
	return m_ProblemEncoding;
}

void CProblemMSRCPSP_TaskAssign::CalculateUpperBounds()
{
	m_CapableResources.clear();
	m_UpperBounds.clear();
	const std::vector<CTask>& tasks = m_ScheduleTemplate.GetTasks();
	m_CapableResources.reserve(tasks.size());
	m_UpperBounds.reserve(tasks.size());

	for (const CTask& task : tasks)
	{
		std::vector<TResourceID> capableResourceIds;
		m_ScheduleTemplate.GetCapableResources(task, capableResourceIds);
		m_CapableResources.push_back(capableResourceIds);

		// TODO - verify if size or size -1
		m_UpperBounds.push_back(capableResourceIds.size());
	}

	SEncodingSection associationSection;
	associationSection.m_SectionType = EEncodingType::ASSOCIATION;
	for (const size_t& ub : m_UpperBounds)
	{
		associationSection.m_SectionDescription.push_back({ (TSolutionValue)0, (TSolutionValue)ub });
	}
	m_ProblemEncoding = SProblemEncoding{ { associationSection } };
}
