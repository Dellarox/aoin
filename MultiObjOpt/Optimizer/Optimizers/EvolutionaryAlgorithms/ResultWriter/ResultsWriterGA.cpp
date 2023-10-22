//
// Created by Kajetan on 22.10.2023.
//

#include <fstream>
#include "ResultsWriterGA.h"
#include "Problems/Problem.h"
#include "Optimizers/OptimizationResult.h"

CResultsWriterGA::CResultsWriterGA(const char* outputDir)
        : m_OutputDir(outputDir)
{
}

bool CResultsWriterGA::WriteOptimizationResultsToFile(const SOptimizationResult& oprResults, const std::string& instanceName,
                                                      int configId, int runId, const std::string& methodName,
                                                      const std::string& populationSize, const std::string& GenerationLimit,
                                                      const std::string& crossoverName, const std::string& crossoverProbability1, const std::string& crossoverProbability2,
                                                      const std::string& mutationName, const std::string& mutationProbability, const std::string& tournamentSize) const
{
    char archive_filename[256];
    snprintf(archive_filename, 256, "%s/%s_config%d_run%d_%s_%s_%s_%s_%s_%s_%s_%s_%s_archive.csv", m_OutputDir, instanceName.c_str(), configId, runId,
             methodName.c_str(), populationSize.c_str(), GenerationLimit.c_str(),
             crossoverName.c_str(), crossoverProbability1.c_str(), crossoverProbability2.c_str(),
             mutationName.c_str(), mutationProbability.c_str(), tournamentSize.c_str());
    std::ofstream arch_file(archive_filename);
    for (const TEvaluation& eval : oprResults.m_ParetoFront) {

        size_t valCount = eval.size();
        for (int i = 0; i < valCount; ++i)
        {
            arch_file << eval[i];
            if (i < valCount - 1)
            {
                arch_file << ";";
            }
            else
            {
                arch_file << std::endl;
            }
        }
    }
    arch_file.close();
    return true;
}
