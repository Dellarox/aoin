#pragma once
#include "Utils/Random.h"
#include "Optimizers/EvolutionaryAlgorithms/Individual/Individual.h"

class IProblem;

class CMutationOperator
{
public:
	
	CMutationOperator(const IProblem* problem, int seedValue);

	virtual void Mutate(std::vector<TGene> &genes);

protected:

	const IProblem* m_Problem = nullptr;
	CRandom m_Random;
};