#include "SPEA2.h"
#include "Optimizers/EvolutionaryAlgorithms/ArchiveUtils.h"

CSPEA2::CSPEA2(const SOptimizerParams& optParams, const IProblem* optProblem)
	: m_Random(optParams.m_SeedValue)
	, m_ProblemDefinition(optProblem)
	, m_Operators(&m_Random)
	, m_Params(optParams, optProblem)
{
}

CSPEA2::~CSPEA2()
{
	Reset();
}

SOptimizationResult CSPEA2::RunOptimization()
{
	Reset();

	// TODO - move to function
	m_Population.reserve(m_Params.m_PopulationSize);
	for (size_t i = 0; i < m_Params.m_PopulationSize; ++i)
	{
		std::vector<TGene> genes;
		CIndividual* newInd = new CIndividual();
		m_Operators.RandomInit(genes, m_ProblemDefinition->GetProblemEncoding());
		newInd->SetGenes(genes);
		newInd->Evaluate(m_ProblemDefinition);
		m_Population.push_back(newInd);
	}

	std::vector<TNeighborhood> neighborhood;
	BuildNeighborhood(m_Population, neighborhood);
	UpdateFineGrainedFitness(m_Population, neighborhood);

	ArchiveUtils::CopyToArchiveWithFiltering(m_Population, m_Archive);

	while (!StopCondition())
	{
		Evolve();

		std::vector<CIndividual*> combinedPop;
		combinedPop.reserve(m_Population.size() + m_Archive.size());
		combinedPop.insert(combinedPop.end(), m_Population.begin(), m_Population.end());
		combinedPop.insert(combinedPop.end(), m_Archive.begin(), m_Archive.end());

		BuildNeighborhood(combinedPop, neighborhood);
		UpdateFineGrainedFitness(combinedPop, neighborhood);
		EnviroSelection(combinedPop);

		++m_Generation;
	}

	// Filter out all dominated individuals from the archive
	// Just to make sure we have only non-dominated individuals at the end
	// This is needed as current implementation can store dominated but good solution in the archive
	// TODO - maybe we should create separate "global" archive to store best found and use NextPopulation instead of the current population
	std::vector<CIndividual*> allArchiveInd = m_Archive;
	m_Archive.clear();
	ArchiveUtils::CopyToArchiveWithFiltering(allArchiveInd, m_Archive);
	for (CIndividual* ind : allArchiveInd)
	{
		delete ind;
	}

	return SOptimizationResult
	{
		ArchiveUtils::ToEvaluation(m_Archive)
	};
}

void CSPEA2::Reset()
{
	m_Generation = 0;

	for (CIndividual* ind : m_Population)
	{
		delete ind;
	}
	m_Population.clear();

	for (CIndividual* ind : m_Archive)
	{
		delete ind;
	}
	m_Archive.clear();
}

void CSPEA2::Evolve()
{
	// archive -> population
	for (size_t i = 0; i < m_Params.m_PopulationSize; i += 2)
	{
		size_t firstParentIdx = Spea2TournamentSelection(m_Archive);
		size_t secondParentIdx = Spea2TournamentSelection(m_Archive);
		CIndividual* firstParent = m_Archive[firstParentIdx];
		CIndividual* secondParent = m_Archive[secondParentIdx];

		// TODO - prob. some of this can be shared with other implementations
		std::vector<TGene> firstChildGenes = firstParent->GetGenes();
		std::vector<TGene> secondChildGenes = secondParent->GetGenes();

		// Crossover
		if (CCrossoverOperator* crossover = m_Params.m_Crossover)
		{
			crossover->Crossover(firstParent->GetGenes(), secondParent->GetGenes(), firstChildGenes, secondChildGenes);
		}

		// Mutate
		if (CMutationOperator* mutation = m_Params.m_Mutation)
		{
			mutation->Mutate(firstChildGenes);
			mutation->Mutate(secondChildGenes);
		}

		// Update population in place, as we use source genes from the archive
		m_Population[i]->SetGenes(firstChildGenes);
		m_Population[i]->Evaluate(m_ProblemDefinition);
		m_Population[i + 1]->SetGenes(firstChildGenes);
		m_Population[i + 1]->Evaluate(m_ProblemDefinition);
	}
}

bool CSPEA2::StopCondition()
{
	return m_Generation >= m_Params.m_GenerationLimit;
}


