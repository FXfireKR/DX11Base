#pragma once
#include "CInputLayer.h"

class CInputLayerManager : public singleton<CInputLayerManager>
{
public:
	CInputLayerManager();
	~CInputLayerManager();



private:
	unordered_map<VertexLayoutDesc, unique_ptr<CInputLayer>> m_mapInputLayout;
};

