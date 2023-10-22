
#include <sstream>
#include "SimulatedAnnealing.h"
#include "Optimizers/EvolutionaryAlgorithms/ArchiveUtils.h"
#include "Optimizers/EvolutionaryAlgorithms/AggregatedFitness.h"

SOptimizationResult CSimulatedAnnealing::RunOptimization() {
    m_GenerationLimit = 0;
    m_Temperature = m_Params.m_TemperatureMax;
    while (!StopCondition()) {
        m_GenerationLimit += 1;
        ReduceTemperature();
    }

    m_optResult.m_ParetoFront.reserve(m_GenerationLimit);
    m_Population.reserve(m_Params.m_PopulationSize);
    for (size_t i = 0; i < m_Params.m_PopulationSize; ++i) {
        createIndividual();
    }

    saveIterationInfoToResult();

    m_Population.reserve(m_Params.m_PopulationSize * 2);
    m_Temperature = m_Params.m_TemperatureMax;
    while (!StopCondition()) {
        Anneal();
        ++m_Generation;
        saveIterationInfoToResult();
    }

    ArchiveUtils::CopyToArchiveForSingleObjectiveSA(m_Population, m_Archive);
    return m_optResult;
}

void CSimulatedAnnealing::createIndividual() {
    auto *newInd = new CIndividual();
    newInd->SetGenes(generateGenes());
    newInd->Evaluate(m_ProblemDefinition);
    m_Population.push_back(newInd);
}

std::vector<TGene> CSimulatedAnnealing::generateGenes() {
    std::vector<TGene> genes;
    m_Operators.RandomInit(genes, m_ProblemDefinition->GetProblemEncoding());
    return genes;
}

void CSimulatedAnnealing::Anneal() {
    std::vector<int> individualsIndexesToRemove;
    for (int i = 0; i < m_Population.size(); ++i) {
        CIndividual *individual = m_Population[i];
        CIndividual *neighbour = CreateNeighbour(individual);
        double evalHelp;
        double deltaEnergy = neighbour->GetEvalValue(0);
        double energy = individual->GetEvalValue(0);

        if (deltaEnergy < energy) {
            evalHelp = 1;
        } else {
            evalHelp = exp(-(deltaEnergy - energy) / m_Temperature);
        }

        if (evalHelp > (double) rand() / RAND_MAX) {
            individualsIndexesToRemove.push_back(i);
            m_Population.push_back(neighbour);
        }
    }

    std::sort(individualsIndexesToRemove.begin(), individualsIndexesToRemove.end(), std::greater<>());
    for (int index: individualsIndexesToRemove) {
        delete m_Population[index];
        m_Population.erase(m_Population.begin() + index);
    }

    ReduceTemperature();
}

CIndividual *CSimulatedAnnealing::CreateNeighbour(CIndividual *individual) {
    std::vector<TGene> neighbourGenes = createNewGenes(individual);
    auto *neighbour = new CIndividual();
    neighbour->SetGenes(neighbourGenes);
    neighbour->Evaluate(m_ProblemDefinition);
    return neighbour;
}

std::vector<TGene> CSimulatedAnnealing::createNewGenes(CIndividual *individual) {
    std::vector<TGene> neighbourGenes;
    neighbourGenes.assign(individual->GetGenes().begin(),
                          individual->GetGenes().end()); // at the start neigh genes are the same like the individual genes
    unsigned long long index1 = 0, index2 = 0;
    unsigned long long helperSwapItemsBinary = m_ProblemDefinition->GetProblemEncoding().m_Encoding[1].m_SectionDescription.size();
    while (index1 == index2) {
        index1 = rand() % helperSwapItemsBinary;
        index2 = rand() % helperSwapItemsBinary;
    }

    std::swap(neighbourGenes[index1],
              neighbourGenes[index2]); // then swapping it to get the genes that will not be the same

    std::swap(neighbourGenes[index1 + helperSwapItemsBinary], neighbourGenes[index2 +
                                                                             helperSwapItemsBinary]); // at the end swapping the items in cities that we want to get or not
    return neighbourGenes;
}

bool CSimulatedAnnealing::StopCondition() const {
    return m_Temperature < m_Params.m_TemperatureMin;
}

void CSimulatedAnnealing::saveIterationInfoToResult() {
    TEvaluation iterationInfo;
    iterationInfo.reserve(4);
    iterationInfo.emplace_back(m_Generation);

    double bestFitness = std::numeric_limits<double>::max();   // Since we're minimizing.
    double worstFitness = std::numeric_limits<double>::lowest(); // Set to the lowest possible value initially.
    double totalFitness = 0.0;

    for (const auto &individual: m_Population) {
        double fitness = CAggregatedFitness::ComputeAggregatedFitnessSA(individual);

        // Finding the best fitness.
        if (fitness < bestFitness) {
            bestFitness = fitness;
        }

        // Finding the worst fitness.
        if (fitness > worstFitness) {
            worstFitness = fitness;
        }

        // Summing up for mean.
        totalFitness += fitness;
    }

    double meanFitness = totalFitness / double(m_Population.size());

    iterationInfo.emplace_back(bestFitness);
    iterationInfo.emplace_back(meanFitness);
    iterationInfo.emplace_back(worstFitness);

    m_optResult.m_ParetoFront.emplace_back(std::move(iterationInfo));
}

void CSimulatedAnnealing::ReduceTemperature() {
    m_Temperature = m_Temperature * m_Params.m_TemperatureReducing;
}

CSimulatedAnnealing::SSAParams::SSAParams(const SOptimizerParams &optParams, const IProblem *optProblem)
        : SOptimizerParams(optParams) {
    m_ConfigLines.TakeValue("PopulationSize", m_PopulationSize);
    m_ConfigLines.TakeValue("TemperatureMin", m_TemperatureMin);
    m_ConfigLines.TakeValue("TemperatureMax", m_TemperatureMax);
    m_ConfigLines.TakeValue("TemperatureReducing", m_TemperatureReducing);
}

