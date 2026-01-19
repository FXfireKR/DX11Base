#pragma once
#include "CObject.h"

constexpr size_t MAX_OBJECT_COUNT = 100000ull;

class CScene;

class CObjectManager
{
public:
	CObjectManager();
	~CObjectManager();

	ObjectID Add(const string& strName_, CScene* const pOwnScene_);
	void Destroy(const string& strName_);
	void Destroy(ObjectID uObjectID_);

	void ProcessPeddingDestroy();

	CObject* Get(const string& strName_);
	CObject* Get(ObjectID uObjectID_);
	const CObject* Get(const string& strName_) const;
	const CObject* Get(ObjectID uObjectID_) const;

private:
	void _RemoveObjectAtIndex(uint32_t uIndex_);

private:
	unordered_map<uint64_t, ObjectID> m_hashIDMap;
	vector<uint32_t> m_vecSparse;
	vector<unique_ptr<CObject>> m_vecObjects;
	vector<ObjectID> m_vecFreeIDs;
};