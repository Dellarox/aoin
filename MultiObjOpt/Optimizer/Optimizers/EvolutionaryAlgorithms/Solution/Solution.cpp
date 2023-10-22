//
// Created by Kajetan on 20.10.2023.
//

#include "Solution.h"

void CSolution::SetSolution(const std::vector<TGene>& genes)
{
    m_Genotype = genes;
}

void CSolution::Evaluate(const IProblem* problem)
{
    problem->Evaluate(m_Genotype, m_EvalValues);
}
