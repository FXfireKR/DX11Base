#pragma once

struct SubMesh
{
	UINT uVertexStart;
	UINT uVertexCount;
};

class CMesh
{
public:
	CMesh() = default;
	~CMesh() = default;

	void CreateTriangle(ID3D11Device* pDevice);
	void CreateQuad(ID3D11Device* pDevice);

	void Bind(ID3D11DeviceContext* pContext) const;
	void Draw(ID3D11DeviceContext* pContext) const;

public:
	inline vector<SubMesh>& GetSubMesh() { return m_vecSubMesh; }
	inline ID3D11Buffer* GetVertexBuffer() const { return m_pVertexBuffer.Get(); }
	
private:
	vector<SubMesh> m_vecSubMesh;

	ComPtr<ID3D11Buffer> m_pVertexBuffer;
	ComPtr<ID3D11Buffer> m_pIndexBuffer;
};