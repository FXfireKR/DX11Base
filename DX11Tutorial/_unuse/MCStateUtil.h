#pragma once

using PROPERTY_ID = uint16_t;
using VALUE_INDEX = uint8_t;	// property 내부 값 인덱스
using STATE_INDEX = uint16_t;	// 블록 당 상태 조합수 65535 이하로 가정

using PROP_HASH = uint64_t;
using VALUE_HASH = uint64_t;
using BlockPropHashMap = std::unordered_map<PROP_HASH, VALUE_HASH>;

struct GlobalPropertyRegistry
{
	unordered_map<uint64_t, PROPERTY_ID> mapNameHashToID;
	vector<string> vecIDToName;

	PROPERTY_ID GetOrCreate(const std::string& name) {
		uint64_t h = fnv1a_64(name);
		auto it = mapNameHashToID.find(h);
		if (it != mapNameHashToID.end()) return it->second;

		PROPERTY_ID newID = (PROPERTY_ID)vecIDToName.size();
		mapNameHashToID[h] = newID;
		vecIDToName.push_back(name);
		return newID;
	}

	bool Find(const std::string& name, PROPERTY_ID& out) const
	{
		uint64_t h = fnv1a_64(name);
		auto it = mapNameHashToID.find(h);
		if (it == mapNameHashToID.end()) return false;
		out = it->second;
		return true;
	}

	bool FindByHash(uint64_t nameHash, PROPERTY_ID& out) const
	{
		auto it = mapNameHashToID.find(nameHash);
		if (it == mapNameHashToID.end()) return false;
		out = it->second;
		return true;
	}
};

struct PropertyDomain
{
	PROPERTY_ID propID;
	unordered_set<uint64_t> pendingValueHashes;
	unordered_map<uint64_t, VALUE_INDEX> mapValueHashToIndex;
	vector<uint64_t> vecIndexToValueHash;

	void CollectValueHash(uint64_t vHash) { pendingValueHashes.insert(vHash); }
	size_t DomainSize() const { return vecIndexToValueHash.size(); }
};

struct BlockTypeDef
{
	BLOCK_ID blockID = 0;
	vector<PropertyDomain> vecProps;
	unordered_map<PROPERTY_ID, uint8_t> mapPropToSlot;
	vector<uint16_t> stride;
	uint16_t totalStateCount = 1;
};

struct AppliedModel // ModelSpec
{
	string modelKey; // 예시 "miecraft:block/acaia_button"
	uint64_t modelHash = 0;

	uint8_t x = 0; // rotation-x DGR
	uint8_t y = 0; // rotation-y DGR
	bool uvlock = false;
	uint16_t weight = 1; // 가중치
};

struct Term
{
	PROPERTY_ID propID = 0;
	VALUE_INDEX valueIndex = 0;
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

static bool ParsePredicate(const std::string& key, std::vector<pair<string, string>>& out)
{
	if (key.empty())
		return true;

	size_t pos = 0;

	while (pos < key.size())
	{
		size_t comma = key.find(',', pos);
		size_t end = (comma == string::npos) ? key.size() : comma;

		size_t eq = key.find('=', pos);
		if (eq == string::npos || eq > end)
			return false; // malformed

		string name = key.substr(pos, eq - pos);
		string value = key.substr(eq + 1, end - (eq + 1));

		out.emplace_back(name, value);

		pos = end + 1;
	}
	return true;
}