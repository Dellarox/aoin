#include "Mutation_Polynomial.h"
#include "Problems/Problem.h"

CMutation_Polynomial::CMutation_Polynomial(const IProblem* problem, int seedValue, float mutProb, float etaCoef)
	: CMutationOperator(problem, seedValue)
	, m_MutProb(mutProb)
	, m_EtaCoef(etaCoef)
{
}

void CMutation_Polynomial::Mutate(std::vector<TGene>& genes)
{
	size_t sectionStartIndex = 0;
	for (const SEncodingSection& encoding : m_Problem->GetProblemEncoding().m_Encoding)
	{
		const size_t sectionSize = encoding.m_SectionDescription.size();
		for (size_t inSecIdx = 0; inSecIdx < sectionSize; ++inSecIdx)
		{
			if (m_Random.NextFloat() < m_MutProb)
			{
				const SEncodingDescriptor& descriptor = encoding.m_SectionDescription[inSecIdx];
				float lb = descriptor.m_MinValue;
				float ub = descriptor.m_MaxValue;
				float boundSpan = ub - lb;
				size_t inGenIdx = sectionStartIndex + inSecIdx;

				float y = genes[inGenIdx];
				float mutPow = 1.f / (m_EtaCoef + 1.f);
				float randU = m_Random.NextFloat();

				float deltaq;
				if (randU < 0.5f)
				{
					float delta1 = (y - lb) / boundSpan;
					float val = 2.f * randU + (1.f - 2.f * randU) * (std::pow(1.f - delta1, (m_EtaCoef + 1.f)));
					deltaq = std::pow(val, mutPow) - 1.f;
				}
				else 
				{
					float delta2 = (ub - y) / boundSpan;
					float val = 2.f * (1.f - randU) + 2.f * (randU - 0.5f) * (std::pow(1.f - delta2, (m_EtaCoef + 1.f)));
					deltaq = 1.f - (std::pow(val, mutPow));
				}

				y = y + deltaq * boundSpan;
				if (y < lb) y = lb;
				if (y > ub) y = ub;
				genes[inGenIdx] = y;
			}
		}
		sectionStartIndex += sectionSize;
	}
}
