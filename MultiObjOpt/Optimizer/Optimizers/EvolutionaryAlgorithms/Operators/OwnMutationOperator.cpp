#include "MutationOperator.h"
#include "OwnMutationOperator.h"


COwnMutationOperator::COwnMutationOperator(const IProblem* problem, int seedValue)
        : m_Problem(problem)
        , m_Random(seedValue)
{
}

void COwnMutationOperator::Mutate(std::vector<TGene> &genes, unsigned long long int index1, unsigned long long int index2,
                               unsigned long long int shiftSize)
{
    // Empty mutation, do nothing
}

void COwnMutationOperator::swapMutate(std::vector<TGene>& childGenes, unsigned long long index1, unsigned long long index2, unsigned long long shiftSize){
    std::swap(childGenes[index1], childGenes[index2]);
    std::swap(childGenes[index1 + shiftSize], childGenes[index2 + shiftSize]);
}

void COwnMutationOperator::inversionMutate(std::vector<TGene>& childGenes, unsigned long long index1, unsigned long long index2, unsigned long long shiftSize) {

}
