//
// Created by Kajetan on 22.10.2023.
//

#include "Mutation_Inverse.h"

CMutation_Inverse::CMutation_Inverse(const IProblem *problem, int seedValue, float mutProb)
        : CMutationOperator(problem, seedValue), m_MutProb(mutProb) {
}

void CMutation_Inverse::Mutate(std::vector<TGene> &genes) {
    if (m_Random.NextFloat() < m_MutProb) {
        unsigned long long index1 = 0;
        unsigned long long index2 = 0;
        unsigned long long shiftSize = m_Problem->GetProblemEncoding().m_Encoding[1].m_SectionDescription.size();
        while(index1 == index2){
            index1 = rand() % shiftSize;
            index2 = rand() % shiftSize;
        }

        if (index1 > index2) {
            std::swap(index1, index2);
        }

        std::swap(genes[index1], genes[index2]);
        std::swap(genes[index1 + shiftSize], genes[index2 + shiftSize]);

        index1++;
        index2--;

        while (index1 <= index2) {
            std::swap(genes[index1], genes[index2]);
            std::swap(genes[index1 + shiftSize], genes[index2 + shiftSize]);
            index1++;
            index2--;
        }
    }
}
