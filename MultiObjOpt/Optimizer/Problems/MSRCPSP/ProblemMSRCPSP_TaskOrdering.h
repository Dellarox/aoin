#pragma once

#include "../Problem.h"
#include "Schedule.h"

class CProblemMSRCPSP_TaskOrdering : public IProblem
{
public:

	bool ReadDefinition(const char* filePath) override;
	bool Evaluate(const TSolution& solution, TEvaluation& evaluation) const override;
	size_t GetObjCount() const override;
	const SProblemEncoding& GetProblemEncoding() const override;

private:

	CSchedule m_ScheduleTemplate;
};