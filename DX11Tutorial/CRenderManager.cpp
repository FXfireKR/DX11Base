#include "pch.h"
#include "CRenderManager.h"

void CRenderManager::Initialize(size_t uMaxRenderFrame_)
{
	m_uMaxRenderFrame = uMaxRenderFrame_;
}

void CRenderManager::SetRenderFrame(shared_ptr<CRenderFrame> newRenderFrame_)
{
	m_queueRenderFrames.push(newRenderFrame_);
}