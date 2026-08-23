#pragma once
#include "CChunkSection.h"

enum class EChunkResidency : uint8_t
{
	ACTIVE = 0,
	RESIDENT,
};

class CChunkColumn
{
public:
	CChunkColumn() = default;
	~CChunkColumn() = default;

	void Initialize(int cx, int cz);

	void ResetSection(int sy);
	void ResetBlockLightSection(int sy);

	CChunkSection* GetSection(int sy);
	const CChunkSection* GetSection(int sy) const;
	CChunkSection* EnsureSection(int sy);

	CChunkLightSection* GetBlockLightSection(int sy);
	const CChunkLightSection* GetBlockLightSection(int sy) const;
	CChunkLightSection* EnsureBlockLightSection(int sy);

public:
	inline const ChunkCoord& GetCoord() const { return m_coord; }

	inline EChunkResidency GetResidency() const { return m_eResidency; }
	inline bool IsActive() const { return m_eResidency == EChunkResidency::ACTIVE; }
	inline bool IsResident() const { return m_eResidency == EChunkResidency::RESIDENT; }

	inline void SetResidency(EChunkResidency eResidency) { m_eResidency = eResidency; }

	inline bool IsGenerated() const { return m_bGenerated; }
	inline void SetGenerated(bool bGenerated) { m_bGenerated = bGenerated; }

	inline bool IsModified() const { return m_bModified; }
	inline void SetModified(bool bModified) { m_bModified = bModified; }

	inline uint64_t GetLastAccessTick() const { return m_uLastAccessTick; }
	inline void SetLastAccessTick(uint64_t tick) { m_uLastAccessTick = tick; }

	inline int GetSkyOccluderY(int lx, int lz) const { return m_skyOccluderY[lz * CHUNK_SIZE_X + lx]; }
	inline void SetSkyOccluderY(int lx, int lz, int y) { m_skyOccluderY[lz * CHUNK_SIZE_X + lx] = static_cast<int16_t>(y); }


private:
	array<unique_ptr<CChunkSection>, CHUNK_SECTION_COUNT> m_sections;
	array<unique_ptr<CChunkLightSection>, CHUNK_SECTION_COUNT> m_blockLightSections;
	array<int16_t, CHUNK_SIZE_X* CHUNK_SIZE_Z> m_skyOccluderY{};


	ChunkCoord m_coord{};
	EChunkResidency m_eResidency = EChunkResidency::RESIDENT;
	bool m_bGenerated = false;
	bool m_bModified = false;
	uint64_t m_uLastAccessTick = 0;
};