#pragma once

using TSolutionValue = float;
using TSolution = std::vector<TSolutionValue>;

struct SEncodingDescriptor
{
    TSolutionValue m_MinValue;
    TSolutionValue m_MaxValue;
};

enum class EEncodingType
{
    ASSOCIATION = 0,
    PERMUTATION,
    BINARY,
};

struct SEncodingSection
{
    std::vector<SEncodingDescriptor> m_SectionDescription;
    EEncodingType m_SectionType;
};

struct SProblemEncoding
{
    std::vector<SEncodingSection> m_Encoding;
};