#include "pch.h"
#include "CRenderFrame.h"

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
			//renderItem. // constant buffer update

			renderItem.pMesh->Bind(pContext);

			// index, vertex 분리
			renderItem.pMesh->Draw(pContext);
		}
		m_queueRenderItem.pop();
	}
}