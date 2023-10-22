#include <cfloat>
#include "NSGAII.h"
#include "Optimizers/EvolutionaryAlgorithms/Clustering/NonDominatedSorting.h"
#include "Optimizers/EvolutionaryAlgorithms/ArchiveUtils.h"

CNSGAII::CNSGAII(const SOptimizerParams& optParams, const IProblem* optProblem)
	: m_Random(optParams.m_SeedValue)
	, m_ProblemDefinition(optProblem)
	, m_Operators(&m_Random)
	, m_Params(optParams, optProblem)
{
}

CNSGAII::~CNSGAII()
{
	Reset();
}

SOptimizationResult CNSGAII::RunOptimization()
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

	while (!StopCondition())
	{
		Evolve();

		std::vector<CIndividual*> combinedPop;
		combinedPop.reserve(m_Population.size() + m_NextPopulation.size());
		combinedPop.insert(combinedPop.end(), m_Population.begin(), m_Population.end());
		combinedPop.insert(combinedPop.end(), m_NextPopulation.begin(), m_NextPopulation.end());


		CNonDominatedSorting nonDominatedSorting;
		std::vector<std::vector<size_t>> combinedClusters;
		nonDominatedSorting.Cluster(combinedPop, combinedClusters);

		std::vector<CIndividual*> tempPopulation;
		tempPopulation.reserve(m_Params.m_PopulationSize);
		for (size_t i = 0; tempPopulation.size() < m_Params.m_PopulationSize; ++i)
		{
			std::vector<size_t>& cluster = combinedClusters[i];
			// if this cluster exceeds the size
			if (tempPopulation.size() + cluster.size() > m_Params.m_PopulationSize)
			{
				CalcCrowdingDistance(combinedPop, cluster);
				std::sort(cluster.begin(), cluster.end(), [combinedPop](const size_t& a, const size_t& b) -> bool
				{
					return combinedPop[a]->GetCrowdingDist() < combinedPop[b]->GetCrowdingDist();
				});
				for (size_t idx : cluster)
				{
					// deep copy individuals as they fill be the new population
					tempPopulation.push_back(new CIndividual(*combinedPop[idx]));
					if (tempPopulation.size() >= m_Params.m_PopulationSize)
						break;
				}
			}
			else
			{
				for (size_t idx : cluster)
				{
					tempPopulation.push_back(new CIndividual(*combinedPop[idx]));
				}
			}
		}

		// Clear both current and next population
		for (CIndividual* ind : combinedPop)
		{
			delete ind;
		}
		m_Population.clear();
		m_NextPopulation.clear();

		m_Population.swap(tempPopulation);
		m_NextPopulation.reserve(m_Population.size());

		++m_Generation;
	}

	return SOptimizationResult
	{
		ArchiveUtils::ToEvaluation(m_Archive)
	};
}

void CNSGAII::Reset()
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

void CNSGAII::Evolve()
{
	// TODO - this could be shared with NTGA2
	for (size_t i = 0; i < m_Params.m_PopulationSize; i += 2)
	{
		size_t firstParentIdx = m_Operators.RankedTournamentSelection(m_Population, m_Params.m_TourSize);
		size_t secondParentIdx = m_Operators.RankedTournamentSelection(m_Population, m_Params.m_TourSize);
		CIndividual* firstParent = m_Population[firstParentIdx];
		CIndividual* secondParent = m_Population[secondParentIdx];

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
	ArchiveUtils::CopyToArchiveWithFiltering(m_NextPopulation, m_Archive);
}

void CNSGAII::CalcCrowdingDistance(std::vector<CIndividual*>& population, std::vector<size_t>& indices)
{
	for (int objIdx = 0; objIdx < m_ProblemDefinition->GetObjCount(); ++objIdx)
	{
		std::sort(indices.begin(), indices.end(), [objIdx, population](const size_t& a, const size_t& b) -> bool
		{
			return population[a]->GetEvalValue(objIdx) < population[b]->GetEvalValue(objIdx);
		});

		population[indices[0]]->SetCrowdingDist(FLT_MAX);
		population[indices[indices.size() - 1]]->SetCrowdingDist(FLT_MAX);

		for (size_t i = 1; i < indices.size() - 1; ++i)
		{
			// TODO - add normalization?
			float currDist = population[indices[i]]->GetCrowdingDist();
			float plusValue = population[indices[i + 1]]->GetEvalValue(objIdx);
			float minusValue = population[indices[i - 1]]->GetEvalValue(objIdx);
			population[indices[i]]->SetCrowdingDist(currDist + plusValue - minusValue);
		}
	}
}

bool CNSGAII::StopCondition()
{
	return m_Generation >= m_Params.m_GenerationLimit;
}

CNSGAII::SNSGAII2Params::SNSGAII2Params(const SOptimizerParams& optParams, const IProblem* optProblem)
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
}

CNSGAII::SNSGAII2Params::~SNSGAII2Params()
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