#include "ArchiveUtils.h"
#include "AggregatedFitness.h"

#include <ostream>
#include <fstream>
#include <algorithm>

#define USE_EVAL_DUPLICATE 1

void ArchiveUtils::CopyToArchiveWithFiltering(const std::vector<CIndividual*>& individuals, std::vector<CIndividual*>& archive)
{
    std::vector<const CIndividual*> filteredIndividuals;
	filteredIndividuals.reserve(individuals.size());

	// For each new individual, check if not dominated or not dominating
	// We don't have to check if the same solution, equal rewards are discarded
	for (size_t p = 0; p < individuals.size(); ++p)
	{
		const CIndividual* newInd = individuals[p];
		bool isDominated = false;
		// Check other new individuals
		size_t i = 0;
		while (!isDominated && i < individuals.size())
		{
			if (p != i)
			{
				isDominated = newInd->IsDominatedBy(individuals[i]);
				if (!isDominated && p < i) // If DuplicateFenotype - leave the last one
				{
					// Check if duplicated
#if USE_EVAL_DUPLICATE
					isDominated = newInd->IsDuplicateEvalValue(individuals[i]);
#else
					isDominated = newInd->IsDuplicateGenotype(individuals[i]);
#endif
				}
			}
			++i;
		}
		// Check individuals in archive
		i = 0;
		while (!isDominated && i < archive.size())
		{
			//if (p != i)
			{
				isDominated = newInd->IsDominatedBy(archive[i]);
				if (!isDominated)
				{
					// Chick if duplicated
#if USE_EVAL_DUPLICATE
					isDominated = newInd->IsDuplicateEvalValue(archive[i]);
#else
					isDominated = newInd->IsDuplicateGenotype(archive[i]);
#endif
				}
			}
			
			++i;
		}
		if (!isDominated)
		{
			filteredIndividuals.push_back(newInd);
		}
	}

	// Now check if already archived individuals are not dominated by new, remove otherwise
	archive.erase(std::remove_if(archive.begin(), archive.end(), [filteredIndividuals](const CIndividual* ind)
	{
		for (const CIndividual* filteredInd : filteredIndividuals)
		{
			if (ind->IsDominatedBy(filteredInd))
			{
				return true;
			}
		}
		return false;
	}), archive.end());

	// Finally, add new non-dominated individuals to the archive
	archive.reserve(archive.size() + filteredIndividuals.size());
	for (const CIndividual* filteredInd : filteredIndividuals)
	{
		// Make deep copy
		archive.push_back(new CIndividual(*filteredInd));
	}
}

void ArchiveUtils::CopyToArchiveForSingleObjective(const std::vector<CIndividual*>& individuals, std::vector<CIndividual*>& archive, std::vector<float> objectiveWeights)
{
    if (individuals.empty()) {
        return;
    }

    CIndividual* bestIndividual = individuals[0];
    float bestFitness = CAggregatedFitness::ComputeAggregatedFitness(bestIndividual, objectiveWeights);

    for (CIndividual* individual : individuals) {
        float currentFitness = CAggregatedFitness::ComputeAggregatedFitness(individual, objectiveWeights);
        if (currentFitness < bestFitness) {
            bestFitness = currentFitness;
            bestIndividual = individual;
        }
    }

    archive.push_back(bestIndividual);
}

void ArchiveUtils::CopyToArchiveForSingleObjectiveSA(const std::vector<CIndividual*>& individuals, std::vector<CIndividual*>& archive)
{
    if (individuals.empty()) {
        return;
    }

    CIndividual* bestIndividual = individuals[0];
    float bestFitness = CAggregatedFitness::ComputeAggregatedFitnessSA(bestIndividual);

    for (CIndividual* individual : individuals) {
        float currentFitness = CAggregatedFitness::ComputeAggregatedFitnessSA(individual);
        if (currentFitness < bestFitness) {
            bestFitness = currentFitness;
            bestIndividual = individual;
        }
    }

    archive.push_back(bestIndividual);
}

void ArchiveUtils::CopyToArchiveWithFiltering(const CIndividual* individual, std::vector<CIndividual*>& archive)
{
	// Check if not dominated
	bool isDominated = false;
	size_t i = 0;
	// Check individuals in archive
	while (!isDominated && i < archive.size())
	{
		isDominated = individual->IsDominatedBy(archive[i]);
		if (!isDominated)
		{
			// Chick if duplicated
#if USE_EVAL_DUPLICATE
			isDominated = individual->IsDuplicateEvalValue(archive[i]);
#else
			isDominated = individual->IsDuplicateGenotype(m_Archive[i]);
#endif
		}
		++i;
	}

	if (!isDominated)
	{
		// Now check if already archived individuals are not dominated by new, remove otherwise
		archive.erase(std::remove_if(archive.begin(), archive.end(), [individual](const CIndividual* ind)
		{
			return ind->IsDominatedBy(individual);
		}), archive.end());

		archive.push_back(new CIndividual(*individual));
	}
}

void ArchiveUtils::SaveArchiveToFile(const std::vector<CIndividual*>& archive)
{
	char archive_filename[128];
	std::string fileName("pareto_out.csv");
	snprintf(archive_filename, 128, "%s", fileName.c_str());
	std::ofstream arch_file(archive_filename);
	for (const CIndividual* ind : archive) {

		size_t valCount = ind->GetEvaluationSize();
		for (int i = 0; i < valCount; ++i)
		{
			arch_file << ind->GetEvalValue(i);
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
}

void ArchiveUtils::SaveArchiveToFile(const std::vector<CIndividual*>& archive, const std::string& instanceName, int configId, int runId)
{
	char archive_filename[128];
	snprintf(archive_filename, 128, "results/%s_config%d_run%d_archive.csv", instanceName.c_str(), configId, runId);
	std::ofstream arch_file(archive_filename);
	for (const CIndividual* ind : archive) {

		size_t valCount = ind->GetEvaluationSize();
		for (int i = 0; i < valCount; ++i)
		{
			arch_file << ind->GetEvalValue(i);
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
}

std::vector<TEvaluation> ArchiveUtils::ToEvaluation(const std::vector<CIndividual*>& archive)
{
	std::vector<TEvaluation> evalValues;
	evalValues.reserve(evalValues.size() + archive.size());
	for (const CIndividual* ind : archive)
	{
		evalValues.emplace_back(ind->GetEvaluation());
	}
	return evalValues;
}
