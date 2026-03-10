#pragma once
#include "CObject.h"

constexpr size_t MAX_OBJECT_COUNT = 100000ull;

class CScene;

class CObjectManager
{
public:
	CObjectManager();
	~CObjectManager();

	OBJECT_ID Add(const string& strName_, CScene* const pOwnScene_);
	void Destroy(const string& strName_);
	void Destroy(OBJECT_ID uObjectID_);

	void ProcessPeddingDestroy();

	CObject* Get(const string& strName_);
	const CObject* Get(const string& strName_) const;

	CObject* Get(OBJECT_ID uObjectID_);
	const CObject* Get(OBJECT_ID uObjectID_) const;

	template<typename Fn>
	void ForEachAliveEnabled(Fn&& fn) 
	{
		for (size_t i = 0; i < m_vecSparse.size(); ++i)
		{
			uint32_t denseIndex = m_vecSparse[i];
			CObject* pObj = m_vecObjects[denseIndex].get();
			if (pObj && pObj->GetAlive() && pObj->GetEnable()) {
				fn(*pObj);
			}
		}
	}

	vector<uint32_t> GetAllObject() { return m_vecSparse; }

private:
	void _RemoveObjectAtIndex(uint32_t uIndex_);

private:
	unordered_map<uint64_t, OBJECT_ID> m_hashIDMap;
	vector<uint32_t> m_vecSparse;
	vector<unique_ptr<CObject>> m_vecObjects;
	vector<OBJECT_ID> m_vecFreeIDs;
};