#pragma once
#include "singleton.h"

class CBlockStateDB : public singleton<CBlockStateDB>
{
public:
	void Initialize(const char* path);
	//bool Evaluate(const char* blockKey, const )

private:
	bool _LoadBlockStates(const filesystem::path& path);
	bool _ReadBlockStateJSON(const string& path);
	bool _ReadVariants(const rapidjson::Value& variants);
	bool _ReadMultipart(const rapidjson::Value& multipart);

private:
	unordered_map<BLOCK_ID, BlockStateDef> m_mapBlockState;
	unordered_map<BLOCK_ID, BlockTypeDef> m_mapBlockType;
	GlobalPropertyRegistry m_propRegistry;
};