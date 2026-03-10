#include "pch.h"
#include "CBlockDB.h"

void CBlockDB::Initialize(const char* resourceRoot)
{
	m_strRoot = resourceRoot;
	m_bInit = true;
	m_bLoadComplete = false;

	m_blockDefDB.Initialize(resourceRoot);
	m_blockStateDB.Initialize(resourceRoot, &m_blockDefDB, &m_modelDB);
	m_modelDB.Initialize(resourceRoot);
}

bool CBlockDB::Load()
{
	if (!m_blockDefDB.Load())
		return false;

	m_blockStateDB.Load();

}

void CBlockDB::Clear()
{

}

const BlockDef* CBlockDB::GetBlockDef(BLOCK_ID blockID) const
{
	return m_blockDefDB.GetBlockDef(blockID);
}

BLOCK_ID CBlockDB::FindBlockID(const char* blockName) const
{
	return m_blockDefDB.FindBlockID(blockName);
}

const char* CBlockDB::FindBlockName(BLOCK_ID blockID) const
{
	return m_blockDefDB.FindBlockName(blockID);
}

bool CBlockDB::EncodeStateIndex(BLOCK_ID blockID, const BlockPropHashMap& props, STATE_INDEX& outStateIndex) const
{
	return m_blockStateDB.EncodeStateIndex(blockID, props, outStateIndex);
}

bool CBlockDB::GetAppliedModels(BLOCK_ID blockID, STATE_INDEX stateIndex, vector<AppliedModel>& outModels) const
{
	return m_blockStateDB.GetAppliedModels(blockID, stateIndex, outModels);
}

const BakedModel* CBlockDB::FindBakedModel(const char* modelKey) const
{
	return m_modelDB.FindBakedModel(modelKey);
}

const BakedModel* CBlockDB::FindBakedModel(uint64_t modelHash) const
{
	return m_modelDB.FindBakedModel(modelHash);
}

const BakedModel* CBlockDB::GetBakedModel(BLOCK_ID blockID, STATE_INDEX stateIndex) const
{
	vector<AppliedModel> vecModels;
	if (!m_blockStateDB.GetAppliedModels(blockID, stateIndex, vecModels))
		return nullptr;

	if (vecModels.empty())
		return nullptr;

	return m_modelDB.FindBakedModel(vecModels[0].modelKey.c_str());
}

bool CBlockDB::_ValidateLinks() const
{
	BLOCK_ID airID = m_blockDefDB.FindBlockID("minecraft:air");
	if (airID != 0)
	{
		assert(false && "minecraft:air must be block id 0");
		return false;
	}

	const auto& vecDefs = m_blockDefDB.GetBlockDefs();
	for (const BlockDef& def : vecDefs)
	{
		if (!def.bLoaded) continue;
		if (def.properties.bAir) continue;
		if (def.stateSource.empty())
		{
#ifdef _DEBUG
			cout << "[BlockDB] stateSource empty : " << def.name << "\n";
#endif // _DEBUG
		}
	}

	return true;
}
