#pragma once
#include "Components.h"

class CScene;

class CObject
{
public:
	CObject() = default;
	~CObject() = default;

	void Init();
	void FixedUpdate(float fDelta);
	void Update(float fDelta);
	void LateUpdate(float fDelta);
	void Render();

	void Destroy();

public:
	template<typename T> T* AddComponent() {
		static_assert(is_base_of_v<CComponent, T>, "AddComponent template type must derive from CComponent");
		static_assert(std::is_same_v<decltype(this), CObject*>);

		constexpr uint64_t uType = static_cast<uint64_t>(T::GetStaticType());
		if (nullptr == this->m_arrComponents[uType]) {
			this->m_arrComponents[uType] = make_unique<T>();
			this->m_arrComponents[uType].get()->SetOwner(this);
			this->m_arrComponents[uType].get()->SetAlive(true);
			this->m_arrComponents[uType].get()->SetEnable(true);
			this->m_arrComponents[uType].get()->Init();
		}
		return static_cast<T*>(this->m_arrComponents[uType].get());
	}

	template<typename T> T* GetComponent() {
		static_assert(is_base_of_v<CComponent, T>, "GetComponent template type must derive from CComponent");

		constexpr uint64_t uType = static_cast<uint64_t>(T::GetStaticType());
		if (nullptr != m_arrComponents[uType]) {
			return static_cast<T*>(m_arrComponents[uType].get());
		}
		return nullptr;
	}

	template<typename T> const T* GetComponent() const {
		static_assert(is_base_of_v<CComponent, T>, "GetComponent template type must derive from CComponent");

		constexpr uint64_t uType = static_cast<uint64_t>(T::GetStaticType());
		if (nullptr != m_arrComponents[uType]) {
			return static_cast<T*>(m_arrComponents[uType].get());
		}
		return nullptr;
	}


	void AddChild(ObjectID uChildID_);

public:
	inline const ObjectID GetID() const { return m_uID; }
	inline const string& GetName() { return m_strName; }

	inline const ObjectID GetParentID() const { return m_uParentID; }
	inline bool HasParent() { return IsValidObject(m_uParentID); }

	inline const vector<ObjectID>& GetChildren() const { return m_vecChildren; }
	inline bool HasChildren() { return !m_vecChildren.empty(); }

	inline bool GetAlive() { return m_bAlive; }
	inline bool GetPeddingDestroy() { return m_bPeddingDestroy; }

	inline void SetEnable(bool bEnable_) { m_bEnable = bEnable_; }
	inline bool GetEnable() { return m_bEnable; }

	inline CScene* GetOwnScene() { return m_pOwnScene; }
	inline const CScene* GetOwnScene() const { return m_pOwnScene; }

private:
	bool m_bAlive = true; // 이 오브젝트는 사용 안함 이라는 뜻
	bool m_bEnable = true; // 활성/비활성 처리
	bool m_bPeddingDestroy = false; // 이 오브젝트는 Update 이후 제거된다는 의미

	string m_strName = "";
	ObjectID m_uID = INVALID_OBJECT_ID;
	ObjectID m_uParentID = INVALID_OBJECT_ID;
	vector<ObjectID> m_vecChildren;
	CScene* m_pOwnScene = nullptr;

	std::array<unique_ptr<CComponent>, COMPONENT_TYPE_MAX> m_arrComponents;

private:
	friend class CObjectManager;
};