#pragma once

/*
* Program entry params
*/
struct SProgramParams
{
	// Path to the optimizer specific configuration
	char* m_OptimizerConfigPath;

	// Problem to solve codename
	char* m_ProblemName;

	// Path to the problem instance definition
	char* m_ProblemDefinitionPath;

	// Path to write the output files (must exists)
	char* m_OutputDirPath;

	// Number of executions (runs) per instance
	int m_ExecutionsCount;

	// Should run program in silent mode
	bool m_Silent;
};