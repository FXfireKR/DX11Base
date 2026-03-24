#include "pch.h"
#include "CBlockInteractor.h"
#include "CWorld.h"
#include "CInventoryComponent.h"
#include "CObject.h"
#include "CTransform.h"

void CBlockInteractor::Init()
{
	m_hitResult = {};
	m_fReach = 7.f;

	m_bBreakHeld = false;
	m_bMining = false;
	m_miningBlock = m_hitResult.block;
	m_miningCell = m_hitResult.cell;
	m_fBreakAccum = 0.f;
	m_fBreakRequired = 0.25f;
	m_fHitFxCoolDown = 0.f;

	m_bPlaceRequested = false;
	m_bBreakRequested = false;
}

void CBlockInteractor::Start()
{
	m_pInventory = m_pOwner->GetComponent<CInventoryComponent>();
}

void CBlockInteractor::LateUpdate(float fDelta)
{
	if (nullptr == m_pWorld || nullptr == m_pCamTransform) 
		return;

	_UpdateRaycast();
	_UpdateHighlight();
	_UpdateMining(fDelta);
	_ConsumeRequests();
}


void CBlockInteractor::SetBreakHeld(bool bHeld)
{
	if (m_bBreakHeld == bHeld) return;

	m_bBreakHeld = bHeld;

	if (!m_bBreakHeld)
	{
		_ResetMining();
	}
}

void CBlockInteractor::RequestPlace()
{
	m_bPlaceRequested = true;
}

void CBlockInteractor::RequestBreak()
{
	m_bBreakRequested = true;
}

inline void CBlockInteractor::_UpdateRaycast()
{
	XMFLOAT3 rayOrigin{};
	XMFLOAT3 rayDir{};

	_MakeCenterRay(rayOrigin, rayDir);

	m_pWorld->RaycastBlock(rayOrigin, rayDir, m_fReach, m_hitResult);


	dbg.ClearBlockHit();
	if (m_hitResult.bHit)
	{
		dbg.SetBlockHit(m_hitResult.block, m_hitResult.normal, m_hitResult.cell.blockID, m_hitResult.cell.stateIndex);
	}
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
	pTransform->BuildWorldMatrix();
}

void CBlockInteractor::_ConsumeRequests()
{
	//if (m_bBreakRequested)
	//{
	//	_TryBreakBlock();
	//}

	if (m_bPlaceRequested)
	{
		_TryPlaceBlock();
	}

	//m_bBreakRequested = false;
	m_bPlaceRequested = false;
}

void CBlockInteractor::_UpdateMining(float fDelta)
{
	if (!m_bBreakHeld || !m_hitResult.bHit || m_hitResult.cell.IsAir())
	{
		_ResetMining();
		return;
	}

	if (!m_bMining || !_IsSameMiningTarget(m_hitResult))
	{
		m_bMining = true;
		m_miningBlock = m_hitResult.block;
		m_miningNormal = m_hitResult.normal;
		m_miningCell = m_hitResult.cell;

		m_fBreakAccum = 0.f;
		m_fBreakRequired = _CalcBreakRequired(m_miningCell);

		if (m_fBreakRequired < 0.f)
		{
			_ResetMining();
			return;
		}
	}

	m_fBreakAccum += fDelta;
	m_fHitFxCoolDown -= fDelta;

	if (m_fBreakAccum < m_fBreakRequired)
		return;

	const XMINT3 breakPos = m_miningBlock;
	const XMINT3 breakNormal = m_miningNormal;
	const BlockCell breakCell = m_miningCell;

	const bool bBroken = m_pWorld->TryBreakBlock(breakPos.x, breakPos.y, breakPos.z);

	_ResetMining();

	if (!bBroken)
		return;

	if (m_pParticle)
	{
		m_pParticle->SpawnBreakBurst(breakPos, breakCell, breakNormal);
	}
}

void CBlockInteractor::_ResetMining()
{
	m_bMining = false;

	m_miningBlock = {};
	m_miningNormal = {};
	m_miningCell = {};

	m_fBreakAccum = 0.f;
	m_fBreakRequired = 0.0f;
	m_fHitFxCoolDown = 0.f;
}

bool CBlockInteractor::_IsSameMiningTarget(const BlockHitResult& hit) const
{
	if (!hit.bHit) 
		return false;

	return (hit.block == m_miningBlock);
}

float CBlockInteractor::_CalcBreakRequired(const BlockCell& cell) const
{
	if (cell.IsAir())
		return 0.0f;

	return 0.25f;
}

bool CBlockInteractor::_TryPlaceBlock()
{
	if (!m_hitResult.bHit) 
		return false;

	const XMINT3 placePos = m_hitResult.prev;
	const BlockCell placeCell = m_pInventory->GetSelectedPlaceBlock();

	if (placeCell.IsAir()) 
		return false;

	return m_pWorld->TryPlaceBlock(placePos.x, placePos.y, placePos.z, placeCell);
}

bool CBlockInteractor::_TryBreakBlock()
{
	if (!m_hitResult.bHit) 
		return false;

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