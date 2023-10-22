#include <cmath>
#include "ProblemDTLZ.h"
#include "DTLZReader.h"

constexpr float pi()
{
	return 3.141592653589793238462643383279502884197169399f;
}

constexpr float pi_half()
{
	return 3.1415926535897932384626433832795028841971693993f / 2;
}

bool CProblemDTLZ::ReadDefinition(const char* filePath)
{
	CDTLZReader problemReader;
	if (!problemReader.ReadDefinition(filePath, m_InstanceParams))
	{
		return false;
	}

	SEncodingSection associationSection;
	associationSection.m_SectionType = EEncodingType::ASSOCIATION;
	associationSection.m_SectionDescription = std::vector<SEncodingDescriptor>(m_InstanceParams.m_DimCount, { (TSolutionValue)0, (TSolutionValue)1 });
	m_ProblemEncoding = SProblemEncoding{ { associationSection } };

	return true;
}

bool CProblemDTLZ::Evaluate(const TSolution& solution, TEvaluation& evaluation) const
{
	switch (m_InstanceParams.m_FunctionId)
	{
	case 1:
		evaluation = f1_fitness(solution);
		break;
	case 2:
	case 3:
		evaluation = f23_fitness(solution);
		break;
	case 4:
		evaluation = f4_fitness(solution);
		break;
	case 5:
	case 6:
		evaluation = f56_fitness(solution);
		break;
	case 7:
		evaluation = f7_fitness(solution);
		break;
	default:
		return false;
	}
	return true;
}

size_t CProblemDTLZ::GetObjCount() const
{
	return m_InstanceParams.m_ObjCount;
}

const SProblemEncoding& CProblemDTLZ::GetProblemEncoding() const
{
	return m_ProblemEncoding;
}

// Convergence metric for a dv (0 = converged to the optimal front)
float CProblemDTLZ::g_func(const std::vector<float>& x) const
{
	float retval = 0.;
	switch (m_InstanceParams.m_FunctionId)
	{ 
	case 6:
		retval = g6_func(x);
		break;
	case 7:
		retval = g7_func(x);
		break;
	case 1:
	case 3:
		retval = g13_func(x);
		break;
	case 2:
	case 4:
	case 5:
		retval = g245_func(x);
		break;
	}
	return retval;
}

// Implementations of the different g-functions used
float CProblemDTLZ::g13_func(const std::vector<float>& x) const
{
	float y = 0.f;
	for (decltype(x.size()) i = 0u; i < x.size(); ++i) 
	{
		y += float(std::pow(x[i] - 0.5f, 2)) - std::cos(20.f * pi() * (x[i] - 0.5f));
	}
	return 100.f * (y + static_cast<float>(x.size()));
}

float CProblemDTLZ::g245_func(const std::vector<float>& x) const
{
	float y = 0.f;
	for (decltype(x.size()) i = 0u; i < x.size(); ++i) 
	{
		y += float(std::pow(x[i] - 0.5f, 2));
	}
	return y;
}

float CProblemDTLZ::g6_func(const std::vector<float>& x) const
{
	float y = 0.0f;
	for (decltype(x.size()) i = 0u; i < x.size(); ++i)
	{
		y += powf(x[i], 0.1f);
	}
	return y;
}

float CProblemDTLZ::g7_func(const std::vector<float>& x) const
{
	// NOTE: the original g-function should return 1 + (9.0 / x.size()) * y but we drop the 1
	// to have the minimum at 0.0 so we can use the p_distance implementation in base_dtlz
	// to have the p_distance converging towards 0.0 rather then towards 1.0
	float y = 0.f;
	for (decltype(x.size()) i = 0u; i < x.size(); ++i) 
	{
		y += x[i];
	}
	return (9.f / static_cast<float>(x.size())) * y;
}

// Implementation of the distribution function h
float CProblemDTLZ::h7_func(const std::vector<float>& f, float g) const
{
	// NOTE: we intentionally ignore the last element of the vector to make things easier
	float y = 0.f;

	for (decltype(f.size()) i = 0u; i < f.size() - 1; ++i) 
	{
		y += (f[i] / (1.0f + g)) * (1.0f + std::sin(3 * pi() * f[i]));
	}
	return static_cast<float>(m_InstanceParams.m_ObjCount) - y;
}

std::vector<float> CProblemDTLZ::f1_fitness(const std::vector<float>& x) const
{
	std::vector<float> f(m_InstanceParams.m_ObjCount);
	// computing distance-function
	std::vector<float> x_M;

	for (decltype(x.size()) i = f.size() - 1u; i < x.size(); ++i) 
	{
		x_M.push_back(x[i]);
	}

	float g = g_func(x_M);

	// computing shape-functions
	f[0] = 0.5f * (1.f + g);

	for (decltype(f.size()) i = 0u; i < f.size() - 1u; ++i) 
	{
		f[0] *= x[i];
	}

	for (decltype(f.size()) i = 1u; i < f.size() - 1u; ++i) 
	{
		f[i] = 0.5f * (1.0f + g);
		for (decltype(f.size()) j = 0u; j < f.size() - (i + 1); ++j) 
		{
			f[i] *= x[j];
		}
		f[i] *= 1.f - x[f.size() - (i + 1u)];
	}

	f[f.size() - 1u] = 0.5f * (1.f - x[0]) * (1.f + g);
	return f;
}

