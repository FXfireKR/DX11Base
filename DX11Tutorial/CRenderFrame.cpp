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
			if (nullptr != renderItem.pPipeline)renderItem.pPipeline->Bind(pContext);
			if (nullptr != renderItem.pMesh) renderItem.pMesh->Bind(pContext);
			if (nullptr != renderItem.pMaterial)renderItem.pMaterial->Bind(pContext);

			_UpdateConstantBuffer(pContext, { renderItem.world });

			//pContext->UpdateSubresource(m_pCBObject, 0, nullptr, &cb, 0, 0);
			pContext->VSSetConstantBuffers(1, 1, &m_pCBObject);

			// index, vertex 분리
			renderItem.pMesh->Draw(pContext);
		}
		m_queueRenderItem.pop();
	}
}

void CRenderFrame::_UpdateConstantBuffer(ID3D11DeviceContext* pContext, CB_ObjectData&& objData)
{
	D3D11_MAPPED_SUBRESOURCE mapped{};
	pContext->Map(m_pCBObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, &objData, sizeof(CB_ObjectData));
	pContext->Unmap(m_pCBObject, 0);
}