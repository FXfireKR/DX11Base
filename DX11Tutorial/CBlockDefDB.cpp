#include "pch.h"
#include "CBlockDefDB.h"
#include "IFileWrapper.h"

void CBlockDefDB::Initialize(const char* resourceRoot)
{
	m_strResourceRoot = resourceRoot;
}

bool CBlockDefDB::Load()
{
	Clear();

	if (!_LoadRegistry())
	{
		assert(false && "CBlockDefDB::_LoadRegistry failed");
		return false;
	}

	if (!_LoadBlockDefs())
	{
		assert(false && "CBlockDefDB::_LoadBlockDefs failed");
		return false;
	}

	if (!_ResolveInheritance())
	{
		assert(false && "CBlockDefDB::_ResolveInheritance failed");
		return false;
	}

	return true;
}

void CBlockDefDB::Clear()
{
	m_mapNameToID.clear();
	m_vecIDToName.clear();
	m_vecBlockDefs.clear();
}

const BlockDef* CBlockDefDB::GetBlockDef(BLOCK_ID blockID) const
{
	if (!IsValidBlockID(blockID)) 
		return nullptr;

	const BlockDef& def = m_vecBlockDefs[blockID];
	if (!def.bLoaded) 
		return nullptr;

	return GetBlockDef(blockID);
}

const BlockDef* CBlockDefDB::FindBlockDef(const char* blockName) const
{
	BLOCK_ID blockID = FindBlockID(blockName);
	if (blockID == INVALID_BLOCK_ID)
		return nullptr;

	return GetBlockDef(blockID);
}

const BLOCK_ID CBlockDefDB::FindBlockID(const char* blockName) const
{
	if (!blockName)
		return INVALID_BLOCK_ID;

	auto it = m_mapNameToID.find(blockName);
	if (it == m_mapNameToID.end())
		return INVALID_BLOCK_ID;

	return it->second;
}

const char* CBlockDefDB::FindBlockName(BLOCK_ID blockID) const
{
	if (!IsValidBlockID(blockID))
		return nullptr;

	return m_vecIDToName[blockID].c_str();
}

const bool CBlockDefDB::IsValidBlockID(BLOCK_ID blockID) const
{
	return blockID < m_vecBlockDefs.size();
}

const bool CBlockDefDB::IsAir(BLOCK_ID blockID) const
{
	const BlockDef* pBlockDef = GetBlockDef(blockID);
	return pBlockDef ? pBlockDef->properties.bAir : false;
}

const bool CBlockDefDB::IsOpaque(BLOCK_ID blockID) const
{
	const BlockDef* pBlockDef = GetBlockDef(blockID);
	return pBlockDef ? pBlockDef->properties.bOpaque : false;
}

const bool CBlockDefDB::IsSolid(BLOCK_ID blockID) const
{
	const BlockDef* pBlockDef = GetBlockDef(blockID);
	return pBlockDef ? pBlockDef->properties.bSolid : false;
}

const bool CBlockDefDB::IsFullCube(BLOCK_ID blockID) const
{
	const BlockDef* pBlockDef = GetBlockDef(blockID);
	return pBlockDef ? pBlockDef->properties.bFullCube : false;
}

bool CBlockDefDB::_LoadRegistry()
{
	const filesystem::path filePath = filesystem::path(m_strResourceRoot) / "assets" / "minecraft" / "blocks" / "block_registry.json";
	rapidjson::Document doc;

	if (!_ReadJson(filePath, doc))
		return false;

	if (!doc.IsObject())
		return false;

	if (!doc.HasMember("entries") || !doc["entries"].IsObject())
		return false;

	const auto& entries = doc["entries"];
	BLOCK_ID maxID = 0;
	for (auto it = entries.MemberBegin(); it != entries.MemberEnd(); ++it)
	{
		if (!it->name.IsString() || !it->value.IsUint())
			continue;

		const string name = it->name.GetString();
		const BLOCK_ID blockID = static_cast<BLOCK_ID>(it->value.GetUint());

		if (m_mapNameToID.find(name) != m_mapNameToID.end())
		{
			assert(false && "duplicate block registry name");
			return false;
		}

		m_mapNameToID[name] = blockID;
		maxID = std::max(maxID, blockID);
	}

	m_vecIDToName.resize(static_cast<size_t>(maxID) + 1);
	m_vecBlockDefs.resize(static_cast<size_t>(maxID) + 1);

	for (const auto& kv : m_mapNameToID)
	{
		m_vecIDToName[kv.second] = kv.first;
		m_vecBlockDefs[kv.second].blockID = kv.second;
		m_vecBlockDefs[kv.second].name = kv.first;
	}

	return true;
}

