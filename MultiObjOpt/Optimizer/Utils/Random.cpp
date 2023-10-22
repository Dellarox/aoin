#include "Random.h"

CRandom::CRandom(int seed)
	: m_RNG(seed)
	, m_RandomFloat(0.f, 0.99999f)
{
}

size_t CRandom::NextIndex(const size_t& size)
{
	return (size_t)(m_RandomFloat(m_RNG) * (float)size);
}

float CRandom::NextFloat(const float& maxValue)
{
	return m_RandomFloat(m_RNG) * maxValue;
}

float CRandom::NextFloat()
{
	return m_RandomFloat(m_RNG);
}
