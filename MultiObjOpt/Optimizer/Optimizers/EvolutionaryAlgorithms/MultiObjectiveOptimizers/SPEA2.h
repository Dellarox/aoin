#pragma once
#include <vector>
#include "Optimizers/EvolutionaryAlgorithms/Operators/Operators.h"
#include "Optimizers/Optimizer.h"

class IProblem;
class CMutationOperator;
class CCrossoverOperator;

class CSPEA2 : public IOptimizer
{
public:

	CSPEA2(const SOptimizerParams& optParams, const IProblem* optProblem);
	~CSPEA2();

	SOptimizationResult RunOptimization() override;

	const std::vector<CIndividual*>& GetArchive() const { return m_Archive; }

private:

	using TNeighborhood = std::vector<std::pair<size_t, float>>;

	void Reset();
	void Evolve();
	bool StopCondition();

	void BuildNeighborhood(std::vector<CIndividual*>& individuals, std::vector<TNeighborhood>& neighborhood);
	float CalcDist(const CIndividual& leftInd, const CIndividual& rightInd);
	void UpdateFineGrainedFitness(std::vector<CIndividual*>& individuals, const std::vector<TNeighborhood>& neighborhood);
	void UpdateRawFitness(std::vector<CIndividual*>& individuals);
	void UpdateDensity(std::vector<CIndividual*>& individuals, const std::vector<TNeighborhood>& neighborhood);
	void EnviroSelection(std::vector<CIndividual*>& individuals);
	void SplitByDomination(std::vector<CIndividual*>& individuals, std::vector<const CIndividual*>& dominatedIndividuals, std::vector<const CIndividual*>& nonDominatedIndividuals);
	void TruncateByDistance(std::vector<const CIndividual*>& filteredIndividuals, size_t maxSize);
	size_t Spea2TournamentSelection(const std::vector<CIndividual*>& population);
	void Mutate(std::vector<TGene>& genotype, const float& mutationProbability);

	const IProblem* m_ProblemDefinition = nullptr;
	int m_Generation = 0;
	std::vector<CIndividual*> m_Population;
	std::vector<CIndividual*> m_Archive;

	CRandom m_Random;
	COperators m_Operators;

	struct SSPEA2Params : SOptimizerParams
	{
		SSPEA2Params(const SOptimizerParams& optParams, const IProblem* optProblem);
		~SSPEA2Params();

		size_t m_PopulationSize = 0;
		size_t m_ArchiveSize = 0;
		size_t m_GenerationLimit = 0;

		CMutationOperator* m_Mutation = nullptr;
		CCrossoverOperator* m_Crossover = nullptr;
	};

	SSPEA2Params m_Params;
};