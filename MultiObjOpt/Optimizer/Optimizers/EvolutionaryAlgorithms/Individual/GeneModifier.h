#pragma once

class CRandom;

/*
* Class containing atomic genotype operations
* It is initialized with a Random object to maintain repeatable results
*/
class CGeneModifier
{
public:

	CGeneModifier(CRandom* random);

private:

	CRandom* m_Random = nullptr;
};