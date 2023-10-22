//
// Created by Kajetan on 20.10.2023.
//

#include "RandomAlgorithm.h"
#include "Optimizers/EvolutionaryAlgorithms/Solution/Solution.h"


SOptimizationResult CRandomAlgorithm::RunOptimization() {
    m_Solution = createSolution();
    m_Archive.push_back(m_Solution);
    saveEvaluationToResult();
    return m_optResult;
}

CSolution CRandomAlgorithm::createSolution() {
    auto *newSolution = new CSolution();
    newSolution->SetSolution(generateSolution());
    newSolution->Evaluate(m_ProblemDefinition);
    return *newSolution;
}

std::vector<float> CRandomAlgorithm::generateSolution()
{
    std::vector<float> solution;
    m_Operators.RandomInit(solution, m_ProblemDefinition->GetProblemEncoding());
    return solution;
}

void CRandomAlgorithm::saveEvaluationToResult() {
    TEvaluation iterationInfo;
    iterationInfo.reserve(4);
    iterationInfo.emplace_back(m_Generation);
    iterationInfo.emplace_back(m_Solution.GetEvalValue(0));
    iterationInfo.emplace_back(m_Solution.GetEvalValue(0));
    iterationInfo.emplace_back(m_Solution.GetEvalValue(0));
    m_optResult.m_ParetoFront.emplace_back(std::move(iterationInfo));
}
