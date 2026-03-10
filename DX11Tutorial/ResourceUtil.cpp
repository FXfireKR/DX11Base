#include "pch.h"
#include "ResourceUtil.h"

bool BuildBlockKeyFromPath(const std::filesystem::path& filePath, std::string& outBlockKey)
{
	const std::string strPath = filePath.generic_string();

	const size_t posAssets = strPath.find("/assets/");
	if (posAssets == std::string::npos) 
		return false;

	size_t posBlockState = strPath.find("/blockstates/");
	if (posBlockState == std::string::npos) 
		return false;

	size_t namespaceBeg = posAssets + std::string("/assets/").size();
	size_t namespaceEnd = posBlockState;
	if (namespaceEnd <= namespaceBeg) 
		return false;

	const std::string nameSpace = strPath.substr(namespaceBeg, namespaceEnd - namespaceBeg);

	size_t relBeg = posBlockState + std::string("/blockstates/").size();
	size_t relEnd = strPath.rfind(".json");
	if (relEnd == std::string::npos || relEnd <= relBeg) 
		return false;

	const std::string rel = strPath.substr(relBeg, relEnd - relBeg);

	outBlockKey = nameSpace + ":" + rel;
	return true;
}