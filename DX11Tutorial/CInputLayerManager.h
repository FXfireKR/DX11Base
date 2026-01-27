#pragma once
#include "CInputLayer.h"

struct InputLayoutData
{
	VertexLayoutDesc layoutDesc;
	ShaderKey shaderKey;
	unique_ptr<CInputLayer> data = nullptr;

	bool operator==(const InputLayoutData& rhs) const {
		return layoutDesc == rhs.layoutDesc 
			&& shaderKey == rhs.shaderKey;
	}
};

class CInputLayerManager : public singleton<CInputLayerManager>
{
public:
	CInputLayerManager() = default;
	~CInputLayerManager() = default;

	uint32_t Create(const VertexLayoutDesc& layoutDesc_, const ShaderKey& shaderKey_);
	const CInputLayer* Get(uint32_t id_) const;

private:
	vector<InputLayoutData> m_vecInputLayoutData;
};