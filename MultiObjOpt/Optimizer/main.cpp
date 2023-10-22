#include <iostream>
#include <sys/stat.h>
#include <cstdlib>
#include "Program/Program.h"
#include "Program/ProgramParams.h"

const int MIN_REQUIRED_ARGS = 5;
const int OPTIMIZER_CONFIG_INDEX = 1;
const int PROBLEM_NAME_INDEX = 2;
const int PROBLEM_DEFINITION_INDEX = 3;
const int OUTPUT_DIR_INDEX = 4;
const int EXECUTION_COUNT_INDEX = 5;
const int SILENT_INDEX = 6;

bool fileExists(const char *path)
{
    struct stat buffer{};
    return (stat(path, &buffer) == 0);
}

void showErrorAndExit(const char *message, const char *detail = "")
{
    std::cerr << "Error: " << message << detail << std::endl;
    exit(1);
}

bool parseArguments(int argc, char *argv[], SProgramParams &params)
{
    if (argc < MIN_REQUIRED_ARGS)
    {
        std::cerr << "Usage: " << argv[0]
                  << " <OptimizerConfigPath> <ProblemName> <ProblemDefinitionPath> <OutputDirPath> [ExecutionsCount] [Silent]"
                  << std::endl;
        return false;
    }

    if (!fileExists(argv[OPTIMIZER_CONFIG_INDEX]))
    {
        showErrorAndExit("The provided OptimizerConfigPath does not exist: ", argv[OPTIMIZER_CONFIG_INDEX]);
    }
    params.m_OptimizerConfigPath = argv[OPTIMIZER_CONFIG_INDEX];

    params.m_ProblemName = argv[PROBLEM_NAME_INDEX];

    if (!fileExists(argv[PROBLEM_DEFINITION_INDEX]))
    {
        showErrorAndExit("The provided ProblemDefinitionPath does not exist: ", argv[PROBLEM_DEFINITION_INDEX]);
    }
    params.m_ProblemDefinitionPath = argv[PROBLEM_DEFINITION_INDEX];

    if (!fileExists(argv[OUTPUT_DIR_INDEX]))
    {
        showErrorAndExit("The provided OutputDirPath does not exist: ", argv[OUTPUT_DIR_INDEX]);
    }
    params.m_OutputDirPath = argv[OUTPUT_DIR_INDEX];

    params.m_ExecutionsCount = (argc > EXECUTION_COUNT_INDEX) ? atoi(argv[EXECUTION_COUNT_INDEX]) : 1;
    params.m_Silent = (argc > SILENT_INDEX) && (bool) atoi(argv[SILENT_INDEX]);

    return true;
}

int main(int argc, char *argv[])
{
    SProgramParams programParams{};

    if (!parseArguments(argc, argv, programParams))
    {
        return -1;
    }

    try
    {
        CProgram program;
        program.Run(programParams);
    }
    catch (const std::exception &e)
    {
        showErrorAndExit("An error occurred while executing the program: ", e.what());
    }

    return 0;
}
