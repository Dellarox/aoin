#pragma once

#include "../Problem.h"
#include "DTLZParams.h"
// Implementation based on https://github.com/esa/pagmo2

class CProblemDTLZ : public IProblem
{
public:

	bool ReadDefinition(const char* filePath) override;
	bool Evaluate(const TSolution& solution, TEvaluation& evaluation) const override;
	size_t GetObjCount() const override;
	const SProblemEncoding& GetProblemEncoding() const override;

private:

	SProblemEncoding m_ProblemEncoding;
    SDTLZParams m_InstanceParams;

	// Convergence metric for a dv (0 = converged to the optimal front)
    float g_func(const std::vector<float>&) const;

    // Implementations of the different g-functions used
    float g13_func(const std::vector<float>&) const;
    float g245_func(const std::vector<float>&) const;
    float g6_func(const std::vector<float>&) const;
    float g7_func(const std::vector<float>&) const;

    // Implementation of the distribution function h
    float h7_func(const std::vector<float>&, float) const;

    // Implementation of the objective functions
    std::vector<float> f1_fitness(const std::vector<float>&) const;
    std::vector<float> f23_fitness(const std::vector<float>&) const;
    std::vector<float> f4_fitness(const std::vector<float>&) const;
    std::vector<float> f56_fitness(const std::vector<float>&) const;
    std::vector<float> f7_fitness(const std::vector<float>&) const;

};