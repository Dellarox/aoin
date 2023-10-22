#include "MutationOperator.h"

CMutationOperator::CMutationOperator(const IProblem* problem, int seedValue)
	: m_Problem(problem)
	, m_Random(seedValue)
{
}

void CMutationOperator::Mutate(std::vector<TGene> &genes)
{
	// Empty mutation, do nothing
}


