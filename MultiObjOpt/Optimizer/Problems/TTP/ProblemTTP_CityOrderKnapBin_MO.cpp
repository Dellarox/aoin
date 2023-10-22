#include "ProblemTTP_CityOrderKnapBin_MO.h"
#include "TTPReader.h"

#define TTP_SAVE_FIXED_GENES 0

bool CProblemTTP_CityOrderKnapBin_MO::ReadDefinition(const char* filePath)
{
    CTTPReader ttpReader;

    if (ttpReader.ReadDefinition(filePath, m_TTPTemplate))
    {
        PrepareProblemEncoding();
        return true;
    }

    return false;
}

bool CProblemTTP_CityOrderKnapBin_MO::Evaluate(const TSolution& solution, TEvaluation& evaluation) const
{
    // TODO - part of this implementation can be moved to the problem definition

    // Build solution
    auto& items = m_TTPTemplate.GetItems();
    auto& cityItems = m_TTPTemplate.GetCityItems();
    auto& distMtx = m_TTPTemplate.GetDistMtx();
    int capacity = m_TTPTemplate.GetCapacity();

    float minSpeed = m_TTPTemplate.GetMinSpeed();
    float maxSpeed = m_TTPTemplate.GetMaxSpeed();

    size_t itemsSize = m_TTPTemplate.GetItemsSize();
    size_t citiesSize = m_TTPTemplate.GetCitiesSize();
    std::vector<bool> selection(itemsSize, false);
    int currWeight = 0;

    // // Left to Right - different fixing heuristic
    // {
    //     size_t i = 0;
    //     while (i < itemsSize)
    //     {
    //         int w = items[i].m_Weight;
    //         if (ttpSolution[citiesSize + i] > 0 && currWeight + w <= capacity)
    //         {
    //             currWeight += w;
    //             selection[i] = true;
    //         }
    //         ++i;
    //     }
    // }

    // Ry ratio
    {
        const auto& itemsRatio = m_TTPTemplate.GetProfitRatioSortedItems();
        size_t i = 0;
        while (i < itemsSize)
        {
            size_t itemIdx = itemsRatio[i];
            int w = items[itemIdx].m_Weight;
            if (solution[citiesSize + itemIdx] > 0 && currWeight + w <= capacity)
            {
                currWeight += w;
                selection[itemIdx] = true;
            }
#if TTP_SAVE_FIXED_GENES
            else
			{
				// Save back the zeroed item
				solution[citiesSize + itemIdx] = 0;
			}
#endif
            ++i;
        }
    }

    // Evaluate
    float travellingTime = 0.f;
    currWeight = 0;
    int currentProfit = 0;

    // For each city
    for (size_t i = 0; i < citiesSize; ++i)
    {
        size_t cityIdx = solution[i];
        size_t nextCityIdx = solution[(i + 1) % citiesSize];

        const std::vector<size_t>& itemsInCity = cityItems[cityIdx];
        for (const size_t& itemIdx : itemsInCity)
        {
            if (selection[itemIdx])
            {
                currWeight += items[itemIdx].m_Weight;
                currentProfit += items[itemIdx].m_Profit;
            }
        }

        float velocity = maxSpeed - ((float)currWeight * ((maxSpeed - minSpeed) / (float)capacity));
        velocity = fmaxf(velocity, minSpeed);
        travellingTime += distMtx[cityIdx][nextCityIdx] / velocity;
    }

    // Assign evaluation values, we assume this bi-objective problem
    evaluation =
            {
                    travellingTime,
                    (float)-currentProfit // invert the profit (min -> optimum)
            };

    return true;
}

size_t CProblemTTP_CityOrderKnapBin_MO::GetObjCount() const
{
    return 2;
}

const SProblemEncoding& CProblemTTP_CityOrderKnapBin_MO::GetProblemEncoding() const
{
    return m_ProblemEncoding;
}

void CProblemTTP_CityOrderKnapBin_MO::PrepareProblemEncoding()
{
    size_t citiesSize = m_TTPTemplate.GetCitiesSize();
    size_t itemsSize = m_TTPTemplate.GetItemsSize();

    SEncodingSection citiesSection = SEncodingSection
            {
                    // city indices <0, n-1>
                    std::vector<SEncodingDescriptor>(citiesSize, SEncodingDescriptor{ (TSolutionValue)0,  (TSolutionValue)(citiesSize - 1) }),
                    EEncodingType::PERMUTATION
            };

    SEncodingSection knapsackSection = SEncodingSection
            {
                    std::vector<SEncodingDescriptor>(itemsSize, SEncodingDescriptor{ (TSolutionValue)0,  (TSolutionValue)1 }),
                    EEncodingType::BINARY
            };

    m_ProblemEncoding = SProblemEncoding{ { citiesSection, knapsackSection } };
}
