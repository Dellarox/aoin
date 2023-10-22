#include <ostream>
#include <fstream>
#include "ResultsWriter.h"
#include "Problems/Problem.h"
#include "Optimizers/OptimizationResult.h"

CResultsWriter::CResultsWriter(const char* outputDir)
	: m_OutputDir(outputDir)
{
}

bool CResultsWriter::WriteOptimizationResultsToFile(const SOptimizationResult& oprResults, const std::string& instanceName, int configId, int runId) const
{
	char archive_filename[256];
	snprintf(archive_filename, 256, "%s/%s_config%d_run%d_archive.csv", m_OutputDir, instanceName.c_str(), configId, runId);
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

