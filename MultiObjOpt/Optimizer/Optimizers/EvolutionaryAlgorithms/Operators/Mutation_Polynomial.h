#pragma once
#include "MutationOperator.h"

class CMutation_Polynomial : public CMutationOperator
{
public:

	CMutation_Polynomial(const IProblem* problem, int seedValue, float mutProb, float etaCoef);

	void Mutate(std::vector<TGene>& genes);

private:
	float m_MutProb;
	float m_EtaCoef;
};