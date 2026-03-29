#include "pch.h"
#include "CInventoryComponent.h"

void CInventoryComponent::Init()
{
	m_arrayHotBar.fill({ {0, 0}, 0 });
	m_iSelectedSlot = 0;

	{
		BlockPropHashMap props;
		BLOCK_ID blockID = BlockDB.FindBlockID("minecraft:glass");
		STATE_INDEX sidx;
		bool ok = BlockDB.EncodeStateIndex(blockID, props, sidx);
		assert(ok);

		m_arrayHotBar[0].block.blockID = blockID;
		m_arrayHotBar[0].block.stateIndex = sidx;
		m_arrayHotBar[0].count = 999;
	}

	{
		BlockPropHashMap props;
		BLOCK_ID blockID = BlockDB.FindBlockID("minecraft:torch");
		STATE_INDEX sidx;
		bool ok = BlockDB.EncodeStateIndex(blockID, props, sidx);
		assert(ok);

		m_arrayHotBar[1].block.blockID = blockID;
		m_arrayHotBar[1].block.stateIndex = sidx;
		m_arrayHotBar[1].count = 999;
	}

	{
		BlockPropHashMap props;
		BLOCK_ID blockID = BlockDB.FindBlockID("minecraft:oak_leaves");
		STATE_INDEX sidx;
		bool ok = BlockDB.EncodeStateIndex(blockID, props, sidx);
		assert(ok);

		m_arrayHotBar[2].block.blockID = blockID;
		m_arrayHotBar[2].block.stateIndex = sidx;
		m_arrayHotBar[2].count = 999;
	}

	{
		BlockPropHashMap props;
		BLOCK_ID blockID = BlockDB.FindBlockID("minecraft:cobblestone");
		STATE_INDEX sidx;
		bool ok = BlockDB.EncodeStateIndex(blockID, props, sidx);
		assert(ok);

		m_arrayHotBar[3].block.blockID = blockID;
		m_arrayHotBar[3].block.stateIndex = sidx;
		m_arrayHotBar[3].count = 999;
	}

	{
		BlockPropHashMap props;
		BLOCK_ID blockID = BlockDB.FindBlockID("minecraft:dirt");
		STATE_INDEX sidx;
		bool ok = BlockDB.EncodeStateIndex(blockID, props, sidx);
		assert(ok);

		m_arrayHotBar[4].block.blockID = blockID;
		m_arrayHotBar[4].block.stateIndex = sidx;
		m_arrayHotBar[4].count = 999;
	}

	{
		BlockPropHashMap props;
		BLOCK_ID blockID = BlockDB.FindBlockID("minecraft:oak_log");
		STATE_INDEX sidx;
		bool ok = BlockDB.EncodeStateIndex(blockID, props, sidx);
		assert(ok);

		m_arrayHotBar[5].block.blockID = blockID;
		m_arrayHotBar[5].block.stateIndex = sidx;
		m_arrayHotBar[5].count = 999;
	}

	{
		BlockPropHashMap props;
		BLOCK_ID blockID = BlockDB.FindBlockID("minecraft:crafting_table");
		STATE_INDEX sidx;
		bool ok = BlockDB.EncodeStateIndex(blockID, props, sidx);
		assert(ok);

		m_arrayHotBar[6].block.blockID = blockID;
		m_arrayHotBar[6].block.stateIndex = sidx;
		m_arrayHotBar[6].count = 999;
	}
}

void CInventoryComponent::Start()
{
}

int CInventoryComponent::GetSelectedSlotIndex() const
{
	return m_iSelectedSlot;
}

void CInventoryComponent::SetSelectedSlotIndex(int index)
{
	if (index < 0 || index >= HOTBAR_SIZE) return;
	m_iSelectedSlot = index;
}

const InventorySlot* CInventoryComponent::GetSelectedSlot() const
{
	return &m_arrayHotBar[m_iSelectedSlot];
}

InventorySlot* CInventoryComponent::GetSelectedSlotMutable()
{
	return &m_arrayHotBar[m_iSelectedSlot];
}

BlockCell CInventoryComponent::GetSelectedPlaceBlock() const
{
	const InventorySlot& slot = m_arrayHotBar[m_iSelectedSlot];
	if (slot.IsEmpty()) return { 0, 0 };
	return slot.block;
}

bool CInventoryComponent::TryConsumeSelectedOne()
{
	InventorySlot& slot = m_arrayHotBar[m_iSelectedSlot];
	if (slot.IsEmpty()) return false;

	if (slot.count > 0)
	{
		--slot.count;
		if (slot.count == 0)
		{
			slot.block = { 0, 0 };
		}
		return true;
	}
	return false;
}
