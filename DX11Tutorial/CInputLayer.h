#pragma once

class CInputLayer
{
public:
	CInputLayer();
	~CInputLayer();

	HRESULT Create(const VertexLayoutDesc& layoutDesc_, const void* pShaderByteCode_, size_t pShaderBufferSize_);

	inline ID3D11InputLayout* GetInputLayout() const { return m_pInputLayout.Get(); }

private:
	ComPtr<ID3D11InputLayout> m_pInputLayout = nullptr;
};