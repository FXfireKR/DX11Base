#pragma once
#include "CMesh.h"
#include "CPipeline.h"

//	class RenderFrame
// 
//	* 해당 클래스는 임의 개 존재할 수 있다.
//	* 인스턴스 하나당 프레임 하나를 담당하는 랜더 정보를 보유한다.
//	* 클래스 내부에서의 객체 제거, 게임 객체 생성은 일절 하지 않는다.

struct RenderItem
{
	CMesh* pMesh = nullptr;
	CPipeline* pPipeline = nullptr;
	XMFLOAT4X4 world;
};

class CRenderFrame
{
public:
	CRenderFrame() = default;
	~CRenderFrame() = default;

	void Submit(const RenderItem& renderItem);
	void Draw(ID3D11DeviceContext* pContext);
	
private:
	queue<RenderItem> m_queueRenderItem; // 일단 지금은 queue로 짠다.
}; 