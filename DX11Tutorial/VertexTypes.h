#pragma once
#include "VertexLayoutTypes.h"

using namespace DirectX;

struct VERTEX_POSITION
{
	XMFLOAT3 position;

	static VertexLayoutDesc GetLayout()
	{
		VertexLayoutDesc desc;
		desc.uStride = sizeof(VERTEX_POSITION);
		desc.vecElements = {
			{ VERTEX_SEMANTIC::POSITION, DXGI_FORMAT_R32G32B32_FLOAT },
		};

		return desc;
	}
};

struct VERTEX_POSITION_COLOR
{
	XMFLOAT3 position;
	XMFLOAT4 color;

	static VertexLayoutDesc GetLayout() 
	{
		VertexLayoutDesc desc;
		desc.uStride = sizeof(VERTEX_POSITION_COLOR);
		desc.vecElements = {
			{ VERTEX_SEMANTIC::POSITION, DXGI_FORMAT_R32G32B32_FLOAT },
			{ VERTEX_SEMANTIC::COLOR, DXGI_FORMAT_R32G32B32A32_FLOAT },
		};

        return desc;
	}
};

struct VERTEX_POSITION_UV
{
	XMFLOAT3 position;
	XMFLOAT2 uv;

	static VertexLayoutDesc GetLayout()
	{
		VertexLayoutDesc desc;
		desc.uStride = sizeof(VERTEX_POSITION_UV);
		desc.vecElements = {
			{ VERTEX_SEMANTIC::POSITION, DXGI_FORMAT_R32G32B32_FLOAT },
			{ VERTEX_SEMANTIC::TEXCOORD, DXGI_FORMAT_R32G32_FLOAT },
		};

		return desc;
	}
};

struct VERTEX_POSITION_UV_COLOR
{
	XMFLOAT3 position;
	XMFLOAT2 uv;
	XMFLOAT4 color;

	static VertexLayoutDesc GetLayout()
	{
		VertexLayoutDesc desc;
		desc.uStride = sizeof(VERTEX_POSITION_UV_COLOR);
		desc.vecElements = {
			{ VERTEX_SEMANTIC::POSITION, DXGI_FORMAT_R32G32B32_FLOAT },
			{ VERTEX_SEMANTIC::TEXCOORD, DXGI_FORMAT_R32G32_FLOAT },
			{ VERTEX_SEMANTIC::COLOR, DXGI_FORMAT_R32G32B32A32_FLOAT },
		};

		return desc;
	}
};

struct VERTEX_POSITION_UV_NORMAL
{
	XMFLOAT3 position;
	XMFLOAT2 uv;
	XMFLOAT3 normal;

	static VertexLayoutDesc GetLayout()
	{
		VertexLayoutDesc desc;
		desc.uStride = sizeof(VERTEX_POSITION_UV_NORMAL);
		desc.vecElements = {
			{ VERTEX_SEMANTIC::POSITION, DXGI_FORMAT_R32G32B32_FLOAT },
			{ VERTEX_SEMANTIC::TEXCOORD, DXGI_FORMAT_R32G32_FLOAT },
			{ VERTEX_SEMANTIC::NORMAL, DXGI_FORMAT_R32G32B32_FLOAT },
		};

		return desc;
	}
};

struct VERTEX_POSITION_NORMAL_UV_COLOR
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 uv;
	XMFLOAT4 color;

	static VertexLayoutDesc GetLayout()
	{
		VertexLayoutDesc desc;
		desc.uStride = sizeof(VERTEX_POSITION_NORMAL_UV_COLOR);
		desc.vecElements = {
			{ VERTEX_SEMANTIC::POSITION, DXGI_FORMAT_R32G32B32_FLOAT },
			{ VERTEX_SEMANTIC::NORMAL, DXGI_FORMAT_R32G32B32_FLOAT },
			{ VERTEX_SEMANTIC::TEXCOORD, DXGI_FORMAT_R32G32_FLOAT },
			{ VERTEX_SEMANTIC::COLOR, DXGI_FORMAT_R32G32B32A32_FLOAT },
		};

		return desc;
	}
};

struct VERTEX_CHUNK
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 uv;

	// rgb = block tint
	// a   = reserved
	XMFLOAT4 color;

	// x = BlockLight
	// y = SkyLight
	XMFLOAT2 light;

	static VertexLayoutDesc GetLayout()
	{
		VertexLayoutDesc desc;
		desc.uStride = sizeof(VERTEX_CHUNK);
		desc.vecElements =
		{
			{
				VERTEX_SEMANTIC::POSITION,
				DXGI_FORMAT_R32G32B32_FLOAT
			},
			{
				VERTEX_SEMANTIC::NORMAL,
				DXGI_FORMAT_R32G32B32_FLOAT
			},
			{
				VERTEX_SEMANTIC::TEXCOORD,
				DXGI_FORMAT_R32G32_FLOAT
			},
			{
				VERTEX_SEMANTIC::COLOR,
				DXGI_FORMAT_R32G32B32A32_FLOAT
			},

			// TEXCOORD1
			{
				VERTEX_SEMANTIC::TEXCOORD,
				DXGI_FORMAT_R32G32_FLOAT,
				D3D11_APPEND_ALIGNED_ELEMENT,
				1
			}
		};

		return desc;
	}
};