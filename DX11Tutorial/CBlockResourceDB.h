#pragma once
#include "singleton.h"

#include "CRuntimeAtlas.h"
#include "CRuntimeAtlasBuilder.h"


class CBlockResourceDB : public singleton<CBlockResourceDB>
{
public:
	void Initialize(const char* resourceRoot, ID3D11Device* pDevice);
	bool Load();
	void Clear();

	



private:
	ID3D11Device* m_pDevice;

	string m_strRoot;
	bool m_bInit = false;
	bool m_bLoadComplete = false;

	CRuntimeAtlasBuilder m_runtimeAtlasBuilder;
	CRuntimeAtlas m_runtimeAtlas;
};