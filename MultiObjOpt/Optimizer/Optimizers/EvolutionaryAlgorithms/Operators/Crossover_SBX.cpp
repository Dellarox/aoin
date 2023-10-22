#include <cfloat>
#include "Crossover_SBX.h"

CCrossover_SBX::CCrossover_SBX(const IProblem* problem, int seedValue, float cxProb, float etaCoef)
	: CCrossoverOperator(problem, seedValue)
	, m_CxProb(cxProb)
	, m_EtaCoef(etaCoef)
	, m_PowVal(1.f / (etaCoef + 1.f))
{
}

void CCrossover_SBX::Crossover(const std::vector<TGene>& leftParentGenes, const std::vector<TGene>& rightParentGenes, std::vector<TGene>& leftChildGenes, std::vector<TGene>& rightChildGenes)
{
	size_t sectionStartIndex = 0;
	for (const SEncodingSection& encoding : m_Problem->GetProblemEncoding().m_Encoding)
	{
		const size_t sectionSize = encoding.m_SectionDescription.size();
		if (m_Random.NextFloat() < m_CxProb)
		{
			for (size_t inSecIdx = 0; inSecIdx < sectionSize; ++inSecIdx)
			{
				size_t inGenIdx = sectionStartIndex + inSecIdx;
				float y1 = leftParentGenes[inGenIdx];
				float y2 = rightParentGenes[inGenIdx];
				if (std::abs(y1 - y2) > FLT_EPSILON)
				{
					// Further calculations require that y1 <= y2
					if (y2 < y1)
					{
						y2 = y1;
						y1 = rightParentGenes[inGenIdx];
					}

					const SEncodingDescriptor& descriptor = encoding.m_SectionDescription[inSecIdx];

					float randU = m_Random.NextFloat();
					float b = 1.f + (y1 - descriptor.m_MinValue) / (y2 - y1);
					float beta = CalcBeta(randU, b, descriptor.m_MinValue);
					leftChildGenes[inGenIdx] = 0.5f * ((y1 + y2) - beta * (y2 - y1));

					b = 1.f + (descriptor.m_MaxValue - y2) / (y2 - y1);
					beta = CalcBeta(randU, b, descriptor.m_MaxValue);
					rightChildGenes[inGenIdx] = 0.5f * ((y1 + y2) + beta * (y2 - y1));
				}
			}
		}
		sectionStartIndex += sectionSize;
	}
}

float CCrossover_SBX::CalcBeta(const float& u, const float& b, const float& a) const
{
	float gamma = 1.f / (2 * powf(b, m_EtaCoef + 1));
	float gammaInv = 1.f - gamma;
	if (u <= (0.5f / gammaInv))
	{
		return powf(2 * u * gammaInv, m_PowVal);
	}
	else
	{
		return powf(1.f / (2.f * (1.f - u * gammaInv)), m_PowVal);
	}
}

