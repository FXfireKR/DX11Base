#include "pch.h"
#include "CRenderFrame.h"

void CRenderFrame::Initialize(ID3D11Buffer& cbObject)
{
	m_pCBObject = &cbObject;
}

void CRenderFrame::Submit(const RenderItem& renderItem)
{
	m_queueRenderItem.push(renderItem);
}

void CRenderFrame::Draw(ID3D11DeviceContext* pContext)
{
	while (!m_queueRenderItem.empty())
	{
		const RenderItem& renderItem = m_queueRenderItem.front();
		{
			renderItem.pPipeline->Bind(pContext);
			renderItem.pMesh->Bind(pContext);

			CB_ObjectData cb;
			cb.world = renderItem.world;

			D3D11_MAPPED_SUBRESOURCE mapped{};
			pContext->Map(m_pCBObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
			memcpy(mapped.pData, &cb, sizeof(CB_ObjectData));
			pContext->Unmap(m_pCBObject, 0);

			//pContext->UpdateSubresource(m_pCBObject, 0, nullptr, &cb, 0, 0);
			pContext->VSSetConstantBuffers(1, 1, &m_pCBObject);

			// index, vertex 분리
			renderItem.pMesh->Draw(pContext);
		}
		m_queueRenderItem.pop();
	}
}