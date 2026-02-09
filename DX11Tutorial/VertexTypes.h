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