//
// Created by Kajetan on 22.10.2023.
//

#include "ResultsWriterSA.h"
#include <fstream>
#include "Problems/Problem.h"
#include "Optimizers/OptimizationResult.h"

CResultsWriterSA::CResultsWriterSA(const char* outputDir)
        : m_OutputDir(outputDir)
{
}

bool
CResultsWriterSA::WriteOptimizationResultsToFile(const SOptimizationResult &oprResults, const std::string &instanceName,
                                                 int configId, int runId, const std::string &methodName,
                                                 const std::string &tempMin, const std::string &tempMax,
                                                 const std::string &tempReduce,
                                                 const std::string &populationSize) const {
    char archive_filename[256];
    snprintf(archive_filename, 256, "%s/%s_config%d_run%d_%s_%s_%s_%s_%s_archive.csv", m_OutputDir, instanceName.c_str(), configId, runId,
             methodName.c_str(), tempMax.c_str(),
             tempMin.c_str(), tempReduce.c_str(), populationSize.c_str());
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
