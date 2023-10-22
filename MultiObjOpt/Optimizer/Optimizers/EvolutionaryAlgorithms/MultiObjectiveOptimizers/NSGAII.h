#pragma once

#include <vector>
#include "Optimizers/EvolutionaryAlgorithms/Operators/Operators.h"
#include "Optimizers/Optimizer.h"

class CNSGAII : public IOptimizer
{
public:
	CNSGAII(const SOptimizerParams& optParams, const IProblem* optProblem);
	~CNSGAII();

	SOptimizationResult RunOptimization() override;

	const std::vector<CIndividual*>& GetArchive() const { return m_Archive; }

private:

	void Reset();
	void Evolve();
	void CalcCrowdingDistance(std::vector<CIndividual*>& population, std::vector<size_t>& indices);
	bool StopCondition();

	const IProblem* m_ProblemDefinition = nullptr;
	int m_Generation = 0;
	std::vector<CIndividual*> m_Population;
	std::vector<CIndividual*> m_NextPopulation;
	std::vector<CIndividual*> m_Archive;

	CRandom m_Random;
	COperators m_Operators;

	struct SNSGAII2Params : SOptimizerParams
	{
		SNSGAII2Params(const SOptimizerParams& optParams, const IProblem* optProblem);
		~SNSGAII2Params();

		size_t m_PopulationSize = 0;
		size_t m_GenerationLimit = 0;
		size_t m_TourSize = 0;

		CMutationOperator* m_Mutation = nullptr;
		CCrossoverOperator* m_Crossover = nullptr;
	};

	SNSGAII2Params m_Params;
};