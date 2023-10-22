#pragma once
#include <vector>
#include "Optimizers/EvolutionaryAlgorithms/Operators/Operators.h"
#include "Optimizers/Optimizer.h"

class IProblem;
class CMutationOperator;
class CCrossoverOperator;

class CNTGA2 : public IOptimizer
{
public:

	CNTGA2(const SOptimizerParams& optParams, const IProblem* optProblem);
	~CNTGA2();

	SOptimizationResult RunOptimization() override;

	const std::vector<CIndividual*>& GetArchive() const { return m_Archive; }

private:

	void Reset();
	void Evolve();
	void NTSelection(std::vector<CIndividual*>& parentsVector);
	void GapSelectionByRandomDim(std::vector<CIndividual*>& parentsVector);
	void SelectByGap(std::vector<CIndividual*>& parentsVector, const std::vector<float>& gapValues, size_t objectiveIdx);
	void CrossoverAndMutate(const size_t& selectionIndex, CIndividual* firstParent, CIndividual* secondParent);
	bool StopCondition();


	const IProblem* m_ProblemDefinition = nullptr;
	int m_Generation = 0;
	std::vector<CIndividual*> m_Population;
	std::vector<CIndividual*> m_NextPopulation;
	std::vector<CIndividual*> m_Archive;

	CRandom m_Random;
	COperators m_Operators;

	struct SNTGA2Params : SOptimizerParams
	{
		SNTGA2Params(const SOptimizerParams& optParams, const IProblem* optProblem);
		~SNTGA2Params();

		size_t m_PopulationSize = 0;
		size_t m_GenerationLimit = 0;
		int m_GapSelectionPercent = 0;
		size_t m_TourSize = 0;

		CMutationOperator* m_Mutation = nullptr;
		CCrossoverOperator* m_Crossover = nullptr;
	};

	SNTGA2Params m_Params;
};