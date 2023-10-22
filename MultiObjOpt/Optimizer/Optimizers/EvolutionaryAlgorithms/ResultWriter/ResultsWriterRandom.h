#pragma once
#include <string>

struct SOptimizationResult;

class CResultsWriterRandom
{
public:
    CResultsWriterRandom(const char* outputDir);

    bool WriteOptimizationResultsToFile(const SOptimizationResult& oprResults, const std::string& instanceName, int configId, int runId,
                                        const std::string& methodName) const;

private:
    const char* m_OutputDir;
};
