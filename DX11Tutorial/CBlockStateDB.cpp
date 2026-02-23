#include "pch.h"
#include "CBlockStateDB.h"

void CBlockStateDB::Initialize(const char* path)
{
	// path 에서 base 경로를 잡으면 이하 경로를 찾아감.
	string basePath(path);
	CModelDB::Get().Initialize(basePath);

	filesystem::path blockStatePath(basePath + "assets/minecraft/blockstates");
	if (false == _LoadBlockStates(blockStatePath)) {
		assert(false && "Load Block State Fail!");
	}
}

bool CBlockStateDB::_LoadBlockStates(const filesystem::path& path)
{
	for (const auto& entry : filesystem::recursive_directory_iterator(path))
	{
		filesystem::path somePath = entry.path();
		if (!somePath.has_extension()) continue;

		if (0 == strcmpi(somePath.extension().string().c_str(), ".json"))
		{
			if (false == _ReadBlockStateJSON(somePath.string())) return false;
		}
	}
	return true;
}

bool CBlockStateDB::_ReadBlockStateJSON(const string& path)
{
	stringstream jsonBuffer;
	if (false == IFileStreamWrapper::ReadAllStream(path, jsonBuffer)) return false;

	Document doc;
	doc.Parse<kParseCommentsFlag | kParseTrailingCommasFlag>(jsonBuffer.str().c_str());

	if (doc.HasParseError() || !doc.IsObject()) {
		ParseErrorCode ecode = doc.GetParseError();
#ifdef _DEBUG
		cout << "Shaders.json parsing error!";
#endif // _DEBUG
		return false;
	}

	if (doc.HasMember("variants")) {
		if (!_ReadVariants(doc["variants"])) return false;
	}

	// TODO: multipart 구현
	//if (doc.HasMember("multipart")) {
	//	if (!_ReadMultipart(doc)) return false;
	//}

	return true;
}

bool CBlockStateDB::_ReadVariants(const rapidjson::Value& variants)
{
	if (!variants.IsObject()) return false;

	BlockStateDef newBlockStateDef{};
	for (auto v = variants.MemberBegin(); v != variants.MemberEnd(); ++v)
	{
		const auto& name = v->name;
		if (!name.IsString()) continue;

		const auto& value = v->value;
		if (value.IsObject())
		{
			// 단일 ModelSpec
		}
		else if (value.IsArray())
		{
			for (auto& elem : value.GetArray())
			{
				if (!elem.IsObject()) continue;
				// 다중 ModelSpec
			}
		}

		// TODO: Parsing
	}
	return true;
}

bool CBlockStateDB::_ReadMultipart(const rapidjson::Value& multipart)
{
	return false;
}
