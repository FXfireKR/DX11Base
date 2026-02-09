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
	//VERTEX_POSITION_COLOR vert[4] =
	//{
	//	{ {0.5f, 0.5f, 0.0f}, {1,0,0,1} },
	//	{ {0.5f, -0.5f, 0.0f}, {0,1,0,1} },
	//	{ {-0.5f, -0.5f, 0.0f}, {0,0,1,1} } ,
	//	{ {-0.5f, 0.5f, 0.0f}, {0,0,1,1} } ,
	//};

	//D3D11_BUFFER_DESC desc{};
	//desc.ByteWidth = sizeof(vert);
	//desc.Usage = D3D11_USAGE_DEFAULT;
	//desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	//D3D11_SUBRESOURCE_DATA data{};
	//data.pSysMem = vert;

	//pDevice->CreateBuffer(&desc, &data, m_pVertexBuffer.GetAddressOf());
}

void CMesh::Bind(ID3D11DeviceContext* pContext)
{
	UINT stride = sizeof(VERTEX_POSITION_COLOR);
	UINT offset = 0;

	pContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	//pContext->IASetIndexBuffer()

	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void CMesh::Draw(ID3D11DeviceContext* pContext)
{
	pContext->Draw(3, 0);
}