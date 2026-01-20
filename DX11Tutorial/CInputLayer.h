#pragma once

class CInputLayer
{
public:
	CInputLayer();
	~CInputLayer();

	inline ID3D11InputLayout* GetInputLayout() const { return m_pInputLayout.Get(); }

private:
	ComPtr<ID3D11InputLayout> m_pInputLayout = nullptr;
};