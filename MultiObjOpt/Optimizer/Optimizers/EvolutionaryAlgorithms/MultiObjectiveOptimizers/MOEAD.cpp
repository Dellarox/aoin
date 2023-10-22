#include "MOEAD.h"
#include "Optimizers/EvolutionaryAlgorithms/ArchiveUtils.h"
#include "Utils/DasDennis.h"

CMOEAD::CMOEAD(const SOptimizerParams& optParams, const IProblem* optProblem)
	: m_Random(optParams.m_SeedValue)
	, m_ProblemDefinition(optProblem)
	, m_Operators(&m_Random)
	, m_Params(optParams, optProblem)
{
}

CMOEAD::~CMOEAD()
{
	Reset();
}

SOptimizationResult CMOEAD::RunOptimization()
{
	Reset();

	ConstructSubproblems(m_Params.m_PartitionsNumber, m_Params.m_NeighberhoodSize);
	size_t popSize = m_Subproblems.size();

	// TODO - this can be shared with other methods
	m_Population.reserve(popSize);
	for (size_t i = 0; i < popSize; ++i)
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
		++m_Generation;
	}

	return SOptimizationResult
	{
		ArchiveUtils::ToEvaluation(m_Archive)
	};
}

void CMOEAD::Reset()
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

void CMOEAD::Evolve()
{
	// Temp individual used for testing new child genes
	CIndividual testIndividual;

	for (size_t i = 0; i < m_Population.size(); ++i)
	{
		const SSubproblem& sp = m_Subproblems[i];
		const size_t nhSize = sp.m_Neighborhood.size();

		// TODO - prob. some of this can be shared with other implementations
		size_t firstParentIdx = sp.m_Neighborhood[m_Random.NextIndex(nhSize)];
		size_t secondParentIdx = sp.m_Neighborhood[m_Random.NextIndex(nhSize)];
		CIndividual* firstParent = m_Population[firstParentIdx];
		CIndividual* secondParent = m_Population[secondParentIdx];

		std::vector<TGene> firstChildGenes = firstParent->GetGenes();
		std::vector<TGene> secondChildGenes = secondParent->GetGenes();

		// Crossover
		if (CCrossoverOperator* crossover = m_Params.m_Crossover)
		{
			crossover->Crossover(firstParent->GetGenes(), secondParent->GetGenes(), firstChildGenes, secondChildGenes);
			// Crossover - we take only one child
		}

		// Mutate
		if (CMutationOperator* mutation = m_Params.m_Mutation)
		{
			mutation->Mutate(firstChildGenes);
			mutation->Mutate(secondChildGenes);
		}

		testIndividual.SetGenes(firstChildGenes);
		testIndividual.Evaluate(m_ProblemDefinition);

		// Now check if any neighborhood solution is improved
		for (size_t j : sp.m_Neighborhood)
		{
			CIndividual& oldInd = *m_Population[j];
			if (IsBetterInSubproblem(testIndividual, oldInd, m_Subproblems[j]))
			{
				// I assume we can just copy whole individual
				oldInd = testIndividual;
			}
		}

		ArchiveUtils::CopyToArchiveWithFiltering(&testIndividual, m_Archive);
	}
}

bool CMOEAD::IsBetterInSubproblem(const CIndividual& newIndividual, const CIndividual& oldIndividual, const SSubproblem& sp)
{
	// Simple weighted sum
	float newValue = 0.f;
	float oldValue = 0.f;
	const size_t objCount = m_ProblemDefinition->GetObjCount();
	for (size_t i = 0; i < objCount; ++i)
	{
		newValue += newIndividual.GetEvalValue(i) * sp.m_WeightVector[i];
		oldValue += oldIndividual.GetEvalValue(i) * sp.m_WeightVector[i];

		// TODO
		// Normalized
		//float minVal = m_QualityMeasures.GetIdealEvalValue(i);
		//float maxVal = m_QualityMeasures.GetNadirEvalValue(i);
		//float diffVal = maxVal - minVal;
		//newValue += (newIndividual.GetEvalValue(i) - minVal) / diffVal * sp.m_WeightVector[i];
		//oldValue += (oldIndividual.GetEvalValue(i) - minVal) / diffVal * sp.m_WeightVector[i];
	}
	// Minimization
	return newValue < oldValue;
}