bool CBlockDefDB::_LoadBlockDefs()
{
	const filesystem::path dirPath = filesystem::path(m_strResourceRoot) / "assets" / "minecraft" / "blocks";
	if (!filesystem::exists(dirPath))
		return false;

	for (auto& it : filesystem::recursive_directory_iterator(dirPath))
	{
		if (!it.is_regular_file()) continue;

		const auto filePath = it.path();
		if (filePath.extension() != ".json")
			continue;

		if (filePath.filename() == "block_registry.json")
			continue;

		if (!_LoadOnBlockDef(filePath))
			return false;
	}

	return true;
}

bool CBlockDefDB::_LoadOnBlockDef(const filesystem::path& filePath)
{
	rapidjson::Document doc;
	if (!_ReadJson(filePath, doc))
		return false;

	BlockDef def;
	if (!_ParseBlockDef(doc, def))
		return false;

	BLOCK_ID blockID = FindBlockID(def.name.c_str());
	if (!def.bIsTemplate)
	{
		if (blockID == INVALID_BLOCK_ID)
		{
			assert(false && "BlockDef name not found in registry");
			return false;
		}
	}

	def.blockID = blockID;
	def.bLoaded = true;

	m_vecBlockDefs[blockID] = std::move(def);
	return true;
}

bool CBlockDefDB::_ReadJson(const filesystem::path& path, rapidjson::Document& outDoc) const
{
	std::stringstream jsonBuffer;
	if (!IFileStreamWrapper::ReadAllStream(path.string(), jsonBuffer))
		return false;

	outDoc.Parse<rapidjson::kParseCommentsFlag | rapidjson::kParseTrailingCommasFlag>(jsonBuffer.str().c_str());
	if (outDoc.HasParseError() || !outDoc.IsObject())
		return false;

	return true;
}

bool CBlockDefDB::_ParseBlockDef(const rapidjson::Value& root, BlockDef& outDef)
{
	outDef = BlockDef{};

	if (root.HasMember("type") && root["type"].IsString())
	{
		string typeStr = root["type"].GetString();
		outDef.bIsTemplate = (typeStr == "template");
	}

	if (!root.HasMember("name") || !root["name"].IsString())
		return false;

	outDef.name = root["name"].GetString();

	if (root.HasMember("parent") && root["parent"].IsString())
		outDef.parent = root["parent"].GetString();

	if (root.HasMember("stateSource") && root["stateSource"].IsString())
		outDef.stateSource = root["stateSource"].GetString();

	if (root.HasMember("soundProfile") && root["soundProfile"].IsString())
		outDef.soundProfile = root["soundProfile"].GetString();

	if (root.HasMember("properties") && root["properties"].IsObject())
	{
		const auto& props = root["properties"];

		if (props.HasMember("air") && props["air"].IsBool())
			outDef.properties.bAir = props["air"].GetBool();

		if (props.HasMember("opaque") && props["opaque"].IsBool())
			outDef.properties.bOpaque = props["opaque"].GetBool();

		if (props.HasMember("solid") && props["solid"].IsBool())
			outDef.properties.bSolid = props["solid"].GetBool();

		if (props.HasMember("fullcube") && props["fullcube"].IsBool())
			outDef.properties.bFullCube = props["fullcube"].GetBool();
	}

	if (root.HasMember("render") && root["render"].IsObject())
	{
		const auto& render = root["render"];
		if (render.HasMember("layer") && render["layer"].IsString())
		{
			_ParseRenderLayer(render["layer"].GetString(), outDef.render.layer);
		}

		if (render.HasMember("ambientOcclusion") && render["ambientOcclusion"].IsBool())
			outDef.render.bAmbientOcclusion = render["ambientOcclusion"].GetBool();
	}

	if (root.HasMember("collision") && root["collision"].IsObject())
	{
		const auto& collision = root["collision"];
		if (collision.HasMember("type") && collision["type"].IsString())
		{
			_ParseCollisionType(collision["type"].GetString(), outDef.collision.colType);
		}
	}

	if (root.HasMember("tags") && root["tags"].IsArray())
	{
		const auto& tags = root["tags"];
		outDef.tags.reserve(tags.Size());

		for (rapidjson::SizeType i = 0; i < tags.Size(); ++i)
		{
			if (!tags[i].IsString())
				continue;

			outDef.tags.push_back(tags[i].GetString());
		}
	}

	return true;
}

