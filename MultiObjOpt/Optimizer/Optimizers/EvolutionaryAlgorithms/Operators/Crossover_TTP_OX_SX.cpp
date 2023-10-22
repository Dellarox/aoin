#include "Crossover_TTP_OX_SX.h"

CCrossover_TTP_OX_SX::CCrossover_TTP_OX_SX(const IProblem* problem, int seedValue, float routeCrProb, float knapCrProb)
    : CCrossoverOperator(problem, seedValue)
    , m_RouteCrProb(routeCrProb)
    , m_KnapCrProb(knapCrProb)
{
}

void CCrossover_TTP_OX_SX::Crossover(const std::vector<TGene>& leftParentGenes, const std::vector<TGene>& rightParentGenes, std::vector<TGene>& leftChildGenes, std::vector<TGene>& rightChildGenes)
{
    // TODO - it might be easier to split genotype into sections, so we don't have to save the sectionStartIndex and use the offset

    size_t sectionStartIndex = 0;
    for (const SEncodingSection& encoding : m_Problem->GetProblemEncoding().m_Encoding)
    {
        const size_t sectionSize = encoding.m_SectionDescription.size();
        if (encoding.m_SectionType == EEncodingType::PERMUTATION)
        {
            // Route Crossover
            if (m_Random.NextFloat() < m_RouteCrProb)
            {
                size_t a = m_Random.NextIndex(sectionSize - 1);
                size_t b = m_Random.NextIndex(sectionSize - a) + a;

                const auto& firstParentA = leftParentGenes.begin() + sectionStartIndex + a;
                const auto& firstParentB = leftParentGenes.begin() + sectionStartIndex + b;
                const auto& secParentA = rightParentGenes.begin() + sectionStartIndex + a;
                const auto& secParentB = rightParentGenes.begin() + sectionStartIndex + b;

                size_t firstSeekIdx = b;
                size_t secSeekIdx = b;

                for (size_t i = b; i != a; i = (i + 1) % sectionSize)
                {
                    // Update first child
                    while (std::find(firstParentA, firstParentB, rightParentGenes[sectionStartIndex + secSeekIdx]) != firstParentB)
                    {
                        secSeekIdx = (secSeekIdx + 1) % sectionSize;
                    }
                    leftChildGenes[sectionStartIndex + i] = rightParentGenes[sectionStartIndex + secSeekIdx];
                    secSeekIdx = (secSeekIdx + 1) % sectionSize;


                    // Update second child
                    while (std::find(secParentA, secParentB, leftParentGenes[sectionStartIndex + firstSeekIdx]) != secParentB)
                    {
                        firstSeekIdx = (firstSeekIdx + 1) % sectionSize;
                    }
                    rightChildGenes[sectionStartIndex + i] = leftParentGenes[sectionStartIndex + firstSeekIdx];
                    firstSeekIdx = (firstSeekIdx + 1) % sectionSize;
                }
            }
        }
        else if (encoding.m_SectionType == EEncodingType::BINARY)
        {
            // Knapsack Crossover
            if (m_Random.NextFloat() < m_KnapCrProb)
            {
                size_t point = sectionStartIndex + m_Random.NextIndex(sectionSize);

                for (size_t g = sectionStartIndex; g < sectionStartIndex + sectionSize; ++g)
                {
                    if (g < point)
                    {
                        leftChildGenes[g] = leftParentGenes[g];
                        rightChildGenes[g] = rightParentGenes[g];
                    }
                    else
                    {
                        leftChildGenes[g] = rightParentGenes[g];
                        rightChildGenes[g] = leftParentGenes[g];
                    }
                }
            }
        }
        sectionStartIndex += sectionSize;
    }
}