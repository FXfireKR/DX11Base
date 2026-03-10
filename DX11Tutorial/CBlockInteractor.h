#pragma once
#include "CComponentBase.h"
#include "BlockRaycastTypes.h"

class CWorld;
class CInventoryComponent;
class CTransform;
class CObject;

class CBlockInteractor : public CComponentBase<CBlockInteractor, COMPONENT_TYPE::BLOCKINTERACTOR>
{
public:
	void Init() override;
	void Start() override;
	void LateUpdate(float fDelta) override;

public:
	void SetWorld(CWorld* pWorld);
	void SetCameraTransform(CTransform* pCamTransform);
	void SetHighlightObject(CObject* pHighlightObject);

	void RequestPlace();
	void RequestBreak();

public:
	const BlockHitResult& GetBlockHit() const;

private:
	void _UpdateRaycast();
	void _UpdateHighlight();
	void _ConsumeRequests();

	bool _TryPlaceBlock();
	bool _TryBreakBlock();

	void _MakeCenterRay(OUT XMFLOAT3& outOrigin, XMFLOAT3& outDir) const;

private:
	CWorld* m_pWorld = nullptr;
	CInventoryComponent* m_pInventory = nullptr;
	CTransform* m_pCamTransform = nullptr;
	CObject* m_pHighlightObject = nullptr;

	BlockHitResult m_hitResult{};

	bool m_bPlaceRequested = false;
	bool m_bBreakRequested = false;

	float m_fReach = 15.f;
};