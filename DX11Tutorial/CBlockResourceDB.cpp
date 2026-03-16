#include "pch.h"
#include "CBlockResourceDB.h"

void CBlockResourceDB::Initialize(const char* resourceRoot, ID3D11Device* pDevice)
{
	m_strRoot = resourceRoot;
	m_pDevice = pDevice;

	m_bInit = true;
	m_bLoadComplete = false;
}

bool CBlockResourceDB::Load()
{
	AtlasBuildInput a;
	a.textureKey = "minecraft:block/stone";
	a.filePath = m_strRoot + "assets/minecraft/textures/block/stone.png";

	AtlasBuildInput b;
	b.textureKey = "minecraft:block/dirt";
	b.filePath = m_strRoot + "assets/minecraft/textures/block/dirt.png";

	m_runtimeAtlasBuilder.Clear();
	m_runtimeAtlasBuilder.AddInput(a);
	m_runtimeAtlasBuilder.AddInput(b);

	bool ok = m_runtimeAtlasBuilder.Build(m_pDevice, m_runtimeAtlas);

	return ok;
}

void CBlockResourceDB::Clear()
{

}