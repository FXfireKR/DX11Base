#pragma once
#include "singleton.h"

#include "CModelDB.h"
#include "CBlockStateDB.h"
#include "CBlockDefDB.h"
#include "IBlockAccessor.hpp"

class CBlockDB : public singleton<CBlockDB>
{
public:
	void Initialize(const char* resourceRoot);
	bool Load();
	void Clear();

	const BlockDef* GetBlockDef(BLOCK_ID blockID) const;
	BLOCK_ID FindBlockID(const char* blockName) const;
	const char* FindBlockName(BLOCK_ID blockID) const;


	bool EncodeStateIndex(BLOCK_ID blockID, const BlockPropHashMap& props, STATE_INDEX& outStateIndex) const;
	bool GetAppliedModels(BLOCK_ID blockID, STATE_INDEX stateIndex, vector<AppliedModel>& outModels) const;

	const BakedModel* FindBakedModel(const char* modelKey) const;
	const BakedModel* FindBakedModel(uint64_t modelHash) const;
	const BakedModel* GetBakedModel(BLOCK_ID blockID, STATE_INDEX stateIndex) const;

private:
	bool _ValidateLinks() const;

private:
	string m_strRoot;
	bool m_bInit = false;
	bool m_bLoadComplete = false;

	CBlockDefDB m_blockDefDB;
	CBlockStateDB m_blockStateDB;
	CModelDB m_modelDB;
};