#pragma once
#include <string>

struct SOptimizationResult;

class CResultsWriterGA
{
public:
    CResultsWriterGA(const char* outputDir);

    bool WriteOptimizationResultsToFile(const SOptimizationResult& oprResults, const std::string& instanceName,
                                        int configId, int runId, const std::string& methodName,
                                        const std::string& populationSize, const std::string& GenerationLimit,
                                        const std::string& crossoverName, const std::string& crossoverProbability1, const std::string& crossoverProbability2,
                                        const std::string& mutationName, const std::string& mutationProbability, const std::string& tournamentSize) const;

private:
    const char* m_OutputDir;
};
