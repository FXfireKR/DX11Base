#pragma once
#include "CMesh.h"
#include "CPipeline.h"
#include "CMaterial.h"

//	class RenderFrame
// 
//	* 해당 클래스는 임의 개 존재할 수 있다.
//	* 인스턴스 하나당 프레임 하나를 담당하는 랜더 정보를 보유한다.
//	* 클래스 내부에서의 객체 제거, 게임 객체 생성은 일절 하지 않는다.

struct CB_ObjectData
{
	XMFLOAT4X4 world;
};

struct RenderItem
{
	CMesh* pMesh = nullptr;
	CPipeline* pPipeline = nullptr;
	CMaterial* pMaterial = nullptr;
	XMFLOAT4X4 world;
};

class CRenderFrame
{
public:
	CRenderFrame() = default;
	~CRenderFrame() = default;

	void Initialize(ID3D11Buffer& cbObject);
	void Submit(const RenderItem& renderItem);
	void Draw(ID3D11DeviceContext* pContext);

private:
	void _UpdateConstantBuffer(ID3D11DeviceContext* pContext, CB_ObjectData&& objData);
	
private:
	queue<RenderItem> m_queueRenderItem; // 일단 지금은 queue로 짠다.
	ID3D11Buffer* m_pCBObject = nullptr; // b1
}; 