#include <cfloat>
#include "NTGA2.h"
#include "Optimizers/EvolutionaryAlgorithms/Clustering/NonDominatedSorting.h"
#include "Optimizers/EvolutionaryAlgorithms/ArchiveUtils.h"

#define REC_SEC_PARENT 1

CNTGA2::CNTGA2(const SOptimizerParams& optParams, const IProblem* optProblem)
	: m_Random(optParams.m_SeedValue)
	, m_ProblemDefinition(optProblem)
	, m_Operators(&m_Random)
	, m_Params(optParams, optProblem)
{
}

CNTGA2::~CNTGA2()
{
	Reset();
}

SOptimizationResult CNTGA2::RunOptimization()
{
	Reset();

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

	ArchiveUtils::CopyToArchiveWithFiltering(m_Population, m_Archive);

	while(!StopCondition())
	{
		Evolve();

		for (CIndividual* ind : m_Population)
		{
			delete ind;
		}
		m_Population = m_NextPopulation;
		m_NextPopulation.clear();
		m_NextPopulation.reserve(m_Population.size());

		++m_Generation;
	}

	return SOptimizationResult
	{
		ArchiveUtils::ToEvaluation(m_Archive)
	};
}

void CNTGA2::Reset()
{
	m_Generation = 0;

	for (CIndividual* ind : m_Population)
	{
		delete ind;
	}
	m_Population.clear();

	for (CIndividual* ind : m_NextPopulation)
	{
		delete ind;
	}
	m_NextPopulation.clear();

	for (CIndividual* ind : m_Archive)
	{
		delete ind;
	}
	m_Archive.clear();
}

void CNTGA2::Evolve()
{
	if ((m_Generation % 100) < (100 - m_Params.m_GapSelectionPercent))
	{
		// Without Gap
		std::vector<CIndividual*> parentsVector;
		parentsVector.reserve(m_Archive.size() + m_Population.size());
		parentsVector.insert(parentsVector.end(), m_Archive.begin(), m_Archive.end());
		parentsVector.insert(parentsVector.end(), m_Population.begin(), m_Population.end());
		NTSelection(parentsVector);
	}
	else
	{
		// Anneal with Gap
		GapSelectionByRandomDim(m_Archive);
	}
	ArchiveUtils::CopyToArchiveWithFiltering(m_NextPopulation, m_Archive);
}

void CNTGA2::NTSelection(std::vector<CIndividual*>& parentsVector)
{
	size_t parentsSize = parentsVector.size();

	// Calculate ranks using NonDominatedSorting
	CNonDominatedSorting nonDominatedSorting;
	std::vector<std::vector<size_t>> combinedClusters;
	nonDominatedSorting.Cluster(parentsVector, combinedClusters);

	for (size_t i = 0; i < m_Params.m_PopulationSize; i += 2)
	{
		// Select two parent and don't care if they are the same
		size_t firstParentIdx = m_Operators.RankedTournamentSelection(parentsVector, m_Params.m_TourSize);
		size_t secondParentIdx = m_Operators.RankedTournamentSelection(parentsVector, m_Params.m_TourSize);

		CrossoverAndMutate(i, parentsVector[firstParentIdx], parentsVector[secondParentIdx]);
	}
}

void CNTGA2::GapSelectionByRandomDim(std::vector<CIndividual*>& parentsVector)
{
	// Sort Archive and calculate gaps
	// Sorted by a random objective
	size_t objectiveId = m_Random.NextIndex(m_ProblemDefinition->GetObjCount());

	std::sort(parentsVector.begin(), parentsVector.end(), [objectiveId](const CIndividual* a, const CIndividual* b) -> bool
		{
			return a->GetEvalValue(objectiveId) < b->GetEvalValue(objectiveId);
		});

	// Calculate Gaps
	size_t frontSize = parentsVector.size();
	std::vector<float> gapValues(frontSize, 0.f);

	gapValues[0] = FLT_MAX;
	gapValues[frontSize - 1] = FLT_MAX;

	for (size_t i = 1; i < frontSize - 1; ++i)
	{
		float iValue = parentsVector[i]->GetEvalValue(objectiveId);
		gapValues[i] = fmaxf(iValue - parentsVector[i - 1]->GetEvalValue(objectiveId), parentsVector[i + 1]->GetEvalValue(objectiveId) - iValue);
	}

	SelectByGap(parentsVector, gapValues, objectiveId);
}

