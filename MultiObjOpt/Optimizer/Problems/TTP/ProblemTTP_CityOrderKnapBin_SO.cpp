#include "ProblemTTP_CityOrderKnapBin_SO.h"

bool CProblemTTP_CityOrderKnapBin_SO::Evaluate(const TSolution& solution, TEvaluation& evaluation) const
{
    if (CProblemTTP_CityOrderKnapBin_MO::Evaluate(solution, evaluation))
    {
        // Use "Renting Ratio" to combine two objectives
        evaluation = { evaluation[1] + (evaluation[0] * m_TTPTemplate.GetRentingRatio()) };
        return true;
    }
    else
    {
        return false;
    }
}

size_t CProblemTTP_CityOrderKnapBin_SO::GetObjCount() const
{
    return 1;
}