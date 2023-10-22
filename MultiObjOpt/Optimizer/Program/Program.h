#pragma once
#include "Program/ProgramParams.h"
#include <string>

class IProblem;
class IOptimizer;
struct SOptimizerParams;

class CProgram
{
public:
	static void Run(const SProgramParams& programParams);

private:

	static IProblem* CreateProblemFromName(const char* problemName);
	static IOptimizer* CreateOptimizerFromName(const char* optimizerName, const SOptimizerParams& optParams, const IProblem* problemDefinition);
	static std::string GetFileName(const char* filePath) ;

};