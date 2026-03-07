#include "pch.h"
#include "CBlockInteractor.h"
#include "CWorld.h"
#include "CInventoryComponent.h"
#include "CObject.h"
#include "CTransform.h"

void CBlockInteractor::Init()
{
	m_hitResult = {};

	m_bPlaceRequested = false;
	m_bBreakRequested = false;

	m_fReach = 15.f;
}

void CBlockInteractor::Start()
{
	m_pInventory = m_pOwner->GetComponent<CInventoryComponent>();
}

void CBlockInteractor::LateUpdate(float fDelta)
{
	if (nullptr == m_pWorld || nullptr == m_pCamTransform) return;

	_UpdateRaycast();
	_UpdateHighlight();
	_ConsumeRequests();
}

void CBlockInteractor::SetWorld(CWorld* pWorld)
{
	m_pWorld = pWorld;
}

void CBlockInteractor::SetCameraTransform(CTransform* pCamTransform)
{
	m_pCamTransform = pCamTransform;
}

void CBlockInteractor::SetHighlightObject(CObject* pHighlightObject)
{
	m_pHighlightObject = pHighlightObject;
}

void CBlockInteractor::RequestPlace()
{
	m_bPlaceRequested = true;
}

void CBlockInteractor::RequestBreak()
{
	m_bBreakRequested = true;
}

const BlockHitResult& CBlockInteractor::GetBlockHit() const
{
	return m_hitResult;
}

inline void CBlockInteractor::_UpdateRaycast()
{
	XMFLOAT3 rayOrigin{};
	XMFLOAT3 rayDir{};

	_MakeCenterRay(rayOrigin, rayDir);

	m_pWorld->RaycastBlock(rayOrigin, rayDir, m_fReach, m_hitResult);
}

void CBlockInteractor::_UpdateHighlight()
{
	if (nullptr == m_pHighlightObject) return;

	if (!m_hitResult.bHit)
	{
		m_pHighlightObject->SetEnable(false);
		return;
	}

	m_pHighlightObject->SetEnable(true);

	CTransform* pTransform = m_pHighlightObject->GetComponent<CTransform>();
	if (nullptr == pTransform) return;

	pTransform->SetLocalTrans({
		static_cast<float>(m_hitResult.block.x) - 0.001f,
		static_cast<float>(m_hitResult.block.y) - 0.001f,
		static_cast<float>(m_hitResult.block.z) - 0.001f
	});

	pTransform->SetLocalScale({ 1.005f, 1.005f, 1.005f });
}

void CBlockInteractor::_ConsumeRequests()
{
	if (m_bBreakRequested)
	{
		_TryBreakBlock();
	}

	if (m_bPlaceRequested)
	{
		_TryPlaceBlock();
	}

	m_bBreakRequested = false;
	m_bPlaceRequested = false;
}

bool CBlockInteractor::_TryPlaceBlock()
{
	if (!m_hitResult.bHit) return false;

	const XMINT3 placePos = m_hitResult.prev;
	const BlockCell placeCell = m_pInventory->GetSelectedPlaceBlock();

	if (placeCell.IsAir()) return false;
	return m_pWorld->TryPlaceBlock(placePos.x, placePos.y, placePos.z, placeCell);
}

bool CBlockInteractor::_TryBreakBlock()
{
	if (!m_hitResult.bHit) return false;

	const XMINT3 placePos = m_hitResult.block;
	return m_pWorld->TryBreakBlock(placePos.x, placePos.y, placePos.z);
}

void CBlockInteractor::_MakeCenterRay(OUT XMFLOAT3& outOrigin, XMFLOAT3& outDir) const
{
	outOrigin = m_pCamTransform->GetWorldTrans();

	XMVECTOR forward = XMVector3TransformNormal(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_pCamTransform->GetWorldMatrix());
	forward = XMVector3Normalize(forward);
	XMStoreFloat3(&outDir, forward);
}