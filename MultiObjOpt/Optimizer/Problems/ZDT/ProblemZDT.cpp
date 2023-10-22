#include <cmath>
#include <algorithm>
#include "ProblemZDT.h"
#include "ZDTReader.h"

constexpr float pi()
{
	return 3.141592653589793238462643383279502884197169399f;
}

bool CProblemZDT::ReadDefinition(const char* filePath)
{
	CZDTReader problemReader;
	if (!problemReader.ReadDefinition(filePath, m_InstanceParams))
	{
		return false;
	}

	SEncodingSection associationSection;
	associationSection.m_SectionType = EEncodingType::ASSOCIATION;
	associationSection.m_SectionDescription = BuildBounds();
	m_ProblemEncoding = SProblemEncoding{ { associationSection } };

	return true;
}

bool CProblemZDT::Evaluate(const TSolution& solution, TEvaluation& evaluation) const
{
	switch (m_InstanceParams.m_FunctionId)
	{
	case 1:
        evaluation = f1_fitness(solution);
		break;
	case 2:
        evaluation = f2_fitness(solution);
		break;
	case 3:
        evaluation = f3_fitness(solution);
		break;
	case 4:
        evaluation = f4_fitness(solution);
		break;
	case 5:
        evaluation = f5_fitness(solution);
		break;
	case 6:
        evaluation = f6_fitness(solution);
		break;
	default:
		return false;
	}
	return true;
}

size_t CProblemZDT::GetObjCount() const
{
    return 2;
}

const SProblemEncoding& CProblemZDT::GetProblemEncoding() const
{
	return m_ProblemEncoding;
}

std::vector<SEncodingDescriptor> CProblemZDT::BuildBounds() const
{
	switch (m_InstanceParams.m_FunctionId)
    {
	case 1:
	case 2:
	case 3:
	case 6:
		return std::vector<SEncodingDescriptor>(m_InstanceParams.m_DimCount, { (TSolutionValue)0, (TSolutionValue)1 });
	case 4: 
    {
        auto bounds = std::vector<SEncodingDescriptor>(m_InstanceParams.m_DimCount, { -5.f, 5.f });
        bounds[0].m_MinValue = 0.0f;
        bounds[0].m_MaxValue = 1.0f;
        return bounds;
	}
	case 5: 
    {
		size_t dim = 30u + 5u * (m_InstanceParams.m_DimCount - 1u);
        return std::vector<SEncodingDescriptor>(dim, { (TSolutionValue)0, (TSolutionValue)1 });
	}
    default:
        return std::vector<SEncodingDescriptor>();
	}
}

std::vector<float> CProblemZDT::f1_fitness(const std::vector<float>& x) const
{
    float g = 0.f;
    std::vector<float> f(2, 0.f);
    f[0] = x[0];
    auto N = x.size();

    for (decltype(N) i = 1u; i < N; ++i) 
    {
        g += x[i];
    }
    g = 1.f + (9.f * g) / static_cast<float>(N - 1u);

    f[1] = g * (1.f - std::sqrt(x[0] / g));
    return f;
}

std::vector<float> CProblemZDT::f2_fitness(const std::vector<float>& x) const
{
    float g = 0.f;
    std::vector<float> f(2, 0.f);
    f[0] = x[0];
    auto N = x.size();

    for (decltype(N) i = 1u; i < N; ++i) 
    {
        g += x[i];
    }
    g = 1.f + (9.f * g) / static_cast<float>(N - 1u);
    f[1] = g * (1.f - (x[0] / g) * (x[0] / g));

    return f;
}

std::vector<float> CProblemZDT::f3_fitness(const std::vector<float>& x) const
{
    float g = 0.f;
    std::vector<float> f(2, 0.f);
    f[0] = x[0];
    auto N = x.size();

    for (decltype(N) i = 1u; i < N; ++i) 
    {
        g += x[i];
    }
    g = 1.f + (9.f * g) / static_cast<float>(N - 1u);
    f[1] = g * (1.f - std::sqrt(x[0] / g) - x[0] / g * std::sin(10.f * pi() * x[0]));

    return f;
}

std::vector<float> CProblemZDT::f4_fitness(const std::vector<float>& x) const
{
    float g = 0.f;
    std::vector<float> f(2, 0.f);
    f[0] = x[0];
    auto N = x.size();

    g = 1 + 10 * static_cast<float>(N - 1u);
    f[0] = x[0];
    for (decltype(N) i = 1u; i < N; ++i) 
    {
        g += x[i] * x[i] - 10.f * std::cos(4.f * pi() * x[i]);
    }
    f[1] = g * (1.f - std::sqrt(x[0] / g));

    return f;
}

std::vector<float> CProblemZDT::f5_fitness(const std::vector<float>& x_double) const
{
    float g = 0.f;
    std::vector<float> f(2, 0.f);
    auto size_x = x_double.size();
    auto n_vectors = ((size_x - 30u) / 5u) + 1u;

    unsigned k = 30;
    std::vector<size_t> u(n_vectors, 0u);
    std::vector<size_t> v(n_vectors);

    // Convert the input vector into rounded values (integers)
    std::vector<float> x;
    std::transform(x_double.begin(), x_double.end(), std::back_inserter(x),
        [](float item) { return std::round(item); });
    f[0] = x[0];

    // Counts how many 1s are there in the first (30 dim)
    u[0] = static_cast<size_t>(std::count(x.begin(), x.begin() + 30, 1.f));

    for (decltype(n_vectors) i = 1u; i < n_vectors; ++i) 
    {
        for (int j = 0; j < 5; ++j) 
        {
            if (x[k] == 1.) 
            {
                ++u[i];
            }
            ++k;
        }
    }
    f[0] = 1.0f + static_cast<float>(u[0]);
    for (decltype(n_vectors) i = 1u; i < n_vectors; ++i) 
    {
        if (u[i] < 5u) 
        {
            v[i] = 2u + u[i];
        }
        else 
        {
            v[i] = 1u;
        }
    }
    for (decltype(n_vectors) i = 1u; i < n_vectors; ++i) 
    {
        g += static_cast<float>(v[i]);
    }
    f[1] = g * (1.f / f[0]);
    return f;
}

std::vector<float> CProblemZDT::f6_fitness(const std::vector<float>& x) const
{
    float g = 0.f;
    std::vector<float> f(2, 0.f);
    f[0] = x[0];
    auto N = x.size();

    f[0] = 1 - std::exp(-4 * x[0]) * float(std::pow(std::sin(6 * pi() * x[0]), 6));
    for (decltype(N) i = 1; i < N; ++i) 
    {
        g += x[i];
    }
    g = 1 + 9 * std::pow((g / static_cast<float>(N - 1u)), 0.25f);
    f[1] = g * (1 - (f[0] / g) * (f[0] / g));

    return f;
}
