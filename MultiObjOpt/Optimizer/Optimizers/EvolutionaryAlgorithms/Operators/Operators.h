#pragma once

#include "Utils/Random.h"
#include "Optimizers/EvolutionaryAlgorithms/Individual/Individual.h"
#include "CrossoverOperator.h"
#include "MutationOperator.h"
#include "Optimizers/EvolutionaryAlgorithms/Individual/GeneModifier.h"
#include "MutationOperator.h"

static const int MAX_RANDOM_BINARY_INDEX = 2;

class COperators
{
public:

	COperators(CRandom* random);

	// Initialization
	void RandomInit(std::vector<TGene>& genotype, const SProblemEncoding& encoding);

	// Selection
	size_t RankedTournamentSelection(const std::vector<CIndividual*>& population, size_t tourSize);
    size_t FitnessTournamentSelection(const std::vector<CIndividual *> &population, size_t tourSize,
                                      const std::vector<float>& objectiveWeights);

	static CMutationOperator * CreateMutationOperator(const std::string& initString, const IProblem* problemDef, int seedValue);
	static CCrossoverOperator* CreateCrossoverOperator(const std::string& initString, const IProblem* problemDef, int seedValue);

private:

	CRandom* m_Random = nullptr;
	CGeneModifier m_GeneMod;
};