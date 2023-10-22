#pragma once

#include "../Problem.h"
#include "TTP.h"

class CProblemTTP_CityOrderKnapBin_MO : public IProblem
{
public:

	bool ReadDefinition(const char* filePath) override;
	bool Evaluate(const TSolution& solution, TEvaluation& evaluation) const override;
	size_t GetObjCount() const override;
	const SProblemEncoding& GetProblemEncoding() const override;

protected:

    std::vector<std::vector<size_t>> m_CityItems;
    std::vector<size_t> m_UpperBounds;
    SProblemEncoding m_ProblemEncoding;
	CTTP m_TTPTemplate;
private:
    void PrepareProblemEncoding();
};