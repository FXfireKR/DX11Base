#pragma once
#include "ChunkTypes.h"

class IBlockAccessor
{
public:
	IBlockAccessor() = default;
	virtual ~IBlockAccessor() = default;

	virtual BlockCell GetBlockCell(int wx, int wy, int wz) const PURE;
	virtual BlockCell GetBlockCell(DirectX::XMINT3 w) const PURE;
	virtual bool IsSolid(const BlockCell& cell) const PURE;
};

struct BlockRaycastOptions
{
	bool bCheckStartCell = true;
	bool bTreatFluidAsSolid = false;
	bool bTreatReplaceableAsSolid = false;
};