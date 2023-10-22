#include "DTLZReader.h"
#include "../../Utils/ReadUtils.h"

const std::string CDTLZReader::s_Delimiter = " ";
const std::string CDTLZReader::s_FunctionKey = "Function";
const std::string CDTLZReader::s_ObjCountKey = "ObjCount";
const std::string CDTLZReader::s_DimCountKey = "DimCount";
const std::string CDTLZReader::s_AlphaKey = "Alpha";

bool CDTLZReader::ReadDefinition(const char* filePath, SDTLZParams& instanceParams) const
{
	std::ifstream readFileStream(filePath);

	ReadUtils::GotoReadSizeByKey(readFileStream, s_FunctionKey, s_Delimiter, instanceParams.m_FunctionId);
	ReadUtils::GotoReadSizeByKey(readFileStream, s_ObjCountKey, s_Delimiter, instanceParams.m_ObjCount);
	ReadUtils::GotoReadSizeByKey(readFileStream, s_DimCountKey, s_Delimiter, instanceParams.m_DimCount);
	ReadUtils::GotoReadSizeByKey(readFileStream, s_AlphaKey, s_Delimiter, instanceParams.m_Alpha);

	readFileStream.close();
	return true;
}
