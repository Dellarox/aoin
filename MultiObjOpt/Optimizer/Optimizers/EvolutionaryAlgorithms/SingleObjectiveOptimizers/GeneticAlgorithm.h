#pragma once

#include "Optimizers/EvolutionaryAlgorithms/Operators/MutationOperator.h"
#include "Optimizers/EvolutionaryAlgorithms/Operators/CrossoverOperator.h"
#include "Optimizers/EvolutionaryAlgorithms/Operators/Operators.h"
#include "Optimizers/Optimizer.h"

class CGeneticAlgorithm : public IOptimizer {
public:
    CGeneticAlgorithm(const SOptimizerParams &optParams, const IProblem *optProblem) :
        m_Random(optParams.m_SeedValue),
        m_ProblemDefinition(optProblem),
        m_Operators(&m_Random),
        m_Params(optParams, optProblem) {};
    ~CGeneticAlgorithm();

    SOptimizationResult RunOptimization() override;

private:
    void Reset();
    void Evolve();

    bool StopCondition() const;
    const IProblem *m_ProblemDefinition;
    int m_Generation = 0;
    CRandom m_Random;
    COperators m_Operators;
    std::vector<CIndividual *> m_Archive;
    std::vector<CIndividual *> m_Population;
    SOptimizationResult m_optResult;

    struct SGAParams : SOptimizerParams {
        SGAParams(const SOptimizerParams &optParams, const IProblem *optProblem);
        ~SGAParams();

        int m_TourSize = 0;
        size_t m_PopulationSize = 0;
        size_t m_GenerationLimit = 0;
        CMutationOperator *m_Mutation;
        CCrossoverOperator *m_Crossover;

        std::vector<float> m_ObjectiveWeights;
        void readWeights(const std::string& weights);
    };

    SGAParams m_Params;
    void createIndividual();
    std::vector<TGene> generateGenes();
    void saveIterationInfoToResult();

    void prepareForSwapAndInversionMutation(unsigned long long int &index1, unsigned long long int& index2, unsigned long long int& shiftSize);
};