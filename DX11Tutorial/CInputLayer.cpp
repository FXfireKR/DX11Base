#include "pch.h"
#include "CInputLayer.h"

CInputLayer::CInputLayer()
{
}

CInputLayer::~CInputLayer()
{
}

HRESULT CInputLayer::Create(const VertexLayoutDesc& layoutDesc_, const void* pShaderByteCode_, size_t pShaderBufferSize_)
{
	const uint32_t uElemSize = static_cast<uint32_t>(layoutDesc_.vecElements.size());
	vector<D3D11_INPUT_ELEMENT_DESC> layout(uElemSize);

	for (size_t i = 0; i < layoutDesc_.vecElements.size(); ++i) {
		const auto& elem = layoutDesc_.vecElements[i];

		layout[i].SemanticName = GetVertexSemanticString(elem.eSemantic);
		layout[i].SemanticIndex = elem.uSemanticIndex;
		layout[i].Format = elem.dxgiFormat;
		layout[i].InputSlot = elem.uInputSlot;
		layout[i].AlignedByteOffset = elem.uOffset;
		layout[i].InputSlotClass = elem.eInputSlotClass;
		layout[i].InstanceDataStepRate = elem.uInstanceDataStepRate;
	}

	return g_Device->CreateInputLayout(layout.data(), uElemSize, pShaderByteCode_, pShaderBufferSize_, m_pInputLayout.GetAddressOf());
}