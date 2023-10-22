//
// Created by Kajetan on 22.10.2023.
//

#include "ResultsWriterRandom.h"
#include <fstream>
#include "Problems/Problem.h"
#include "Optimizers/OptimizationResult.h"

CResultsWriterRandom::CResultsWriterRandom(const char* outputDir)
        : m_OutputDir(outputDir)
{
}

bool CResultsWriterRandom::WriteOptimizationResultsToFile(const SOptimizationResult &oprResults,
                                                          const std::string &instanceName, int configId, int runId,
                                                          const std::string &methodName) const {
    char archive_filename[256];
    snprintf(archive_filename, 256, "%s/%s_config%d_run%d_%s_archive.csv", m_OutputDir, instanceName.c_str(), configId, runId,
             methodName.c_str());
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
