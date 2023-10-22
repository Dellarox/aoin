#include <regex>
#include <cstring>
#include <algorithm>
#include "Operators.h"
#include "Optimizers/EvolutionaryAlgorithms/AggregatedFitness.h"
#include "Mutation_Polynomial.h"
#include "Mutation_RandomBit.h"
#include "Crossover_UniformCX.h"
#include "Crossover_SBX.h"
#include "Crossover_TTP_OX_SX.h"
#include "Mutation_Swap.h"
#include "Mutation_Inverse.h"

COperators::COperators(CRandom *random)
    : m_Random(random), m_GeneMod(random)
{
}

void COperators::RandomInit(std::vector<TGene>& genotype, const SProblemEncoding& encoding)
{
    genotype.clear();

    // TODO - add total size information to the encoding and reserve the size

    for (const SEncodingSection& encodingSection : encoding.m_Encoding)
    {
        switch (encodingSection.m_SectionType)
        {
            case EEncodingType::ASSOCIATION:
            {
                for (const SEncodingDescriptor& encDesc : encodingSection.m_SectionDescription)
                {
                    genotype.push_back(encDesc.m_MinValue + m_Random->NextFloat(encDesc.m_MaxValue - encDesc.m_MinValue));
                }
                break;
            }
            case EEncodingType::PERMUTATION:
            {
                size_t sectionStart = genotype.size();
                size_t sectionSize = encodingSection.m_SectionDescription.size();
                for (size_t i = 0; i < sectionSize; ++i)
                {
                    genotype.push_back((TGene)i);
                }
                m_Random->Shuffle(genotype.begin() + sectionStart, genotype.begin() + sectionStart + sectionSize);
                break;
            }
            case EEncodingType::BINARY:
            {
                std::generate_n(std::back_inserter(genotype), encodingSection.m_SectionDescription.size(),
                                [this]() { return (TGene)m_Random->NextIndex(MAX_RANDOM_BINARY_INDEX); });
            }
        }
    }
}

size_t COperators::RankedTournamentSelection(const std::vector<CIndividual *> &population, size_t tourSize)
{
    size_t tournamentSize = tourSize;
    size_t popSize = population.size();

    size_t bestIdx = m_Random->NextIndex(popSize);
    size_t bestRank = population[bestIdx]->GetRank();

    for (size_t i = 1; i < tournamentSize; ++i)
    {
        size_t randomIdx = m_Random->NextIndex(popSize);
        size_t rank = population[randomIdx]->GetRank();
        if (rank < bestRank)
        {
            bestRank = rank;
            bestIdx = randomIdx;
        }
    }

    return bestIdx;
}

size_t COperators::FitnessTournamentSelection(const std::vector<CIndividual *> &population, size_t tourSize,
                                              const std::vector<float>& objectiveWeights)
{
    size_t popSize = population.size();

    // Initialize best index with a random individual
    size_t bestIdx = m_Random->NextIndex(popSize);
    float bestFitness = CAggregatedFitness::ComputeAggregatedFitness(population[bestIdx], objectiveWeights);

    for (size_t i = 1; i < tourSize; ++i)
    {
        size_t randomIdx = m_Random->NextIndex(popSize);
        float currentFitness = CAggregatedFitness::ComputeAggregatedFitness(population[randomIdx], objectiveWeights);

        if (currentFitness < bestFitness)
        { // Smaller fitness values are better
            bestFitness = currentFitness;
            bestIdx = randomIdx;
        }
    }

    return bestIdx;
}

CMutationOperator * COperators::CreateMutationOperator(const std::string &initString, const IProblem *problemDef, int seedValue)
{
    auto const re = std::regex{R"(\s+)"};
    auto const vec = std::vector<std::string>(std::sregex_token_iterator{initString.begin(), initString.end(), re, -1},
                                              std::sregex_token_iterator{});
    const char *opName = vec[0].c_str();

    // TODO - Explicit listing on supported types gives a full control but it might be changed in the future to more generic mechanism
    if (strcmp(opName, "Null") == 0)
    {
        return new CMutationOperator(problemDef, seedValue);
    }
//    else if (strcmp(opName, "RandomBit") == 0)
//    {
//        float mutProb = std::stof(vec[1]);
//        return new CMutation_RandomBit(problemDef, seedValue, mutProb);
//    }
//    else if (strcmp(opName, "Polynomial") == 0)
//    {
//        float mutProb = std::stof(vec[1]);
//        float etaCoef = std::stof(vec[2]);
//        return new CMutation_Polynomial(problemDef, seedValue, mutProb, etaCoef);
//    }
    else if (strcmp(opName, "Swap") == 0){
        float mutProb = std::stof(vec[1]);
        return new CMutation_Swap(problemDef, seedValue, mutProb);
    }
    else if (strcmp(opName, "Inverse") == 0){
        float mutProb = std::stof(vec[1]);
        return new CMutation_Inverse(problemDef, seedValue, mutProb);
    }

    return nullptr;
}

CCrossoverOperator* COperators::CreateCrossoverOperator(const std::string &initString, const IProblem *problemDef, int seedValue)
{
    auto const re = std::regex{R"(\s+)"};
    auto const vec = std::vector<std::string>(std::sregex_token_iterator{initString.begin(), initString.end(), re, -1},
                                              std::sregex_token_iterator{});
    const char *opName = vec[0].c_str();

    // TODO - Explicit listing on supported types gives a full control but it might be changed in the future to more generic mechanism
    if (strcmp(opName, "Null") == 0)
    {
        return new CCrossoverOperator(problemDef, seedValue);
    }
    else if (strcmp(opName, "UniformCX") == 0)
    {
        float cxProb = std::stof(vec[1]);
        return new CCrossover_UniformCX(problemDef, seedValue, cxProb);
    }
    else if (strcmp(opName, "SBX") == 0)
    {
        float cxProb = std::stof(vec[1]);
        float etaCoef = std::stof(vec[2]);
        return new CCrossover_SBX(problemDef, seedValue, cxProb, etaCoef);
    }
    else if (strcmp(opName, "TTP_OX_SX") == 0)
    {
        float routeCrProb = std::stof(vec[1]);
        float knapCrProb = std::stof(vec[2]);
        return new CCrossover_TTP_OX_SX(problemDef, seedValue, routeCrProb, knapCrProb);
    }

    return nullptr;
}
