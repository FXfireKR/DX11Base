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

		renderItem.pPipeline->Bind(pContext);

		//renderItem.pMesh->

		m_queueRenderItem.pop();
	}
}