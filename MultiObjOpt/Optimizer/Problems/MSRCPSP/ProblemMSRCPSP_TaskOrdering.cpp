#include "ProblemMSRCPSP_TaskOrdering.h"
#include "MSRCPSPReader.h"

bool CProblemMSRCPSP_TaskOrdering::ReadDefinition(const char* filePath)
{
	CMSRCPSPReader problemReader;
	return problemReader.ReadDefinition(filePath, m_ScheduleTemplate);
}

bool CProblemMSRCPSP_TaskOrdering::Evaluate(const TSolution& solution, TEvaluation& evaluation) const
{
	// TODO - implement

	return false;
}

size_t CProblemMSRCPSP_TaskOrdering::GetObjCount() const
{
	return 5;
}

const SProblemEncoding& CProblemMSRCPSP_TaskOrdering::GetProblemEncoding() const
{
	// TODO - implement

	return SProblemEncoding();
}
