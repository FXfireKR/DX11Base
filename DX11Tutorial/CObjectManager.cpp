#include "pch.h"
#include "CObjectManager.h"

CObjectManager::CObjectManager()
{
	m_vecObjects.reserve(MAX_OBJECT_COUNT);
}

CObjectManager::~CObjectManager()
{
}

ObjectID CObjectManager::Add(const string& strName_, CScene* const pOwnScene_)
{
	uint64_t strHash = fnv1a_64(strName_);
	ObjectID uObjectID = INVALID_OBJECT_ID;

	if (true == m_hashIDMap.contains(strHash))
	{
		uObjectID = m_hashIDMap.find(strHash)->second;
	}
	else // (false == m_hashIDMap.contains(strHash))
	{
		if (false == m_vecFreeIDs.empty())
		{
			uObjectID = m_vecFreeIDs.back();
			m_vecFreeIDs.pop_back();
		}
		else // (true == m_vecFreeIDs.empty())
		{
			uObjectID = static_cast<ObjectID>(m_vecSparse.size());
			m_vecSparse.push_back(0); // insert dummy-value
		}
		m_hashIDMap.insert(make_pair(strHash, uObjectID));

		// setting new data
		auto newObject = make_unique<CObject>();
		newObject->m_strName = strName_;
		newObject->m_uID = uObjectID;
		newObject->m_pOwnScene = pOwnScene_;

		uint32_t uDenseIndex = static_cast<uint32_t>(m_vecObjects.size());
		m_vecObjects.push_back(std::move(newObject));
		m_vecSparse[uObjectID] = uDenseIndex;
	}
	return uObjectID;
}

void CObjectManager::Destroy(const string& strName_)
{
	uint64_t strHash = fnv1a_64(strName_);
	auto iter = m_hashIDMap.find(strHash);

	if (iter == m_hashIDMap.end()) return;

	ObjectID foundID = iter->second;
	Destroy(foundID);
}

void CObjectManager::Destroy(ObjectID uObjectID_)
{
	if (m_vecObjects.size() <= uObjectID_) return;

	if (nullptr != m_vecObjects[uObjectID_]) {
		m_vecObjects[uObjectID_]->Destroy();
	}
}

void CObjectManager::ProcessPeddingDestroy()
{
	for (int i = static_cast<int>(m_vecObjects.size() - 1); i >= 0; --i) 
	{
		if (true == m_vecObjects[i]->GetPeddingDestroy()) 
		{
			_RemoveObjectAtIndex(i);
		}
	}
}

void CObjectManager::_RemoveObjectAtIndex(uint32_t uIndex_)
{
	ObjectID removedID = m_vecObjects[uIndex_]->GetID();
	uint32_t uLastIndex = static_cast<uint32_t>(m_vecObjects.size() - 1);
	if (uIndex_ != uLastIndex)
	{
		std::swap(m_vecObjects[uIndex_], m_vecObjects[uLastIndex]);

		ObjectID movedID = m_vecObjects[uIndex_]->GetID();
		m_vecSparse[movedID] = uIndex_;
	}
	m_vecObjects.pop_back();
	m_vecFreeIDs.push_back(removedID);
}

const CObject* CObjectManager::Get(const string& strName_) const
{
	uint64_t strHash = fnv1a_64(strName_);
	auto iter = m_hashIDMap.find(strHash);
	return Get(iter->second);
}

const CObject* CObjectManager::Get(ObjectID uObjectID_) const
{
	if (uObjectID_ >= m_vecSparse.size()) return nullptr;

	uint32_t uIndex = m_vecSparse[uObjectID_];
	return m_vecObjects[uIndex].get();
}