void CSPEA2::BuildNeighborhood(std::vector<CIndividual*>& individuals, std::vector<TNeighborhood>& neighborhood)
{
	// we ignore distance to self
	neighborhood = std::vector<TNeighborhood>(individuals.size(), TNeighborhood(individuals.size() - 1));
	size_t popSize = individuals.size();

	for (size_t i = 0; i < popSize; ++i)
	{
		size_t entryIdx = 0;
		for (size_t j = 0; j < popSize; ++j)
		{
			// just skip if self
			if (i == j)
			{
				continue;
			}

			std::pair<size_t, float>& entry = neighborhood[i][entryIdx];
			entry.first = j;
			entry.second = CalcDist(*individuals[i], *individuals[j]);
			++entryIdx;
		}
	}

	// Sort ascending by distance
	for (TNeighborhood& nh : neighborhood)
	{
		std::sort(nh.begin(), nh.end(), [](const std::pair<size_t, float>& a, const std::pair<size_t, float>& b) -> bool
		{
			return a.second < b.second;
		});
	}
}

float CSPEA2::CalcDist(const CIndividual& leftInd, const CIndividual& rightInd)
{
	size_t objCount = m_ProblemDefinition->GetObjCount();
	float dist = 0.f;
	for (int i = 0; i < objCount; ++i)
	{
	// non norm
		dist += powf(rightInd.GetEvalValue(i) - leftInd.GetEvalValue(i), 2);
	// TODO
	// normalize - important (much better results)
		//dist += powf((rightInd.GetEvalValue(i) - leftInd.GetEvalValue(i)) / m_QualityMeasures.GetNadirEvalValue(i), 2);
	// alt normalize
		//dist += powf((rightInd.GetEvalValue(i) - leftInd.GetEvalValue(i)) / (m_QualityMeasures.GetNadirEvalValue(i) - m_QualityMeasures.GetIdealEvalValue(i)), 2);
	}
	return sqrtf(dist);
}

void CSPEA2::UpdateFineGrainedFitness(std::vector<CIndividual*>& individuals, const std::vector<TNeighborhood>& neighborhood)
{
	UpdateRawFitness(individuals);
	UpdateDensity(individuals, neighborhood);
}

void CSPEA2::UpdateRawFitness(std::vector<CIndividual*>& individuals)
{
	size_t indSize = individuals.size();
	std::vector<size_t> strengthVector(indSize, 0);
	std::vector<std::vector<size_t>> dominators(indSize, std::vector<size_t>());

	// R(i) -> rank
	for (size_t i = 0; i < indSize; ++i)
	{
		for (size_t j = 0; j < indSize; ++j)
		{
			if (i == j)
				continue;

			if (individuals[i]->IsDominatedBy(individuals[j]))
			{
				// Mark that Ind J dominates Ind I
				dominators[i].push_back(j);
				// Increase Ind J strength
				strengthVector[j] += 1;
			}
		}
	}

	for (size_t i = 0; i < indSize; ++i)
	{
		size_t totalDominatorsStrength = 0;
		for (size_t domIdx : dominators[i])
		{
			totalDominatorsStrength += strengthVector[domIdx];
		}
		individuals[i]->SetRank(totalDominatorsStrength);
	}
}

void CSPEA2::UpdateDensity(std::vector<CIndividual*>& individuals, const std::vector<TNeighborhood>& neighborhood)
{
	// D(i) -> CrowdDist
	size_t k = sqrtf(m_Params.m_PopulationSize + m_Params.m_ArchiveSize);

	for (size_t i = 0; i < individuals.size(); ++i)
	{
		individuals[i]->SetCrowdingDist(neighborhood[i][k].second);
	}
}

void CSPEA2::EnviroSelection(std::vector<CIndividual*>& individuals)
{
	// Not sure, but I suppose we don't save previous step archive
	size_t archiveSize = m_Params.m_ArchiveSize;
	for (CIndividual* ind : m_Archive)
	{
		delete ind;
	}
	m_Archive.clear();
	m_Archive.reserve(archiveSize);

	std::vector<const CIndividual*> nonDominatedIndividuals;
	std::vector<const CIndividual*> dominatedIndividuals;
	SplitByDomination(individuals, dominatedIndividuals, nonDominatedIndividuals);

	int sizeDiff = (int)archiveSize - (int)nonDominatedIndividuals.size();
	if (sizeDiff > 0)
	{
		// Add dominated individuals with good results
		std::sort(dominatedIndividuals.begin(), dominatedIndividuals.end(), [](const CIndividual* a, const CIndividual* b) -> bool
		{
			return (a->GetRank() + a->GetCrowdingDist()) < (b->GetRank() + b->GetCrowdingDist());
		});
		for (int i = 0; i < sizeDiff; ++i)
		{
			nonDominatedIndividuals.push_back(dominatedIndividuals[i]);
		}
	}
	else if (nonDominatedIndividuals.size() > archiveSize)
	{
		// Remove most crowded individuals
		TruncateByDistance(nonDominatedIndividuals, archiveSize);
	}

	// Finally, add new individuals to the archive
	for (const CIndividual* filteredInd : nonDominatedIndividuals)
	{
		m_Archive.push_back(new CIndividual(*filteredInd));
	}
}

