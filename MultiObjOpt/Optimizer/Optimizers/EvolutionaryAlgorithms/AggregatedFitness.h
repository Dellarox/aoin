//
// Created by Konrad Gmyrek on 27.09.2023.
//

#pragma once

#include "Optimizers/EvolutionaryAlgorithms/Individual/Individual.h"

class CAggregatedFitness {
public:
    static float ComputeAggregatedFitness(CIndividual *individual, std::vector<float> objectiveWeights);
    static float ComputeAggregatedFitnessSA(CIndividual *individual);
};
