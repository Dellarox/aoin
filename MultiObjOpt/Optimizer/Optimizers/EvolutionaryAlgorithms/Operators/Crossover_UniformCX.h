#pragma once
#include "CrossoverOperator.h"

class CCrossover_UniformCX : public CCrossoverOperator
{
public:

	CCrossover_UniformCX(const IProblem* problem, int seedValue, float cxProb);

	void Crossover(const std::vector<TGene>& leftParentGenes, const std::vector<TGene>& rightParentGenes,
		std::vector<TGene>& leftChildGenes, std::vector<TGene>& rightChildGenes) override;

private:

	float m_CxProb;
};