bool CBlockDefDB::_ResolveInheritance()
{
	for (size_t i = 0; i < m_vecBlockDefs.size(); ++i)
	{
		BlockDef& child = m_vecBlockDefs[i];
		if (!child.bLoaded)
			continue;

		if (child.parent.empty())
			continue;

		const BlockDef* pParent = FindBlockDef(child.parent.c_str());
		if (!pParent)
		{
			assert(false && "parent BlockDef not found");
			return false;
		}

		if (child.stateSource.empty())
			child.stateSource = pParent->stateSource;

		if (child.soundProfile.empty())
			child.soundProfile = pParent->soundProfile;

		if (!child.properties.bAir && pParent->properties.bAir)
			child.properties.bOpaque = pParent->properties.bOpaque;

		if (!child.properties.bSolid && pParent->properties.bSolid)
			child.properties.bSolid = pParent->properties.bSolid;

		if (!child.properties.bFullCube && pParent->properties.bFullCube)
			child.properties.bFullCube = pParent->properties.bFullCube;

		if (child.render.layer == BLOCK_RENDER_LAYER::OPAQUE_LAYER 
			&& pParent->render.layer != BLOCK_RENDER_LAYER::OPAQUE_LAYER)
		{
			child.render.layer = pParent->render.layer;
		}

		if (child.render.bAmbientOcclusion == true
			&& pParent->render.bAmbientOcclusion == false)
		{
			child.render.bAmbientOcclusion = false;
		}

		if (child.collision.colType == BLOCK_COLLISION_TYPE::NONE
			&& pParent->collision.colType == BLOCK_COLLISION_TYPE::NONE)
		{
			child.collision.colType = pParent->collision.colType;
		}

		if (child.tags.empty())
			child.tags = pParent->tags;
	}

	return true;
}

bool CBlockDefDB::_ParseRenderLayer(const char* strLayer, BLOCK_RENDER_LAYER& outLayer)
{
	if (!strLayer) return false;

	if (strcmp(strLayer, "opaque") == 0)		{ outLayer = BLOCK_RENDER_LAYER::OPAQUE_LAYER; return true; }
	if (strcmp(strLayer, "cutout") == 0)		{ outLayer = BLOCK_RENDER_LAYER::CUTOUT_LAYER; return true; }
	if (strcmp(strLayer, "translucent") == 0)	{ outLayer = BLOCK_RENDER_LAYER::TRANSLUCENT_LAYER; return true; }
	if (strcmp(strLayer, "invisible") == 0)		{ outLayer = BLOCK_RENDER_LAYER::INVISIBLE_LAYER; return true; }

	return false;
}

bool CBlockDefDB::_ParseCollisionType(const char* strType, BLOCK_COLLISION_TYPE& outType)
{
	if (!strType) return false;

	if (strcmp(strType, "none") == 0)		{ outType = BLOCK_COLLISION_TYPE::NONE; return true; }
	if (strcmp(strType, "full_cube") == 0)	{ outType = BLOCK_COLLISION_TYPE::FULL_CUBE; return true; }
	if (strcmp(strType, "custom") == 0)		{ outType = BLOCK_COLLISION_TYPE::CUSTOM; return true; }

	return false;
}