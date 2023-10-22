//
// Created by Kajetan on 22.10.2023.
//
#pragma once
#ifndef METAHEURISTICSREPO_MUTATION_INVERSE_H
#define METAHEURISTICSREPO_MUTATION_INVERSE_H


#include <vector>
#include "MutationOperator.h"

class CMutation_Inverse : public CMutationOperator {
public:
    CMutation_Inverse(const IProblem *problem, int seedValue, float mutProb);

    void Mutate(std::vector<TGene> &genes) override;
private:
    float m_MutProb;
};


#endif //METAHEURISTICSREPO_MUTATION_INVERSE_H
