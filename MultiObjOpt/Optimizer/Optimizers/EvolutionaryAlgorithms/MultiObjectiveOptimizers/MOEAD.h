#pragma once
#include <vector>
#include "Optimizers/EvolutionaryAlgorithms/Operators/Operators.h"
#include "Optimizers/Optimizer.h"

class IProblem;
class CMutationOperator;
class CCrossoverOperator;

class CMOEAD : public IOptimizer
{
public:

	CMOEAD(const SOptimizerParams& optParams, const IProblem* optProblem);
	~CMOEAD();

	SOptimizationResult RunOptimization() override;

	const std::vector<CIndividual*>& GetArchive() const { return m_Archive; }

private:

	struct SSubproblem
	{
		std::vector<float> m_WeightVector;
		std::vector<size_t> m_Neighborhood;
	};

	std::vector<SSubproblem> m_Subproblems;

	void Reset();
	void Evolve();
	bool IsBetterInSubproblem(const CIndividual& newIndividual, const CIndividual& oldIndividual, const SSubproblem& sp);
	void ConstructSubproblems(size_t partitionsNumber, size_t neighborhoodSize);
	void ConstructSubproblemsSimple2D(size_t partitionsNumber, size_t neighborhoodSize);
	void ConstructSubproblemsMultiD(size_t partitionsNumber, size_t neighborhoodSize, size_t dimCount);
	bool StopCondition();

	const IProblem* m_ProblemDefinition = nullptr;
	int m_Generation = 0;
	std::vector<CIndividual*> m_Population;
	std::vector<CIndividual*> m_Archive;

	CRandom m_Random;
	COperators m_Operators;

	struct SMOEADParams : SOptimizerParams
	{
		SMOEADParams(const SOptimizerParams& optParams, const IProblem* optProblem);
		~SMOEADParams();

		size_t m_PartitionsNumber = 0;
		size_t m_NeighberhoodSize = 0;
		size_t m_GenerationLimit = 0;
		size_t m_TourSize = 0;

		CMutationOperator* m_Mutation = nullptr;
		CCrossoverOperator* m_Crossover = nullptr;
	};

	SMOEADParams m_Params;
};