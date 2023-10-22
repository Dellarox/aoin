#include "Mutation_TTP_Reverse_Flip.h"

CMutation_TTP_Reverse_Flip::CMutation_TTP_Reverse_Flip(const IProblem* problem, int seedValue, float reverseMutProb, float flipMutProb)
    : CMutationOperator(problem, seedValue)
    , m_ReverseMutProb(reverseMutProb)
    , m_FlipMutProb(flipMutProb)
{
}

void CMutation_TTP_Reverse_Flip::Mutate(std::vector<TGene>& genes)
{
    size_t sectionStartIndex = 0;
    for (const SEncodingSection& encoding : m_Problem->GetProblemEncoding().m_Encoding)
    {
        const size_t sectionSize = encoding.m_SectionDescription.size();
        if (encoding.m_SectionType == EEncodingType::PERMUTATION)
        {
            // Route Mutation
            if (m_Random.NextFloat() < m_ReverseMutProb)
            {
                // Use inverse mutation for cities
                size_t firstGene = sectionStartIndex + m_Random.NextIndex(sectionSize);
                size_t secondGene = sectionStartIndex + m_Random.NextIndex(sectionSize);

                if (firstGene < secondGene)
                {
                    std::reverse(genes.begin() + firstGene, genes.begin() + secondGene + 1);
                }
                else if (secondGene < firstGene)
                {
                    std::reverse(genes.begin() + secondGene, genes.begin() + firstGene + 1);
                }
            }
        }
        else if (encoding.m_SectionType == EEncodingType::BINARY)
        {
            // Knapsack Mutation
            if (m_Random.NextFloat() < m_FlipMutProb)
            {
                size_t randItemIdx = sectionStartIndex + m_Random.NextIndex(sectionSize);
                genes[randItemIdx] = !genes[randItemIdx];
            }
        }
        sectionStartIndex += sectionSize;
    }
}