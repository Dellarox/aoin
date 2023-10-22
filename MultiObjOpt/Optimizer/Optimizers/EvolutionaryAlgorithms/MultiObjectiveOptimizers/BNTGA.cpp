#include <cfloat>
#include "BNTGA.h"
#include "Optimizers/EvolutionaryAlgorithms/ArchiveUtils.h"

#define USE_PP_DIST 0

CBNTGA::CBNTGA(const SOptimizerParams& optParams, const IProblem* optProblem)
	: m_Random(optParams.m_SeedValue)
	, m_ProblemDefinition(optProblem)
	, m_Operators(&m_Random)
	, m_Params(optParams, optProblem)
{
}

CBNTGA::~CBNTGA()
{
	Reset();
}

SOptimizationResult CBNTGA::RunOptimization()
{
	Reset();

	// TODO - some parts can be shared with NTGA2
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

	while (!StopCondition())
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

void CBNTGA::Reset()
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

void CBNTGA::Evolve()
{
	GapSelectionByRandomDim(m_Archive);
	ArchiveUtils::CopyToArchiveWithFiltering(m_NextPopulation, m_Archive);
}

void CBNTGA::GapSelectionByRandomDim(std::vector<CIndividual*>& parentsVector)
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

#if USE_PP_DIST
	gapValues[0] = parentsVector[0]->GetEvalValue(objectiveId) - m_QualityMeasures.GetIdealEvalValue(objectiveId);
	if (frontSize > 1)
	{
		// This is interesting how to evaluate the opposite edge point...
		gapValues[frontSize - 1] = parentsVector[frontSize - 1].GetEvalValue(objectiveId) - parentsVector[frontSize - 2].GetEvalValue(objectiveId);
	}
#else // USE_PP_DIST
	gapValues[0] = FLT_MAX;
	gapValues[frontSize - 1] = FLT_MAX;
#endif //USE_PP_DIST

	for (size_t i = 1; i < frontSize - 1; ++i)
	{
		float iValue = parentsVector[i]->GetEvalValue(objectiveId);
		gapValues[i] = fmaxf(iValue - parentsVector[i - 1]->GetEvalValue(objectiveId), parentsVector[i + 1]->GetEvalValue(objectiveId) - iValue);
	}

	size_t globalSelections = m_Generation * m_Population.size();
	float maxSpan = parentsVector[frontSize - 1]->GetEvalValue(objectiveId) - parentsVector[0]->GetEvalValue(objectiveId);
	float lambda = 1.f;

	for (size_t i = 0; i < frontSize; ++i)
	{
		size_t selected = parentsVector[i]->GetSelected();
		// Gap Balanced
		gapValues[i] = (gapValues[i] / (selected + 1));
		// Gap Balanced norm2 Lambda
		//gapValues[i] = (gapValues[i] / sqrtf(maxGap)) + sqrtf(log((float)globalSelections) / (selected + 1));
		// Gap Balanced norm Lambda
		//gapValues[i] = (gapValues[i] / (selected + 1)) + lambda * sqrtf(log((float)globalSelections * maxSpan) / (selected + 1));
		// TODO - try moving "* maxSpan" from log to sqrtf
		// Gap Balanced norm2 Lambda
		//gapValues[i] = (gapValues[i] / (selected + 1)) + lambda * sqrtf(log((float)globalSelections) * maxSpan / (selected + 1));
		// Gap Balanced Lambda 1
		//gapValues[i] = (gapValues[i] / (selected + 1)) + lambda * sqrtf(log((float)globalSelections) / (selected + 1));
	}

	SelectByGap(parentsVector, gapValues, objectiveId);
}

void CBNTGA::SelectByGap(std::vector<CIndividual*>& parentsVector, const std::vector<float>& gapValues, size_t objectiveIdx)
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

		parentsVector[firstParentIdx]->OnSelected();
		parentsVector[secondParentIdx]->OnSelected();
		CrossoverAndMutate(i, parentsVector[firstParentIdx], parentsVector[secondParentIdx]);
	}
}

void CBNTGA::CrossoverAndMutate(const size_t& selectionIndex, CIndividual* firstParent, CIndividual* secondParent)
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

bool CBNTGA::StopCondition()
{
	return m_Generation >= m_Params.m_GenerationLimit;
}

CBNTGA::SBNTGAParams::SBNTGAParams(const SOptimizerParams& optParams, const IProblem* optProblem)
	: SOptimizerParams(optParams)
{
	m_ConfigLines.TakeValue("PopulationSize", m_PopulationSize);
	m_ConfigLines.TakeValue("GenerationLimit", m_GenerationLimit);
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

CBNTGA::SBNTGAParams::~SBNTGAParams()
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
