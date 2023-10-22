#pragma once

#include "Optimizers/ConfigReader/ConfigLines.h"

struct SOptimizerParams
{
	int m_SeedValue = 0;
	bool m_Silent = false;

	CConfigLines m_ConfigLines;
};