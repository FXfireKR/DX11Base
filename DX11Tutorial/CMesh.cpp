#include "pch.h"
#include "CMesh.h"

void CMesh::CreateTriangle(ID3D11Device* pDevice)
{
	VERTEX_POSITION_COLOR vert[3] =
	{
		{ {-0.5f, -0.5f, 0.0f}, {0,0,1,1} } ,
		{ {0.0f, 0.5f, 0.0f}, {1,0,1,1} },
		{ {0.5f, -0.5f, 0.0f}, {0,1,0,1} },
	};

	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = sizeof(vert);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data{};
	data.pSysMem = vert;

	pDevice->CreateBuffer(&desc, &data, m_pVertexBuffer.GetAddressOf());
}

void CMesh::CreateQuad(ID3D11Device* pDevice)
{
	VERTEX_POSITION_UV vert[4] =
	{
		{ {-0.5f, 0.5f, 0.0f}, {0.f, 0.f} },
		{ {0.5f, 0.5f, 0.0f}, {1.f, 0.f} },
		{ {-0.5f, -0.5f, 0.0f}, {0.f, 1.f} } ,
		{ {0.5f, -0.5f, 0.0f}, {1.f, 1.f} } ,
	};

	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = sizeof(vert);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data{};
	data.pSysMem = vert;

	pDevice->CreateBuffer(&desc, &data, m_pVertexBuffer.GetAddressOf());

	uint32_t indices[6] =
	{
		0, 1, 2,
		2, 1, 3
	};

	D3D11_BUFFER_DESC descIndex{};
	descIndex.ByteWidth = sizeof(indices);
	descIndex.Usage = D3D11_USAGE_DEFAULT;
	descIndex.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA dataIndex{};
	dataIndex.pSysMem = indices;

	pDevice->CreateBuffer(&descIndex, &dataIndex, m_pIndexBuffer.GetAddressOf());
}

void CMesh::Bind(ID3D11DeviceContext* pContext)
{
	UINT stride = sizeof(VERTEX_POSITION_UV);
	UINT offset = 0;

	pContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);

	if (m_pIndexBuffer) {
		pContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}

	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void CMesh::Draw(ID3D11DeviceContext* pContext)
{
	if (!m_pIndexBuffer) {
		pContext->Draw(3, 0);
	}
	else {
		pContext->DrawIndexed(6, 0, 0);
	}
}