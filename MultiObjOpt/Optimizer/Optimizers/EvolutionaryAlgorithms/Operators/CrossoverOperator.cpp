#include "CrossoverOperator.h"

CCrossoverOperator::CCrossoverOperator(const IProblem* problem, int seedValue)
	: m_Problem(problem)
	, m_Random(seedValue)
{
}

void CCrossoverOperator::Crossover(const std::vector<TGene>& leftParentGenes, const std::vector<TGene>& rightParentGenes, std::vector<TGene>& leftChildGenes, std::vector<TGene>& rightChildGenes)
{
	// Empty crossover, do nothing
}

