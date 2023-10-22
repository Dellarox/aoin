#pragma once
#include <vector>
#include "ProblemEncoding.h"
#include <cstddef>

using TEvaluationValue = float;
using TEvaluation = std::vector<TEvaluationValue>;

/*
* Problem interface class
*/
class IProblem
{
public:
	virtual bool ReadDefinition(const char* filePath) = 0;
	virtual bool Evaluate(const TSolution& solution, TEvaluation& evaluation) const = 0;
	virtual size_t GetObjCount() const = 0;
	virtual const SProblemEncoding& GetProblemEncoding() const = 0;
};