void CSPEA2::SplitByDomination(std::vector<CIndividual*>& individuals, std::vector<const CIndividual*>& dominatedIndividuals, std::vector<const CIndividual*>& nonDominatedIndividuals)
{
	nonDominatedIndividuals.clear();
	nonDominatedIndividuals.reserve(individuals.size());
	dominatedIndividuals.clear();
	dominatedIndividuals.reserve(individuals.size());
	// For each new individual, check if not dominated
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
					isDominated = newInd->IsDuplicateEvalValue(individuals[i]);
				}
			}
			++i;
		}
		if (isDominated)
		{
			dominatedIndividuals.push_back(newInd);
		}
		else
		{
			nonDominatedIndividuals.push_back(newInd);
		}
	}
}

void CSPEA2::TruncateByDistance(std::vector<const CIndividual*>& filteredIndividuals, size_t maxSize)
{
	// TODO - it recalculates all the distances every time, try to do it once
	// also simplified - remove first closest point
	while (filteredIndividuals.size() > maxSize)
	{
		float maxDist = 0.f;
		size_t toRemove = 0;
		for (size_t i = 0; i < filteredIndividuals.size(); ++i)
		{
			for (size_t j = i + 1; j < filteredIndividuals.size(); ++j)
			{
				float d = CalcDist(*filteredIndividuals[i], *filteredIndividuals[j]);
				if (d > maxDist)
				{
					maxDist = d;
					toRemove = i;
				}
			}
		}

		filteredIndividuals.erase(filteredIndividuals.begin() + toRemove);
	}
}

size_t CSPEA2::Spea2TournamentSelection(const std::vector<CIndividual*>& population)
{
	// binary selection
	size_t tournamentSize = 2;
	size_t popSize = population.size();

	size_t bestIdx = m_Random.NextIndex(popSize);
	// fitness to be minimized
	float bestFitness = (float)population[bestIdx]->GetRank() + population[bestIdx]->GetCrowdingDist();

	for (size_t i = 1; i < tournamentSize; ++i)
	{
		size_t randomIdx = m_Random.NextIndex(popSize);
		float fitness = (float)population[randomIdx]->GetRank() + population[randomIdx]->GetCrowdingDist();
		if (fitness < bestFitness)
		{
			bestFitness = fitness;
			bestIdx = randomIdx;
		}
	}

	return bestIdx;
}

CSPEA2::SSPEA2Params::SSPEA2Params(const SOptimizerParams& optParams, const IProblem* optProblem)
	: SOptimizerParams(optParams)
{
	m_ConfigLines.TakeValue("PopulationSize", m_PopulationSize);
	// TODO - maybe change it to explicit parameter
	m_ArchiveSize = m_PopulationSize;
	m_ConfigLines.TakeValue("GenerationLimit", m_GenerationLimit);

	std::string mutationInitString;
	m_ConfigLines.TakeValue("Mutation", mutationInitString);
	m_Mutation = reinterpret_cast<CMutationOperator *>(COperators::CreateMutationOperator(mutationInitString,
                                                                                          optProblem,
                                                                                          optParams.m_SeedValue));

	std::string crossoverInitString;
	m_ConfigLines.TakeValue("Crossover", crossoverInitString);
	m_Crossover = COperators::CreateCrossoverOperator(crossoverInitString, optProblem, optParams.m_SeedValue);

	// TODO - write if missing or any remaining
}

CSPEA2::SSPEA2Params::~SSPEA2Params()
{
	if (m_Mutation)
	{
		delete m_Mutation;
		m_Mutation = nullptr;
	}

	if (m_Crossover)
	{
		delete m_Crossover;
		m_Crossover = nullptr;
	}
}
