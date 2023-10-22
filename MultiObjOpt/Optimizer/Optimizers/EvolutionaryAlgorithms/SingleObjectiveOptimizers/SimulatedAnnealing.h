#pragma once

#include "Optimizers/EvolutionaryAlgorithms/Operators/MutationOperator.h"
#include "Optimizers/EvolutionaryAlgorithms/Operators/CrossoverOperator.h"
#include "Optimizers/EvolutionaryAlgorithms/Operators/Operators.h"
#include "Optimizers/Optimizer.h"

class CSimulatedAnnealing : public IOptimizer {
public:
    CSimulatedAnnealing(const SOptimizerParams &optParams, const IProblem *optProblem) :
            m_Random(optParams.m_SeedValue),
            m_ProblemDefinition(optProblem),
            m_Operators(&m_Random),
            m_Params(optParams, optProblem) {};

    SOptimizationResult RunOptimization() override;

private:
    void Anneal();

    bool StopCondition() const;
    const IProblem *m_ProblemDefinition;
    int m_Generation = 0;
    CRandom m_Random;
    COperators m_Operators;
    std::vector<CIndividual *> m_Archive;
    std::vector<CIndividual *> m_Population;
    SOptimizationResult m_optResult;
    double m_Temperature;
    int m_GenerationLimit;

    struct SSAParams : SOptimizerParams {
        SSAParams(const SOptimizerParams &optParams, const IProblem *optProblem);

        int m_TemperatureMax = 0;
        double m_TemperatureMin = 0;
        double m_TemperatureReducing = 0;
        size_t m_PopulationSize = 0;
    };

    SSAParams m_Params;
    void createIndividual();
    std::vector<TGene> generateGenes();
    void saveIterationInfoToResult();

    CIndividual *CreateNeighbour(CIndividual *individual);

    std::vector<TGene> createNewGenes(CIndividual *individual);

    void ReduceTemperature();
};