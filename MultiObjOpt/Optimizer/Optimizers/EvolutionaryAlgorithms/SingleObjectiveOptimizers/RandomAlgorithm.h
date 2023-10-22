//
// Created by Kajetan on 20.10.2023.
//

#ifndef METAHEURISTICSREPO_RANDOMALGORITHM_H
#define METAHEURISTICSREPO_RANDOMALGORITHM_H


#include "Optimizers/Optimizer.h"
#include "Utils/Random.h"
#include "Optimizers/EvolutionaryAlgorithms/Operators/Operators.h"
#include "Optimizers/EvolutionaryAlgorithms/Solution/Solution.h"

class CRandomAlgorithm : public IOptimizer {
public:
    CRandomAlgorithm(const SOptimizerParams &optParams, const IProblem *optProblem):
        m_ProblemDefinition(optProblem),
        m_Random(optParams.m_SeedValue),
        m_Operators(&m_Random) {};
    SOptimizationResult RunOptimization() ;
private:
    COperators m_Operators;
    const IProblem *m_ProblemDefinition;
    int m_Generation = 0;
    CRandom m_Random;
    CSolution m_Solution;
    std::vector<CSolution> m_Archive;
    SOptimizationResult m_optResult;

    CSolution createSolution();
    std::vector<float> generateSolution();
    void saveEvaluationToResult();
};


#endif //METAHEURISTICSREPO_RANDOMALGORITHM_H
