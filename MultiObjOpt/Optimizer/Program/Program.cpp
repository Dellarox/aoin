#include "Program.h"
#include "Optimizers/ConfigReader/ConfigReader.h"
#include "Optimizers/EvolutionaryAlgorithms/ResultWriter/ResultsWriter.h"
#include "Problems/MSRCPSP/ProblemMSRCPSP_TaskAssign.h"
#include "Problems/MSRCPSP/ProblemMSRCPSP_TaskOrdering.h"
#include "Problems/TTP/ProblemTTP_CityOrderKnapBin_MO.h"
#include "Problems/TTP/ProblemTTP_CityOrderKnapBin_SO.h"
#include "Problems/DTLZ/ProblemDTLZ.h"
#include "Problems/ZDT/ProblemZDT.h"
#include "Optimizers/EvolutionaryAlgorithms/SingleObjectiveOptimizers/GeneticAlgorithm.h"
#include "Optimizers/EvolutionaryAlgorithms/MultiObjectiveOptimizers/BNTGA.h"
#include "Optimizers/EvolutionaryAlgorithms/MultiObjectiveOptimizers/NTGA2.h"
#include "Optimizers/EvolutionaryAlgorithms/MultiObjectiveOptimizers/NSGAII.h"
#include "Optimizers/EvolutionaryAlgorithms/MultiObjectiveOptimizers/MOEAD.h"
#include "Optimizers/EvolutionaryAlgorithms/MultiObjectiveOptimizers/SPEA2.h"
#include "Optimizers/EvolutionaryAlgorithms/SingleObjectiveOptimizers/RandomAlgorithm.h"
#include "Optimizers/EvolutionaryAlgorithms/SingleObjectiveOptimizers/SimulatedAnnealing.h"
#include "Optimizers/EvolutionaryAlgorithms/ResultWriter/ResultsWriterGA.h"
#include "Optimizers/EvolutionaryAlgorithms/ResultWriter/ResultsWriterSA.h"
#include "Optimizers/EvolutionaryAlgorithms/ResultWriter/ResultsWriterRandom.h"
#include <string>
#include <iostream>
#include <cstring>
#include <chrono>

void CProgram::Run(const SProgramParams &programParams)
{
    IProblem *problemDefinition = CreateProblemFromName(programParams.m_ProblemName);
    if (!problemDefinition)
    {
        std::cerr << "Invalid problem name: " << programParams.m_ProblemName << std::endl;
        return;
    }
    if (!problemDefinition->ReadDefinition(programParams.m_ProblemDefinitionPath))
    {
        std::cerr << "Invalid problem definition: " << programParams.m_ProblemDefinitionPath << " for problem: "
                  << programParams.m_ProblemName << std::endl;
        delete problemDefinition;
        return;
    }

    std::string instanceName = GetFileName(programParams.m_ProblemDefinitionPath);
    CConfigReader configReader;
    CConfigLines configLines;
    if (!configReader.ReadConfig(programParams.m_OptimizerConfigPath, configLines))
    {
        std::cerr << "Invalid optimizer config: " << programParams.m_OptimizerConfigPath << std::endl;
        delete problemDefinition;
        return;
    }

    for (size_t i = 0; i < programParams.m_ExecutionsCount; ++i)
    {
        SOptimizerParams optParams;
        optParams.m_Silent = programParams.m_Silent;
        optParams.m_ConfigLines = configLines;
        optParams.m_SeedValue = (int) i;

        std::string methodName;
        optParams.m_ConfigLines.TakeValue("Algorithm", methodName);

        if (IOptimizer *optimizer = CreateOptimizerFromName(methodName.c_str(), optParams, problemDefinition))
        {
            // Record the start time
            auto start = std::chrono::high_resolution_clock::now();

            std::cout << std::endl << "Starting run #" << i << " ==> ";
            SOptimizationResult optResult = optimizer->RunOptimization();

            // Record the end time and compute the duration
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

            if(methodName == "GA"){
                CResultsWriterGA resultsWriter(programParams.m_OutputDirPath);

                std::string populationSize;
                optParams.m_ConfigLines.TakeValue("PopulationSize", populationSize);

                std::string GenerationLimit;
                optParams.m_ConfigLines.TakeValue("GenerationLimit", GenerationLimit);

                std::string crossoverParams;
                optParams.m_ConfigLines.TakeValue("Crossover", crossoverParams);

                std::string crossoverName = crossoverParams.substr(0, crossoverParams.find_first_of(' '));
                std::string crossoverProbability1 = crossoverParams.substr(crossoverParams.find_first_of(' ') + 1, crossoverParams.find_last_of(' ') - crossoverParams.find_first_of(' ') - 1);
                crossoverProbability1.pop_back();
                std::string crossoverProbability2 = crossoverParams.substr(crossoverParams.find_last_of(' ') + 1);

                std::string mutationParams;
                optParams.m_ConfigLines.TakeValue("Mutation", mutationParams);

                std::string mutationName = mutationParams.substr(0, mutationParams.find_first_of(' '));
                std::string mutationProbability = mutationParams.substr(mutationParams.find_first_of(' ') + 1);

                std::string tournamentSize;
                optParams.m_ConfigLines.TakeValue("TourSize", tournamentSize);

                resultsWriter.WriteOptimizationResultsToFile(optResult, instanceName, 0, (int) i, methodName, populationSize, GenerationLimit, crossoverName, crossoverProbability1, crossoverProbability2, mutationName, mutationProbability, tournamentSize);
            }
            else if (methodName == "SA"){
                CResultsWriterSA resultsWriter(programParams.m_OutputDirPath);

                std::string initialTemperature;
                optParams.m_ConfigLines.TakeValue("TemperatureMax", initialTemperature);

                std::string coolingRate;
                optParams.m_ConfigLines.TakeValue("TemperatureReducing", coolingRate);

                std::string stoppingTemperature;
                optParams.m_ConfigLines.TakeValue("TemperatureMin", stoppingTemperature);

                std::string populationSize;
                optParams.m_ConfigLines.TakeValue("PopulationSize", populationSize);

                resultsWriter.WriteOptimizationResultsToFile(optResult, instanceName, 0, (int) i, methodName, stoppingTemperature, initialTemperature, coolingRate, populationSize);
            }
            else if (methodName == "RANDOM"){
                CResultsWriterRandom resultsWriter(programParams.m_OutputDirPath);

                resultsWriter.WriteOptimizationResultsToFile(optResult, instanceName, 0, (int) i, methodName);
            }

            std::cout << "Finished in " << duration.count() << "ms" << std::endl;

            delete optimizer;
        }
        else
        {
            std::cerr << "Invalid optimizer name: " << methodName << std::endl;
        }
    }
    delete problemDefinition;
}

