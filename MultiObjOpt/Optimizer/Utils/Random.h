#pragma once

#include <random>
#include <algorithm>
#include <cmath>

/*
* Randomization class
*/
class CRandom
{
public:

	CRandom(int seed);

	size_t NextIndex(const size_t& maxValue);
	float NextFloat(const float& maxValue);
	float NextFloat();

	template <class ClassIt>
	void Shuffle(ClassIt itFirst, ClassIt itLast)
	{
		std::shuffle(itFirst, itLast, m_RNG);
	}

private:
	std::mt19937 m_RNG;
	std::uniform_real_distribution<float> m_RandomFloat;
};
