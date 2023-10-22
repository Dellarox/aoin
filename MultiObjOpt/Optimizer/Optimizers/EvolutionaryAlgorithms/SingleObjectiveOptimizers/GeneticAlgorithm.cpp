
#include <sstream>
#include "GeneticAlgorithm.h"
#include "Optimizers/EvolutionaryAlgorithms/ArchiveUtils.h"
#include "Optimizers/EvolutionaryAlgorithms/AggregatedFitness.h"

CGeneticAlgorithm::~CGeneticAlgorithm()
{
    Reset();
}

SOptimizationResult CGeneticAlgorithm::RunOptimization()
{
    Reset();

    m_optResult.m_ParetoFront.reserve(m_Params.m_GenerationLimit);
    m_Population.reserve(m_Params.m_PopulationSize);
    for (size_t i = 0; i < m_Params.m_PopulationSize; ++i)
    {
        createIndividual();
    }

    saveIterationInfoToResult();

    m_Population.reserve(m_Params.m_PopulationSize * 2);
    while (!StopCondition())
    {
        Evolve();
        ++m_Generation;
        saveIterationInfoToResult();
    }

    ArchiveUtils::CopyToArchiveForSingleObjective(m_Population, m_Archive, m_Params.m_ObjectiveWeights);
    return m_optResult;
}

void CGeneticAlgorithm::createIndividual()
{
    auto *newInd = new CIndividual();
    newInd->SetGenes(generateGenes());
    newInd->Evaluate(m_ProblemDefinition);
    m_Population.push_back(newInd);
}

std::vector<TGene> CGeneticAlgorithm::generateGenes()
{
    std::vector<TGene> genes;
    m_Operators.RandomInit(genes, m_ProblemDefinition->GetProblemEncoding());
    return genes;
}

void CGeneticAlgorithm::Reset()
{
    m_Generation = 0;
    for (CIndividual *ind: m_Population)
    {
        delete ind;
    }
    m_Population.clear();
    m_optResult.m_ParetoFront.clear();
}

void CGeneticAlgorithm::Evolve()
{
    std::vector<CIndividual*> children;

    for (size_t i = 0; i < m_Params.m_PopulationSize; i += 2)
    {
        size_t firstParentIdx = m_Operators.FitnessTournamentSelection(m_Population, m_Params.m_TourSize,
                                                                       m_Params.m_ObjectiveWeights);
        size_t secondParentIdx = m_Operators.FitnessTournamentSelection(m_Population, m_Params.m_TourSize,
                                                                        m_Params.m_ObjectiveWeights);
        const auto &firstParentGenes = m_Population[firstParentIdx]->GetGenes();
        const auto &secondParentGenes = m_Population[secondParentIdx]->GetGenes();

        std::vector<TGene> firstChildGenes = firstParentGenes;
        std::vector<TGene> secondChildGenes = secondParentGenes;

        if (CCrossoverOperator *crossover = m_Params.m_Crossover)
        {
            crossover->Crossover(firstParentGenes, secondParentGenes, firstChildGenes, secondChildGenes);
        }

        if (CMutationOperator *mutation = m_Params.m_Mutation)
        {
            mutation->Mutate(firstChildGenes);
            mutation->Mutate(secondChildGenes);
        }

        auto *firstInd = new CIndividual();
        firstInd->SetGenes(firstChildGenes);
        firstInd->Evaluate(m_ProblemDefinition);
        children.push_back(firstInd);

        auto *secondInd = new CIndividual();
        secondInd->SetGenes(secondChildGenes);
        secondInd->Evaluate(m_ProblemDefinition);
        children.push_back(secondInd);
    }

    // Combine parents and children
    m_Population.insert(m_Population.end(), children.begin(), children.end());

    // Sort combined population based on fitness
    std::sort(m_Population.begin(), m_Population.end(), [](const CIndividual* a, const CIndividual* b) {
        return a->GetEvaluation() < b->GetEvaluation(); // Assumes smaller evaluations are better. Adjust if needed.
    });

    // Delete and clear the unneeded individuals
    for (size_t i = m_Params.m_PopulationSize; i < m_Population.size(); ++i)
    {
        delete m_Population[i];
    }
    m_Population.resize(m_Params.m_PopulationSize); // Resize to keep only the top individuals
}

bool CGeneticAlgorithm::StopCondition() const
{
    return m_Generation >= m_Params.m_GenerationLimit;
}

void CGeneticAlgorithm::saveIterationInfoToResult()
{
    TEvaluation iterationInfo;
    iterationInfo.reserve(4);
    iterationInfo.emplace_back(m_Generation);

    double bestFitness = std::numeric_limits<double>::max();   // Since we're minimizing.
    double worstFitness = std::numeric_limits<double>::lowest(); // Set to the lowest possible value initially.
    double totalFitness = 0.0;

    for (const auto& individual : m_Population)
    {
        double fitness = CAggregatedFitness::ComputeAggregatedFitness(individual, m_Params.m_ObjectiveWeights);

        // Finding the best fitness.
        if (fitness < bestFitness)
        {
            bestFitness = fitness;
        }

        // Finding the worst fitness.
        if (fitness > worstFitness)
        {
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

CGeneticAlgorithm::SGAParams::SGAParams(const SOptimizerParams &optParams, const IProblem *optProblem)
        : SOptimizerParams(optParams)
{
    m_ConfigLines.TakeValue("PopulationSize", m_PopulationSize);
    m_ConfigLines.TakeValue("GenerationLimit", m_GenerationLimit);
    m_ConfigLines.TakeValue("TourSize", m_TourSize);

    std::string weightsString;
    m_ConfigLines.TakeValue("ObjectiveWeights", weightsString);
    readWeights(weightsString);

    std::string mutationInitString;
    m_ConfigLines.TakeValue("Mutation", mutationInitString);
    m_Mutation = COperators::CreateMutationOperator(mutationInitString, optProblem, optParams.m_SeedValue);

    std::string crossoverInitString;
    m_ConfigLines.TakeValue("Crossover", crossoverInitString);
    m_Crossover = COperators::CreateCrossoverOperator(crossoverInitString, optProblem, optParams.m_SeedValue);
}

CGeneticAlgorithm::SGAParams::~SGAParams()
{
    if (m_Mutation)
    {
        delete m_Mutation;
        m_Mutation = nullptr;
    }

    if (m_Crossover)
    {
        delete m_Crossover;
        m_Crossover = nullptr;
    }
}

void CGeneticAlgorithm::SGAParams::readWeights(const std::string &weights)
{
    if (weights.empty())
    {
        m_ObjectiveWeights = {};
        return;
    }

    std::istringstream stream(weights.substr(1, weights.length() - 2)); // remove brackets
    std::string token;
    std::vector<float> values;

    while (std::getline(stream, token, ','))
    {
        values.push_back(std::stof(token));  // convert string to float
    }

    m_ObjectiveWeights.reserve(values.size());

    for (float & value : values)
    {
        m_ObjectiveWeights.emplace_back(value);
    }
}
