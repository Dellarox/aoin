#include "Crossover_UniformCX.h"

CCrossover_UniformCX::CCrossover_UniformCX(const IProblem* problem, int seedValue, float cxProb)
	: CCrossoverOperator(problem, seedValue)
	, m_CxProb(cxProb)
{
}

void CCrossover_UniformCX::Crossover(const std::vector<TGene>& leftParentGenes, const std::vector<TGene>& rightParentGenes, std::vector<TGene>& leftChildGenes, std::vector<TGene>& rightChildGenes)
{
	size_t sectionStartIndex = 0;
	for (const SEncodingSection& encoding : m_Problem->GetProblemEncoding().m_Encoding)
	{
		const size_t sectionSize = encoding.m_SectionDescription.size();
		if (m_Random.NextFloat() < m_CxProb)
		{
			for (size_t g = sectionStartIndex; g < sectionStartIndex + sectionSize; ++g)
			{
				if (m_Random.NextFloat() < 0.5f)
				{
					leftChildGenes[g] = rightParentGenes[g];
				}
				// Check explicitly both sides ??
				if (m_Random.NextFloat() < 0.5f)
				{
					rightChildGenes[g] = leftParentGenes[g];
				}
			}
		}
		sectionStartIndex += sectionSize;
	}
}

