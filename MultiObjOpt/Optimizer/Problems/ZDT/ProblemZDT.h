#pragma once

#include "../Problem.h"
#include "ZDTParams.h"
// Implementation based on https://github.com/esa/pagmo2

class CProblemZDT : public IProblem
{
public:

	bool ReadDefinition(const char* filePath) override;
	bool Evaluate(const TSolution& solution, TEvaluation& evaluation) const override;
	size_t GetObjCount() const override;
	const SProblemEncoding& GetProblemEncoding() const override;

private:

	SProblemEncoding m_ProblemEncoding;
	SZDTParams m_InstanceParams;

	std::vector<SEncodingDescriptor> BuildBounds() const;

	// Implementation of the objective functions
	std::vector<float> f1_fitness(const std::vector<float>&) const;
    std::vector<float> f2_fitness(const std::vector<float>&) const;
    std::vector<float> f3_fitness(const std::vector<float>&) const;
    std::vector<float> f4_fitness(const std::vector<float>&) const;
    std::vector<float> f5_fitness(const std::vector<float>&) const;
    std::vector<float> f6_fitness(const std::vector<float>&) const;
};