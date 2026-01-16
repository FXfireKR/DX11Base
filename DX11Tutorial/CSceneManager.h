#pragma once
#include "SceneHeader.h"
#include "singleton.h"

using SceneFactory = std::function<unique_ptr<CScene>()>;

class CSceneManager : public singleton<CSceneManager>
{
public:
	CSceneManager();
	~CSceneManager();

	void Register(SCENE_TYPE eType_, SceneFactory factory_);

	void Create(SCENE_TYPE eType_);
	void Remove(SCENE_TYPE eType_);

private:
	unordered_map<SCENE_TYPE, SceneFactory> m_mapFactories;
	array<unique_ptr<CScene>, SCENE_MAX_SIZE> m_arrayScene; // CScene의 own은 여기
};