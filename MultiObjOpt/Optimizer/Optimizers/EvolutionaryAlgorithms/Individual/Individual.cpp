#include <cstddef>
#include "Individual.h"

void CIndividual::SetGenes(const std::vector<TGene>& genes)
{
	m_Genotype = genes;
}

void CIndividual::SetGene(size_t geneIdx, TGene geneValue)
{
	if (geneIdx < m_Genotype.size())
	{
		m_Genotype[geneIdx] = geneValue;
	}
}

void CIndividual::SwapGenes(size_t leftGeneIdx, size_t rightGeneIdx)
{
	TGene tempGene = m_Genotype[leftGeneIdx];
	m_Genotype[leftGeneIdx] = m_Genotype[rightGeneIdx];
	m_Genotype[rightGeneIdx] = tempGene;
}

void CIndividual::Evaluate(const IProblem* problem)
{
	problem->Evaluate(m_Genotype, m_EvalValues);
}

bool CIndividual::IsDominatedBy(const CIndividual* otherIndividual) const
{
	// If this has any better value
	for (int i = 0; i < m_EvalValues.size(); ++i)
	{
		if (m_EvalValues[i] < otherIndividual->m_EvalValues[i])
		{
			return false;
		}
	}

	// Now we are sure we have worse or equal values

	// If other has at least one better value
	for (int i = 0; i < m_EvalValues.size(); ++i)
	{
		if (otherIndividual->m_EvalValues[i] < m_EvalValues[i])
		{
			return true;
		}
	}

	// If equal
	return false;
}

bool CIndividual::IsDuplicateGenotype(const CIndividual* otherIndividual) const
{
	// First check by evaluate values which is much faster
	for (int i = 0; i < m_EvalValues.size(); ++i)
	{
		if (otherIndividual->m_EvalValues[i] != m_EvalValues[i])
		{
			return false;
		}
	}

	// If evaluation values are the same, make sure it's not the same genotype
	return otherIndividual->GetGenes() == m_Genotype;
}

bool CIndividual::IsDuplicateEvalValue(const CIndividual* otherIndividual) const
{
	for (int i = 0; i < m_EvalValues.size(); ++i)
	{
		if (otherIndividual->m_EvalValues[i] != m_EvalValues[i])
		{
			return false;
		}
	}

	return true;
}
