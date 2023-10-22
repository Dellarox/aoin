#include "Mutation_RandomBit.h"
#include "Problems/Problem.h"

CMutation_RandomBit::CMutation_RandomBit(const IProblem* problem, int seedValue, float mutProb)
	: CMutationOperator(problem, seedValue)
	, m_MutProb(mutProb)
{
}

void CMutation_RandomBit::Mutate(std::vector<TGene>& genes)
{
	size_t sectionStartIndex = 0;
	for (const SEncodingSection& encoding : m_Problem->GetProblemEncoding().m_Encoding)
	{
		const size_t sectionSize = encoding.m_SectionDescription.size();
		for (size_t g = sectionStartIndex; g < sectionStartIndex + sectionSize; ++g)
		{
			if (m_Random.NextFloat() < m_MutProb)
			{
				// TODO - section[g] is probably wrong
				const TSolutionValue& minVal = encoding.m_SectionDescription[g].m_MinValue;
				genes[g] = minVal + m_Random.NextFloat(encoding.m_SectionDescription[g].m_MaxValue - minVal);
			}
		}
		sectionStartIndex += sectionSize;
	}
}
