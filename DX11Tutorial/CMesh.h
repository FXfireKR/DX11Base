#pragma once

struct SubMesh
{
	UINT uVertexStart;
	UINT uVertexCount;
	ComPtr<ID3D11Buffer> pVertexBuffer;
};

class CMesh
{
public:
	CMesh();
	~CMesh();

	void Draw();

private:
	vector<SubMesh> m_vecSubMesh;
};