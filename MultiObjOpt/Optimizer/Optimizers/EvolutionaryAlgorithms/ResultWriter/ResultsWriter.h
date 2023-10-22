#pragma once
#include <string>

struct SOptimizationResult;

class CResultsWriter
{
public:
	CResultsWriter(const char* outputDir);

	bool WriteOptimizationResultsToFile(const SOptimizationResult& oprResults, const std::string& instanceName, int configId, int runId) const;

private:
    const char* m_OutputDir;
};
