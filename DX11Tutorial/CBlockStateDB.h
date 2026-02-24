#pragma once
#include "singleton.h"

/*
* VALUE가 INDEX로 저장되므로, 단계를 나눈다.
* Pass_1 : variants key 문자열들을 모아서 peding 시킴
* Finalize : 정렬해서 Hash-map 구축, stride, totalState 계산
* Pass_2 : variants key 문자열들을 기반으로 컴파일된 VariantRule 생성
*/

class CBlockStateDB : public singleton<CBlockStateDB>
{
public:
	void Initialize(const char* path);
	bool GetAppliedModels(IN BLOCK_ID blockID, STATE_INDEX stateIndex, OUT vector<AppliedModel>& vecAppliedModels);
	bool EncodeStateIndex(IN BLOCK_ID blockID, const BlockPropHashMap& props, OUT STATE_INDEX& stateIndex) const;

private:
	bool _ScanBlockStateFiles(vector<filesystem::path>& outFiles) const;
	bool _Pass1_CollectDomains(const vector<filesystem::path>& files);
	bool _FinalizeAllBockTypes();
	bool _Pass2_CompileRules(const vector<filesystem::path>& files);

	bool _ReadJson(const std::filesystem::path& path, rapidjson::Document& outDoc) const;
	bool _ReadVariants_Pass1(BLOCK_ID blockID, const rapidjson::Value& variants);
	bool _ReadVariants_Pass2(BLOCK_ID blockID, const rapidjson::Value& variants);

	// keyStr 파서
	bool _ParseKeyTerms_ToHashes(const char* keyStr, std::vector<std::pair<uint64_t, uint64_t>>& out) const; // (propNameHash, valueHash)
	bool _CompileKeyTerms_ToTerms(const BlockTypeDef& typeDef, const char* keyStr, std::vector<Term>& outTerms) const;

	// ModelSpec 파서(네가 이미 구현한 걸 붙이되, 아래 시그니처 유지 추천)
	static int  _NormalizeRot(int val);
	static bool _ReadModelSpec(const rapidjson::Value& value, AppliedModel& outModel);

	// registry/get-create
	BlockTypeDef& _GetOrCreateBlockType(BLOCK_ID blockID);
	PropertyDomain& _GetOrCreateDomain(BlockTypeDef& typeDef, PROPERTY_ID pid);

	const BlockTypeDef* _FindBlockType(BLOCK_ID blockID) const;
	const BlockStateDef* _FindBlockState(BLOCK_ID blockID) const;

	// stateIndex decode (valueIndex per prop slot)
	void _DecodeStateIndex(const BlockTypeDef& typeDef, STATE_INDEX sidx, std::vector<VALUE_INDEX>& outValueIndexPerProp) const;

private:
	string m_strRoot;
	GlobalPropertyRegistry m_propRegistry;

	unordered_map<BLOCK_ID, BlockStateDef> m_mapBlockState;
	unordered_map<BLOCK_ID, BlockTypeDef> m_mapBlockType;
};