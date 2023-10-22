#pragma once
#include "Utils/Random.h"
#include "Optimizers/EvolutionaryAlgorithms/Individual/Individual.h"

class IProblem;

class CCrossoverOperator
{
public:

	CCrossoverOperator(const IProblem* problem, int seedValue);

	virtual void Crossover(const std::vector<TGene>& leftParentGenes, const std::vector<TGene>& rightParentGenes, 
		std::vector<TGene>& leftChildGenes, std::vector<TGene>& rightChildGenes);

protected:

	const IProblem* m_Problem = nullptr;
	CRandom m_Random;
};