std::vector<float> CProblemDTLZ::f23_fitness(const std::vector<float>& x) const
{
	std::vector<float> f(m_InstanceParams.m_ObjCount);
	// computing distance-function
	std::vector<float> x_M;
	for (decltype(x.size()) i = f.size() - 1u; i < x.size(); ++i) 
	{
		x_M.push_back(x[i]);
	}

	auto g = g_func(x_M);

	// computing shape-functions
	f[0] = (1.f + g);
	for (decltype(f.size()) i = 0u; i < f.size() - 1u; ++i) 
	{
		f[0] *= cos(x[i] * pi_half());
	}

	for (decltype(f.size()) i = 1u; i < f.size() - 1u; ++i) 
	{
		f[i] = (1.f + g);
		for (decltype(f.size()) j = 0u; j < f.size() - (i + 1u); ++j) 
		{
			f[i] *= cos(x[j] * pi_half());
		}
		f[i] *= std::sin(x[f.size() - (i + 1u)] * pi_half());
	}

	f[f.size() - 1u] = (1.f + g) * std::sin(x[0] * pi_half());
	return f;
}

std::vector<float> CProblemDTLZ::f4_fitness(const std::vector<float>& x) const
{
	std::vector<float> f(m_InstanceParams.m_ObjCount);
	// computing distance-function
	std::vector<float> x_M;
	for (decltype(x.size()) i = f.size() - 1; i < x.size(); ++i) 
	{
		x_M.push_back(x[i]);
	}

	auto g = g_func(x_M);

	// computing shape-functions
	f[0] = (1.f + g);
	for (decltype(f.size()) i = 0u; i < f.size() - 1u; ++i) 
	{
		f[0] *= std::cos(powf(x[i], static_cast<float>(m_InstanceParams.m_Alpha)) * pi_half());
	}
	
	for (decltype(f.size()) i = 1u; i < f.size() - 1u; ++i) 
	{
		f[i] = (1.0f + g);
		for (decltype(f.size()) j = 0u; j < f.size() - (i + 1u); ++j)
		{
			f[i] *= std::cos(powf(x[j], static_cast<float>(m_InstanceParams.m_Alpha)) * pi_half());
		}
		f[i] *= std::sin(powf(x[f.size() - (i + 1u)], static_cast<float>(m_InstanceParams.m_Alpha)) * pi_half());
	}

	f[f.size() - 1u] = (1.f + g) * std::sin(powf(x[0], static_cast<float>(m_InstanceParams.m_Alpha)) * pi_half());
	return f;
}

std::vector<float> CProblemDTLZ::f56_fitness(const std::vector<float>& x) const
{
	std::vector<float> f(m_InstanceParams.m_ObjCount);
	// computing distance-function
	std::vector<float> x_M;

	for (decltype(x.size()) i = f.size() - 1u; i < x.size(); ++i) 
	{
		x_M.push_back(x[i]);
	}

	auto g = g_func(x_M);

	// computing meta-variables
	std::vector<float> theta(f.size(), 0.f);
	theta[0] = x[0];
	float t = 1.f / (2.f * (1.f + g));

	for (decltype(f.size()) i = 1u; i < f.size(); ++i) 
	{
		theta[i] = t + ((g * x[i]) / (1.0f + g));
	}

	// computing shape-functions
	f[0] = (1.f + g);
	for (decltype(f.size()) i = 0u; i < f.size() - 1u; ++i) 
	{
		f[0] *= std::cos(theta[i] * pi_half());
	}

	for (decltype(f.size()) i = 1u; i < f.size() - 1u; ++i) 
	{
		f[i] = (1.f + g);
		for (decltype(f.size()) j = 0u; j < f.size() - (i + 1u); ++j) 
		{
			f[i] *= std::cos(theta[j] * pi_half());
		}
		f[i] *= std::sin(theta[f.size() - (i + 1u)] * pi_half());
	}

	f[f.size() - 1u] = (1.f + g) * std::sin(theta[0] * pi_half());
	return f;
}

std::vector<float> CProblemDTLZ::f7_fitness(const std::vector<float>& x) const
{
	std::vector<float> f(m_InstanceParams.m_ObjCount);
	// computing distance-function
	std::vector<float> x_M;
	float g;

	for (decltype(x.size()) i = f.size() - 1u; i < x.size(); ++i) 
	{
		x_M.push_back(x[i]);
	}

	g = 1.f + g_func(x_M); // +1.0 according to the original definition of the g-function for DTLZ7

	// computing shape-functions
	for (decltype(f.size()) i = 0u; i < f.size() - 1u; ++i) 
	{
		f[i] = x[i];
	}

	f[f.size() - 1u] = (1.f + g) * h7_func(f, g);
	return f;
}
