#pragma once

using PROPERTY_ID = uint16_t;
using VALUE_INDEX = uint8_t;	// property 내부 값 인덱스
using STATE_INDEX = uint16_t;	// 블록 당 상태 조합수 65535 이하로 가정

struct GlobalPropertyRegistry
{
	unordered_map<uint64_t, PROPERTY_ID> mapNameHashToID;
	vector<string> vecIDToName;
};

struct PropertyDomain
{
	PROPERTY_ID propID;
	unordered_map<uint64_t, VALUE_INDEX> mapValueHashToIndex;
	vector<uint64_t> vecIndexToValueHash;
};

struct BlockTypeDef
{
	BLOCK_ID blockID;
	vector<PropertyDomain> vecProps;
	vector<uint16_t> stride;
	uint16_t totalStateCount = 1;
};

struct AppliedModel // ModelSpec
{
	string modelKey; // 예시 "miecraft:block/acaia_button"
	uint8_t x = 0;
	uint8_t y = 0;
	bool uvlock = false;
	uint16_t weight = 1;
};

struct Term
{
	PROPERTY_ID propID;
	VALUE_INDEX valueIndex;
};

struct VariantRule
{
	vector<Term> vecAndTerms;
	vector<AppliedModel> vecChoices;
};

struct BlockStateDef
{
	vector<VariantRule> vecVariants;
	//vector<MultipartEntry> vecMultipart;
};

static void ReadModelSpec(IN const rapidjson::Value& name, const rapidjson::Value& value, OUT BlockStateDef& blockStateDef)
{

}