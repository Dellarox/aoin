#pragma once
#include "CrossoverOperator.h"

/**
 * @brief TTP specialized crossover (Ordered Crossover of the route + Single-point Crossover of the knapsack)
*/
class CCrossover_TTP_OX_SX : public CCrossoverOperator
{
public:

    CCrossover_TTP_OX_SX(const IProblem* problem, int seedValue, float routeCrProb, float knapCrProb);

    void Crossover(const std::vector<TGene>& leftParentGenes, const std::vector<TGene>& rightParentGenes,
                   std::vector<TGene>& leftChildGenes, std::vector<TGene>& rightChildGenes) override;

private:

    float m_RouteCrProb;
    float m_KnapCrProb;
};