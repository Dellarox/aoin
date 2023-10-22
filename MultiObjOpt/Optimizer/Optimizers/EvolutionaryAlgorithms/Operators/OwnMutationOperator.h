//
// Created by Kajetan on 22.10.2023.
//

#ifndef METAHEURISTICSREPO_OWNMUTATIONOPERATOR_H
#define METAHEURISTICSREPO_OWNMUTATIONOPERATOR_H


#include <vector>
#include "Optimizers/EvolutionaryAlgorithms/Individual/Individual.h"
#include "Utils/Random.h"

class IProblem;

class COwnMutationOperator
{
public:

    COwnMutationOperator(const IProblem* problem, int seedValue);

    virtual void Mutate(std::vector<TGene> &genes, unsigned long long index1, unsigned long long index2, unsigned long long shiftSize);

    void swapMutate(std::vector<TGene>& childGenes, unsigned long long index1, unsigned long long index2, unsigned long long shiftSize);

    void inversionMutate(std::vector<TGene>& childGenes, unsigned long long index1, unsigned long long index2, unsigned long long shiftSize);

protected:

    const IProblem* m_Problem = nullptr;
    CRandom m_Random;
};


#endif //METAHEURISTICSREPO_OWNMUTATIONOPERATOR_H
