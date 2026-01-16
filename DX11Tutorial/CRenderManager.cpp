#include "pch.h"
#include "CRenderManager.h"

CRenderManager::CRenderManager(const size_t& uMaxRenderFrame_)
	: m_uMaxRenderFrame(uMaxRenderFrame_)
{
}

CRenderManager::~CRenderManager()
{
}

void CRenderManager::SetRenderFrame(shared_ptr<CRenderFrame> newRenderFrame_)
{
	m_queueRenderFrames.push(newRenderFrame_);
}