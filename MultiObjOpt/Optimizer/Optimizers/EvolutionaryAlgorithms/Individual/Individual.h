#pragma once
#include <vector>
#include "Problems/Problem.h"

using TGene = float;

class CIndividual
{
public:

	const std::vector<TGene>& GetGenes() const { return m_Genotype; }
	void SetGenes(const std::vector<TGene>& genes);
	void SetGene(size_t geneIdx, TGene geneValue);
	void SwapGenes(size_t leftGeneIdx, size_t rightGeneIdx);

	void Evaluate(const IProblem* problem);
	const TEvaluation& GetEvaluation() const { return m_EvalValues; }
	size_t GetEvaluationSize() const { return m_EvalValues.size(); }
	TEvaluationValue GetEvalValue(size_t objectiveIdx) const { return m_EvalValues[objectiveIdx]; }

	bool IsDominatedBy(const CIndividual* otherIndividual) const;
	bool IsDuplicateGenotype(const CIndividual* otherIndividual) const;
	bool IsDuplicateEvalValue(const CIndividual* otherIndividual) const;

	void SetRank(size_t rank) { m_Rank = rank; }
	size_t GetRank() const { return m_Rank; }

	void SetCrowdingDist(float crowdDist) { m_CrowdingDistance = crowdDist; }
	float GetCrowdingDist() const { return m_CrowdingDistance; }

	size_t GetSelected() const { return m_Selected; }
	void OnSelected() { m_Selected += 1; }

private:

	std::vector<TGene> m_Genotype;
	TEvaluation m_EvalValues;

	size_t m_Rank = 0;
	float m_CrowdingDistance = 0.f;
	size_t m_Selected = 0;
};