#pragma once

#include "../Problem.h"
#include "Schedule.h"

class CProblemMSRCPSP_TaskAssign : public IProblem
{
public:

	bool ReadDefinition(const char* filePath) override;
	bool Evaluate(const TSolution& solution, TEvaluation& evaluation) const override;
	size_t GetObjCount() const override;
	const SProblemEncoding& GetProblemEncoding() const override;

private:

	void CalculateUpperBounds();

	std::vector<std::vector<TResourceID>> m_CapableResources;
	std::vector<size_t> m_UpperBounds;
	SProblemEncoding m_ProblemEncoding;

	CSchedule m_ScheduleTemplate;
};