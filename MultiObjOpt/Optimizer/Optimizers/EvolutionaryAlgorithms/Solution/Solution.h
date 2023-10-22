//
// Created by Kajetan on 20.10.2023.
//
#pragma once
#ifndef METAHEURISTICSREPO_SOLUTION_H
#define METAHEURISTICSREPO_SOLUTION_H

#include <vector>
#include "Problems/Problem.h"

using TGene = float;

class CSolution {
public:
    void SetSolution(const std::vector<TGene> &genes);
    void Evaluate(const IProblem *problem);

// make getter for m_EvalValues
    TEvaluationValue GetEvalValue(size_t objectiveIdx) const { return m_EvalValues[objectiveIdx]; }

private:
    std::vector<float> m_Genotype;
    TEvaluation m_EvalValues;
};


#endif //METAHEURISTICSREPO_SOLUTION_H
