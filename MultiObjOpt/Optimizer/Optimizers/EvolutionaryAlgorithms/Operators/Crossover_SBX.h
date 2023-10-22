#pragma once
#include "CrossoverOperator.h"

class CCrossover_SBX : public CCrossoverOperator
{
public:

	CCrossover_SBX(const IProblem* problem, int seedValue, float cxProb, float etaCoef);

	void Crossover(const std::vector<TGene>& leftParentGenes, const std::vector<TGene>& rightParentGenes,
		std::vector<TGene>& leftChildGenes, std::vector<TGene>& rightChildGenes) override;

private:

	float CalcBeta(const float& u, const float& b, const float& a) const;

	float m_CxProb;
	float m_EtaCoef;
	float m_PowVal;
};