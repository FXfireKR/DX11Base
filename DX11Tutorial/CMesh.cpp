#include "pch.h"
#include "CMesh.h"

void CMesh::Draw(ID3D11DeviceContext* pContext)
{
	UINT stride = 0;
	UINT offset = 0;

	pContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	//pContext->IASetIndexBuffer()

	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	pContext->Draw(0, 0);
}