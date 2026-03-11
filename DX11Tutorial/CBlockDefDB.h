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

	const BlockDef* FindNameDef(const char* defName) const;

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
	bool _LoadTemplates();
	bool _LoadBlocks();
	bool _LoadOneDefFile(const filesystem::path& filePath);

	const bool _ReadJson(const filesystem::path& path, rapidjson::Document& outDoc) const;
	const bool _ParseBlockDefRaw(const rapidjson::Value& root, BlockDefRaw& outRaw) const;

	bool _BuildRuntimeDefs();
	const bool _ResolveOne(const string& blockName, BlockDef& outDef, unordered_set<string>& outVisiting) const;

	/*bool _LoadRegistry();
	bool _LoadBlockDefs();
	bool _LoadOneBlockDef(const filesystem::path& filePath);

	bool _ReadJson(const filesystem::path& path, rapidjson::Document& outDoc) const;
	bool _ParseBlockDef(const rapidjson::Value& root, BlockDef& outDef);

	bool _ResolveAllDefs();
	bool _ResolveOneDef(const string& name, unordered_map<string, uint8_t>& visitState);
	void _MergeParentToChild(const BlockDef& parent, BlockDef& child);
	bool _BuildRuntimeTable();
	
	static bool _ParseRenderLayer(const char* strLayer, BLOCK_RENDER_LAYER& outLayer);
	static bool _ParseCollisionType(const char* strType, BLOCK_COLLISION_TYPE& outType);*/

private:
	string m_strResourceRoot;

	// registry
	unordered_map<string, BLOCK_ID> m_mapNameToID;
	vector<string> m_vecIDToName;

	// all defs (template + concrete)
	unordered_map<string, BlockDefRaw> m_mapTemplates; // template only
	unordered_map<string, BlockDefRaw> m_mapNamedDefs; // actual blocks only

	// runtime concrete blocks only, index == BLOCK_ID
	vector<BlockDef> m_vecBlockDefs;
};