//
// Created by Kajetan on 22.10.2023.
//
#pragma once
#ifndef METAHEURISTICSREPO_MUTATION_SWAP_H
#define METAHEURISTICSREPO_MUTATION_SWAP_H


#include <vector>
#include "MutationOperator.h"

class CMutation_Swap : public CMutationOperator{
public:
    CMutation_Swap(const IProblem* problem, int seedValue, float mutProb);

    void Mutate(std::vector<TGene> &genes) override;

private:
    float m_MutProb;
};


#endif //METAHEURISTICSREPO_MUTATION_SWAP_H
