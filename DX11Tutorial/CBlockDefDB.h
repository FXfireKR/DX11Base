#pragma once
#include "BlockDefTypes.h"

class CBlockDefDB
{
public:
	CBlockDefDB() = default;
	~CBlockDefDB() = default;

	void Initialize(const char* resourceRoot);
	bool Load();
	void Clear();

	const BlockDef* GetBlockDef(BLOCK_ID blockID) const;
	const BlockDef* FindBlockDef(const char* blockName) const;

	const BLOCK_ID FindBlockID(const char* blockName) const;
	const char* FindBlockName(BLOCK_ID blockID) const;

	const bool IsValidBlockID(BLOCK_ID blockID) const;

	const bool IsAir(BLOCK_ID blockID) const;
	const bool IsOpaque(BLOCK_ID blockID) const;
	const bool IsSolid(BLOCK_ID blockID) const;
	const bool IsFullCube(BLOCK_ID blockID) const;

public:
	inline const vector<BlockDef>& GetBlockDefs() const { return m_vecBlockDefs; }

private:
	bool _LoadRegistry();
	bool _LoadBlockDefs();
	bool _LoadOnBlockDef(const filesystem::path& filePath);

	bool _ReadJson(const filesystem::path& path, rapidjson::Document& outDoc) const;
	bool _ParseBlockDef(const rapidjson::Value& root, BlockDef& outDef);
	bool _ResolveInheritance();
	
	static bool _ParseRenderLayer(const char* strLayer, BLOCK_RENDER_LAYER& outLayer);
	static bool _ParseCollisionType(const char* strType, BLOCK_COLLISION_TYPE& outType);

private:
	string m_strResourceRoot;

	// index == BLOCK_ID
	unordered_map<string, BLOCK_ID> m_mapNameToID;
	vector<string> m_vecIDToName;
	vector<BlockDef> m_vecBlockDefs;
};