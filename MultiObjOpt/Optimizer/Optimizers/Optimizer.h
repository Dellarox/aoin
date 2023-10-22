#pragma once
#include "OptimizerParams.h"
#include "OptimizationResult.h"

class IProblem;

/*
* Optimizer interface class
*/
class IOptimizer
{
public:
	virtual SOptimizationResult RunOptimization() = 0;
};