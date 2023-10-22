#pragma once
#include "Optimizers/EvolutionaryAlgorithms/Individual/Individual.h"

// TODO - make generic, for now it is used with Individual only

class CNonDominatedSorting
{
public:

	void Cluster(std::vector<CIndividual*>& population, std::vector<std::vector<size_t>>& clusters);

private:

	struct SSolution
	{
		SSolution(size_t i);

		size_t m_Idx;
		std::vector<SSolution*> m_DominatedSolutions;
		size_t m_DominationCounter;
	};
};