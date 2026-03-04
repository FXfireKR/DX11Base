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
	const CMesh* pLastMesh = nullptr;
	const CPipeline* pLastPipeline = nullptr;
	const CMaterial* pLastMaterial = nullptr;

	while (!m_queueRenderItem.empty())
	{
		const RenderItem& renderItem = m_queueRenderItem.front();
		if (true == _CheckValidToDraw(renderItem))
		{
			if (pLastPipeline != renderItem.pPipeline) {
				renderItem.pPipeline->Bind(pContext);
				pLastPipeline = renderItem.pPipeline;
			}

			if (pLastMesh != renderItem.pMesh) {
				renderItem.pMesh->Bind(pContext);
				pLastMesh = renderItem.pMesh;
			}

			if (nullptr != renderItem.pMaterial) {
				if (pLastMaterial != renderItem.pMaterial) {
					renderItem.pMaterial->Bind(pContext);
					pLastMaterial = renderItem.pMaterial;
				}
			}

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
	static_assert(sizeof(CB_ObjectData) % 16 == 0, "CB_ObjectData must be 16-byte aligned.");

	D3D11_MAPPED_SUBRESOURCE mapped{};
	pContext->Map(m_pCBObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, &objData, sizeof(CB_ObjectData));
	pContext->Unmap(m_pCBObject, 0);
}

bool CRenderFrame::_CheckValidToDraw(const RenderItem& renderItem)
{
	if (nullptr == renderItem.pPipeline) return false;
	if (nullptr == renderItem.pMesh) return false;
	//if (nullptr == renderItem.pMaterial) return false; // optional

	return true;
}
