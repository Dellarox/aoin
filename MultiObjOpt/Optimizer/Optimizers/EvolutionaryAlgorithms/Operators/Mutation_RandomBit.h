#pragma once
#include "MutationOperator.h"

class CMutation_RandomBit : public CMutationOperator
{
public:

	CMutation_RandomBit(const IProblem* problem, int seedValue, float mutProb);

	void Mutate(std::vector<TGene>& genes);

private:
	float m_MutProb;
};