void CNTGA2::SelectByGap(std::vector<CIndividual*>& parentsVector, const std::vector<float>& gapValues, size_t objectiveIdx)
{
	size_t parentsSize = parentsVector.size();

	// Backup in case of 1 solution in archive
	if (parentsSize < 2)
	{
		for (size_t i = 0; i < m_Params.m_PopulationSize; i += 2)
		{
			CrossoverAndMutate(i, parentsVector[0], parentsVector[0]);
		}
		return;
	}

	for (size_t i = 0; i < m_Params.m_PopulationSize; i += 2)
	{
		size_t tournamentSize = m_Params.m_TourSize;
		size_t firstParentIdx = m_Random.NextIndex(parentsSize);
		float bestGap = gapValues[firstParentIdx];

		for (size_t i = 1; i < tournamentSize; ++i)
		{
			size_t randomIdx = m_Random.NextIndex(parentsSize);
			if (gapValues[randomIdx] > bestGap)
			{
				bestGap = gapValues[randomIdx];
				firstParentIdx = randomIdx;
			}
		}

#if REC_SEC_PARENT
		size_t secondParentIdx = firstParentIdx + (m_Random.NextIndex(2) == 0 ? 1 : -1);
		if (secondParentIdx < 0 || secondParentIdx >= parentsSize)
		{
			secondParentIdx = m_Random.NextIndex(parentsSize);
			bestGap = gapValues[secondParentIdx];

			for (size_t i = 1; i < tournamentSize; ++i)
			{
				size_t randomIdx = m_Random.NextIndex(parentsSize);
				if (gapValues[randomIdx] > bestGap)
				{
					bestGap = gapValues[randomIdx];
					secondParentIdx = randomIdx;
				}
			}
		}
#else
		size_t secondParentIdx = 0;
		if (firstParentIdx == 0)
		{
			secondParentIdx = 1;
		}
		else if (firstParentIdx == parentsSize - 1)
		{
			secondParentIdx = parentsSize - 2;
		}
		else
		{
			secondParentIdx = firstParentIdx + (m_Random.NextIndex(2) == 0 ? 1 : -1);
		}
#endif // REC_SEC_PARENT

		CrossoverAndMutate(i, parentsVector[firstParentIdx], parentsVector[secondParentIdx]);
	}
}

void CNTGA2::CrossoverAndMutate(const size_t& selectionIndex, CIndividual* firstParent, CIndividual* secondParent)
{
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

	CIndividual* firstInd = new CIndividual();
	firstInd->SetGenes(firstChildGenes);
	firstInd->Evaluate(m_ProblemDefinition);
	m_NextPopulation.push_back(firstInd);

	CIndividual* secondInd = new CIndividual();
	secondInd->SetGenes(secondChildGenes);
	secondInd->Evaluate(m_ProblemDefinition);
	m_NextPopulation.push_back(secondInd);
}

bool CNTGA2::StopCondition()
{
	return m_Generation >= m_Params.m_GenerationLimit;
}

CNTGA2::SNTGA2Params::SNTGA2Params(const SOptimizerParams& optParams, const IProblem* optProblem)
	: SOptimizerParams(optParams)
{
	m_ConfigLines.TakeValue("PopulationSize", m_PopulationSize);
	m_ConfigLines.TakeValue("GenerationLimit", m_GenerationLimit);
	m_ConfigLines.TakeValue("GapSelectionPercent", m_GapSelectionPercent);
	m_ConfigLines.TakeValue("TourSize", m_TourSize);

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

CNTGA2::SNTGA2Params::~SNTGA2Params()
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
