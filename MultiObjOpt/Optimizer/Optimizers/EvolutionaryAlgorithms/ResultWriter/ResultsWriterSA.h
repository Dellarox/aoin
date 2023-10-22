#pragma once
#include <string>

struct SOptimizationResult;

class CResultsWriterSA
{
public:
    CResultsWriterSA(const char* outputDir);

    bool WriteOptimizationResultsToFile(const SOptimizationResult& oprResults, const std::string& instanceName, int configId, int runId,
                                        const std::string& methodName, const std::string& tempMin,
                                        const std::string& tempMax, const std::string& tempReduce,
                                        const std::string& populationSize) const;

private:
    const char* m_OutputDir;
};
