#pragma once

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