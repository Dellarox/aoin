#pragma once

#include "ProblemTTP_CityOrderKnapBin_MO.h"

class CProblemTTP_CityOrderKnapBin_SO : public CProblemTTP_CityOrderKnapBin_MO
{
public:

    bool Evaluate(const TSolution& solution, TEvaluation& evaluation) const override;
    size_t GetObjCount() const override;
};