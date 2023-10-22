#pragma once
#include <vector>
#include <string>
#include "Optimizers/EvolutionaryAlgorithms/Individual/Individual.h"

namespace ArchiveUtils
{
    void CopyToArchiveWithFiltering(const std::vector<CIndividual*>& individuals, std::vector<CIndividual*>& archive);
    void CopyToArchiveForSingleObjective(const std::vector<CIndividual*>& individuals, std::vector<CIndividual*>& archive, std::vector<float> objectiveWeights);
    void CopyToArchiveWithFiltering(const CIndividual* individual, std::vector<CIndividual*>& archive);
	void SaveArchiveToFile(const std::vector<CIndividual*>& archive);
	void SaveArchiveToFile(const std::vector<CIndividual*>& archive, const std::string& instanceName, int configId, int runId);
	std::vector<TEvaluation> ToEvaluation(const std::vector<CIndividual*>& archive);

    void CopyToArchiveForSingleObjectiveSA(const std::vector<CIndividual *> &individuals, std::vector<CIndividual *> &archive);
};
