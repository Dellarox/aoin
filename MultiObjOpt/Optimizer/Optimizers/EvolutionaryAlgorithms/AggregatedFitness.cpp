//
// Created by Konrad Gmyrek on 27.09.2023.
//

#include "AggregatedFitness.h"

float CAggregatedFitness::ComputeAggregatedFitness(CIndividual* individual, std::vector<float> objectiveWeights)
{
    // If weights not provided assume that this is TTP_SO
    if (objectiveWeights.empty())
    {
        return individual->GetEvaluation()[0];
    }

    const TEvaluation &evaluation = individual->GetEvaluation();
    float fitness = 0.0;

    for (int i = 0; i < objectiveWeights.size(); ++i) {
        fitness += evaluation[i] * objectiveWeights[i];
    }

    return fitness;
}

float CAggregatedFitness::ComputeAggregatedFitnessSA(CIndividual *individual) {
    return individual->GetEvaluation()[0];
}
