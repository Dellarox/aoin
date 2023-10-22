#pragma once

#include <stdint.h>
#include <cmath>
#include <float.h>

using TSkillType = uint8_t;
using TSkillLevel = uint8_t;

struct SSkill
{
	TSkillType m_TypeID;
	TSkillLevel m_Level;

	SSkill();
	SSkill(TSkillType typeID, TSkillLevel level);
};