void CMOEAD::ConstructSubproblems(size_t partitionsNumber, size_t neighborhoodSize)
{
	size_t dimCount = m_ProblemDefinition->GetObjCount();

	if (dimCount == 2)
	{
		ConstructSubproblemsSimple2D(partitionsNumber, neighborhoodSize);
	}
	else
	{
		ConstructSubproblemsMultiD(partitionsNumber, neighborhoodSize, dimCount);
	}
}

void CMOEAD::ConstructSubproblemsSimple2D(size_t partitionsNumber, size_t neighborhoodSize)
{
	m_Subproblems.clear();

	// Simple impl for 2dim problems only
	float stepVal = 1.f / (partitionsNumber - 1);
	for (size_t i = 0; i < partitionsNumber; ++i)
	{
		m_Subproblems.emplace_back();
		SSubproblem& sp = m_Subproblems.back();
		sp.m_WeightVector[0] = 0.f + i * stepVal;
		sp.m_WeightVector[1] = 1.f - i * stepVal;
	}

	// For 2 dim, we assume they are ordered
	for (size_t i = 0; i < partitionsNumber; ++i)
	{
		int leftIdx = (int)i - ((int)neighborhoodSize / 2);
		if (leftIdx < 0) leftIdx = 0;
		size_t rightIdx = leftIdx + neighborhoodSize;
		if (rightIdx >= partitionsNumber)
		{
			// We assume that neighborhood is always smaller than subproblems size
			size_t excees = rightIdx - partitionsNumber;
			leftIdx -= excees;
			rightIdx -= excees;
		}
		for (size_t j = leftIdx; j < rightIdx; ++j)
		{
			m_Subproblems[i].m_Neighborhood.push_back(j);
		}
	}
}

void CMOEAD::ConstructSubproblemsMultiD(size_t partitionsNumber, size_t neighborhoodSize, size_t dimCount)
{
	m_Subproblems.clear();

	// Generate subproblems
	DasDennis ddGenerator(partitionsNumber, dimCount);
	size_t subVecCount = ddGenerator.GetPointsNumber();
	ddGenerator.GeneratePoints();
	std::vector<std::vector<float>> subproblemVectors = ddGenerator.GetPoints();

	for (const auto& subVec : subproblemVectors)
	{
		m_Subproblems.emplace_back();
		SSubproblem& sp = m_Subproblems.back();
		for (size_t j = 0; j < dimCount; ++j)
		{
			sp.m_WeightVector[j] = subVec[j];
		}
	}

	// Calculate distances
	using TNeighborhood = std::vector<std::pair<size_t, float>>;
	std::vector<TNeighborhood> neighborhood = std::vector<TNeighborhood>(subVecCount, TNeighborhood(subVecCount));
	for (size_t i = 0; i < subVecCount; ++i)
	{
		auto& weightVectorI = m_Subproblems[i].m_WeightVector;
		size_t entryIdx = 0;
		for (size_t j = 0; j < subVecCount; ++j)
		{
			auto& weightVectorJ = m_Subproblems[j].m_WeightVector;
			std::pair<size_t, float>& entry = neighborhood[i][entryIdx];
			entry.first = j;

			float dist = 0.f;
			if (i != j)
			{
				for (size_t d = 0; d < dimCount; ++d)
				{
					dist += powf(weightVectorI[d] - weightVectorJ[d], 2);
				}
				dist = sqrtf(dist);
			}
			entry.second = dist;
			++entryIdx;
		}
	}

	// Link neighborhood
	for (TNeighborhood& nh : neighborhood)
	{
		// Sort ascending by distance, including distance to self
		std::sort(nh.begin(), nh.end(), [](const std::pair<size_t, float>& a, const std::pair<size_t, float>& b) -> bool
		{
			return a.second < b.second;
		});
	}

	for (size_t i = 0; i < subVecCount; ++i)
	{
		for (size_t j = 0; j < neighborhoodSize; ++j)
		{
			m_Subproblems[i].m_Neighborhood.push_back(neighborhood[i][j].first);
		}
	}
}

bool CMOEAD::StopCondition()
{
	return m_Generation >= m_Params.m_GenerationLimit;
}

CMOEAD::SMOEADParams::SMOEADParams(const SOptimizerParams& optParams, const IProblem* optProblem)
	: SOptimizerParams(optParams)
{
	m_ConfigLines.TakeValue("PartitionsNumber", m_PartitionsNumber);
	m_ConfigLines.TakeValue("NeighberhoodSize", m_NeighberhoodSize);
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

CMOEAD::SMOEADParams::~SMOEADParams()
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
