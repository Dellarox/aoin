#include "ZDTReader.h"
#include "../../Utils/ReadUtils.h"

const std::string CZDTReader::s_Delimiter = " ";
const std::string CZDTReader::s_FunctionKey = "Function";
const std::string CZDTReader::s_DimCountKey = "DimCount";

bool CZDTReader::ReadDefinition(const char* filePath, SZDTParams& instanceParams) const
{
	std::ifstream readFileStream(filePath);

	ReadUtils::GotoReadSizeByKey(readFileStream, s_FunctionKey, s_Delimiter, instanceParams.m_FunctionId);
	ReadUtils::GotoReadSizeByKey(readFileStream, s_DimCountKey, s_Delimiter, instanceParams.m_DimCount);

	readFileStream.close();
	return true;
}