IProblem *CProgram::CreateProblemFromName(const char *problemName)
{
    if (strcmp(problemName, "MSRCPSP_TaskAssign") == 0) return new CProblemMSRCPSP_TaskAssign();
    if (strcmp(problemName, "MSRCPSP_TaskOrdering") == 0) return new CProblemMSRCPSP_TaskOrdering();
    if (strcmp(problemName, "TTP_CityOrderKnapBin_MO") == 0) return new CProblemTTP_CityOrderKnapBin_MO();
    if (strcmp(problemName, "TTP_CityOrderKnapBin_SO") == 0) return new CProblemTTP_CityOrderKnapBin_SO();
    if (strcmp(problemName, "DTLZ") == 0) return new CProblemDTLZ();
    if (strcmp(problemName, "ZDT") == 0) return new CProblemZDT();
    return nullptr;
}

IOptimizer *CProgram::CreateOptimizerFromName(const char *optimizerName, const SOptimizerParams &optParams,
                                              const IProblem *problemDefinition)
{
    if (strcmp(optimizerName, "GA") == 0) return new CGeneticAlgorithm(optParams, problemDefinition);
    if (strcmp(optimizerName, "BNTGA") == 0) return new CBNTGA(optParams, problemDefinition);
    if (strcmp(optimizerName, "NTGA2") == 0) return new CNTGA2(optParams, problemDefinition);
    if (strcmp(optimizerName, "NSGAII") == 0) return new CNSGAII(optParams, problemDefinition);
    if (strcmp(optimizerName, "MOEAD") == 0) return new CMOEAD(optParams, problemDefinition);
    if (strcmp(optimizerName, "SPEA2") == 0) return new CSPEA2(optParams, problemDefinition);
    if (strcmp(optimizerName, "RANDOM") == 0) return new CRandomAlgorithm(optParams, problemDefinition);
    if (strcmp(optimizerName, "SA") == 0) return new CSimulatedAnnealing(optParams, problemDefinition);
    return nullptr;
}

std::string CProgram::GetFileName(const char *filePath)
{
    std::string strFilePath(filePath);
    size_t fileNameStartPos = strFilePath.rfind('/') + 1;
    size_t fileNameEndPos = strFilePath.rfind('.');
    return strFilePath.substr(fileNameStartPos, fileNameEndPos - fileNameStartPos);
}
