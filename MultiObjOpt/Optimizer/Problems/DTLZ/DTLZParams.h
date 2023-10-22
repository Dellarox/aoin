#pragma once

struct SDTLZParams
{
	size_t m_FunctionId = 0;
	size_t m_ObjCount = 0;
	size_t m_DimCount = 0;

	// Used in DTLZ4
	size_t m_Alpha = 0;
};