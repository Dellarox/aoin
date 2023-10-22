#pragma once
#include "MutationOperator.h"

class CMutation_TTP_Reverse_Flip : public CMutationOperator
{
public:

    CMutation_TTP_Reverse_Flip(const IProblem* problem, int seedValue, float reverseMutProb, float flipMutProb);

    void Mutate(std::vector<TGene>& genes);

private:
    float m_ReverseMutProb;
    float m_FlipMutProb;
}; 