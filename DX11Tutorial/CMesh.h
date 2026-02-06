#pragma once

struct SubMesh
{
	UINT uVertexStart;
	UINT uVertexCount;
};

class CMesh
{
public:
	CMesh();
	~CMesh();

	void Draw();

private:
	vector<SubMesh> m_vecSubMesh;
	ComPtr<ID3D11Buffer> pVertexBuffer;
};