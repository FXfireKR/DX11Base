#pragma once

class CInputLayer
{
public:
	CInputLayer(const CInputLayer&) = delete;
	CInputLayer& operator=(const CInputLayer&) = delete;

	CInputLayer();
	~CInputLayer();

	HRESULT Create(const VertexLayoutDesc& layoutDesc_, const void* pShaderByteCode_, size_t pShaderBufferSize_);

public:
	inline ID3D11InputLayout* GetInputLayout() const { return m_pInputLayout.Get(); }

private:
	ComPtr<ID3D11InputLayout> m_pInputLayout = nullptr;
};