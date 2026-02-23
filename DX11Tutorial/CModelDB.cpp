#include "pch.h"
#include "CModelDB.h"

void CModelDB::Initialize(const string& resourceRoot)
{
    m_strResourceRoot = resourceRoot;
}

void CModelDB::ScanAndLoad()
{
    filesystem::path searchPath(m_strResourceRoot + "assets/minecraft/blockstates");

    for (const auto& entry : filesystem::directory_iterator(searchPath))
    {
        filesystem::path somePath = entry.path();
        if (!somePath.has_extension()) continue;

        if (0 == strcmp(somePath.extension().string().c_str(), "json"))
        {
            // TODO : Read BlockState json
        }
    }
}

MODEL_ID CModelDB::LoadModel(const char* modelKey)
{
    const uint64_t h = fnv1a_64(modelKey);
    auto it = m_mapKeyToID.find(h);
    if (it != m_mapKeyToID.end()) return it->second;

    MODEL_ID newID = static_cast<MODEL_ID>(m_vecEntries.size());
    auto entry = std::make_unique<ModelEntry>();
    entry->key = modelKey;

    MCModelResolved resolved;
    if (!_ResolveModel(modelKey, resolved))
    {
        // fail
        return UINT64_ERROR;
    }
    else
    {
        entry->bResolved = true;
        entry->resolved = std::move(resolved);
        entry->bBaked = true;
        _BakeElements(entry->resolved, entry->baked);
    }

    m_vecEntries.push_back(std::move(entry));
    m_mapKeyToID[h] = newID;
    return newID;
}

MODEL_ID CModelDB::GetModel(const char* modelKey)
{
    const uint64_t h = fnv1a_64(modelKey);
    auto it = m_mapKeyToID.find(h);
    if (it != m_mapKeyToID.end()) return it->second;
    return UINT64_ERROR;
}

const BakedModel* CModelDB::GetBaked(MODEL_ID id) const
{
    if (id >= m_vecEntries.size()) return nullptr;
    const auto& e = m_vecEntries[id];
    if (!e || !e->bBaked) return nullptr;
    return &(e->baked);
}

bool CModelDB::_LoadRawModelJSON(IN const char* modelKey, OUT MCModelRaw& modelRaw)
{
    // model key 입력 예시 "minecraft:block/stone"
    // 변환 예시 "assets/minecraft/models/block/stone.json"

    string path = _BuildModelPath(m_strResourceRoot, modelKey);
    std::stringstream jsonBuffer;
    IFileStreamWrapper::ReadAllStream(path, jsonBuffer);

    // available comments & Trailing commas
    Document docs;
    docs.Parse<kParseCommentsFlag | kParseTrailingCommasFlag>(jsonBuffer.str().c_str());
    
    if (docs.HasParseError() || !docs.IsObject()) 
    {
        ParseErrorCode ecode = docs.GetParseError();
#ifdef _DEBUG
        cout << "Shaders.json parsing error!";
#endif // _DEBUG
        return false;
    }

    modelRaw = MCModelRaw{};

    // parent
    if (docs.HasMember("parent"))
    {
        const auto& parent = docs["parent"];
        if (parent.IsString())
        {
            modelRaw.bHasParent = true;
            modelRaw.parentKey = parent.GetString();
        }
    }

    // texture
    if (docs.HasMember("textures"))
    {
        const auto& textures = docs["textures"];
        if (textures.IsObject())
        {
            for (auto it = textures.MemberBegin(); it != textures.MemberEnd(); ++it)
            {
                const auto& key = it->name;
                const auto& value = it->value;
                if (!key.IsString() || !value.IsString()) continue;

                modelRaw.textures[key.GetString()] = value.GetString();
            }
        }
    }
    
    // elements
    if (docs.HasMember("elements"))
    {
        const auto& elements = docs["elements"];
        if (elements.IsArray())
        {
            modelRaw.elements.reserve(elements.Size());

            for (rapidjson::SizeType i = 0; i < elements.Size(); ++i) 
            {
                const auto& elem = elements[i];
                if (!elem.IsObject()) continue;

                MCModelElement modelElem{};

                if (elem.HasMember("from")) CRapidJsonParsorWrapper::ReadVector3(elem["from"], modelElem.from);
                if (elem.HasMember("to")) CRapidJsonParsorWrapper::ReadVector3(elem["to"], modelElem.to);

                // rotation
                if (elem.HasMember("rotation"))
                {
                    const auto& r = elem["rotation"];
                    if (r.IsObject())
                    {
                        modelElem.bHasRotation = true;

                        if (r.HasMember("origin"))
                            CRapidJsonParsorWrapper::ReadVector3(r["origin"], modelElem.rotOrigin);

                        if (r.HasMember("axis") && r["axis"].IsString())
                            modelElem.rotAxis = ParseAxis(r["axis"].GetString());

                        if (r.HasMember("angle") && r["angle"].IsNumber())
                            modelElem.rotAngleDeg = r["angle"].GetFloat();

                        if (r.HasMember("rescale") && r["rescale"].IsBool())
                            modelElem.bRescale = r["rescale"].GetBool();
                    }
                }

                // face
                if (elem.HasMember("faces"))
                {
                    const auto& faces = elem["faces"];
                    if (faces.IsObject())
                    {
                        for (auto it = faces.MemberBegin(); it != faces.MemberEnd(); ++it)
                        {
                            if (!it->name.IsString())
                                continue;

                            const char* faceName = it->name.GetString();
                            const int dir = ParseFaceDir(faceName);
                            if (dir < 0 || dir >= (int)FACE_DIR::COUNT)
                                continue;

                            const auto& f = it->value;
                            if (!f.IsObject())
                                continue;

                            modelElem.bHasFace[dir] = true;
                            MCModelFace face{};

                            // texture 필수
                            if (f.HasMember("texture") && f["texture"].IsString())
                                face.textureRef = f["texture"].GetString();

                            // uv optional
                            if (f.HasMember("uv"))
                            {
                                const auto& uv = f["uv"];
                                if (CRapidJsonParsorWrapper::ReadUV4(uv, face.uv))
                                    face.bHasUV = true;
                            }

                            // cullface optional
                            if (f.HasMember("cullface") && f["cullface"].IsString())
                            {
                                const int cdir = ParseFaceDir(f["cullface"].GetString());
                                if (cdir >= 0 && cdir < (int)FACE_DIR::COUNT)
                                {
                                    face.bHasCull = true;
                                    face.cullDir = (uint8_t)cdir;
                                }
                            }

                            // rotation optional (0/90/180/270)
                            if (f.HasMember("rotation") && f["rotation"].IsInt())
                                face.rotation = (uint8_t)f["rotation"].GetInt();

                            // tintindex optional
                            if (f.HasMember("tintindex") && f["tintindex"].IsInt())
                                face.tintIndex = f["tintindex"].GetInt();

                            modelElem.face[dir] = std::move(face);
                        }
                    }
                }

                modelRaw.elements.push_back(std::move(modelElem));
            }
        }
    }
    return true;
}

bool CModelDB::_ResolveModel(IN const char* modelKey, OUT MCModelResolved& modelResolved)
{
    modelResolved = MCModelResolved{};
    unordered_set<string> visit;
    return _ResolveParentRecursive(modelKey, modelResolved, visit);
}

bool CModelDB::_ResolveParentRecursive(IN const char* modelKey, MCModelResolved& modelResolved, unordered_set<string>& visitedStack)
{
    // cycle detect
    if (visitedStack.contains(modelKey)) return false;

    visitedStack.insert(modelKey);

    MCModelRaw raw;
    if (!_LoadRawModelJSON(modelKey, raw)) return false;

    // parent first resolve
    MCModelResolved parentResolved;
    bool bHasParent = raw.bHasParent;

    if (bHasParent)
    {
        if (!_ResolveParentRecursive(raw.parentKey.c_str(), parentResolved, visitedStack)) return false;

        modelResolved.textures = parentResolved.textures;

        modelResolved.elements = parentResolved.elements;
    }

    // child-run zzzz
    for (auto& kv : raw.textures)
        modelResolved.textures[kv.first] = kv.second;

    // elem
    if (!raw.elements.empty())
        modelResolved.elements = raw.elements;

    visitedStack.erase(modelKey);
    return true;
}

string CModelDB::_ResolveTextureRef(const MCModelResolved& model, const string& texRef, int depth)
{
    if (depth > 16) return std::string(); // fail
    if (texRef.empty()) return std::string();

    // "#alias"
    if (texRef[0] == '#')
    {
        string alias = texRef.substr(1);
        auto it = model.textures.find(alias);
        if (it == model.textures.end()) return std::string();

        return _ResolveTextureRef(model, it->second, depth + 1);
    }
    return texRef;
}

void CModelDB::_BakeElements(IN const MCModelResolved& modelResolved, OUT BakedModel& bakedModel)
{
    bakedModel.quads.clear();
    bakedModel.quads.reserve(modelResolved.elements.size() * 6);

    for (auto& elem : modelResolved.elements)
    {
        for (int d = 0; d < (int)FACE_DIR::COUNT; ++d)
        {
            if (!elem.bHasFace[d]) continue;

            _BakeOneElementFace(modelResolved, elem, d, bakedModel);
        }
    }
}

void CModelDB::_BakeOneElementFace(IN const MCModelResolved& modelResolved, const MCModelElement& modelElem, int faceDir, OUT BakedModel& bakedModel)
{
    const FACE_DIR eDir = static_cast<FACE_DIR>(faceDir);
    const MCModelFace& face = modelElem.face[faceDir];

    string texKey = _ResolveTextureRef(modelResolved, face.textureRef);
    if (texKey.empty()) return;

    //uint32_t texID = 

    XMFLOAT3 p[4];
    BuildFaceQuadPositions_01(modelElem, eDir, p);

    if (modelElem.bHasRotation)
    {
        // NOTE: 스켈레톤
        // - p[i]를 elem_.rotOrigin 기준으로 회전
        // - origin도 0..1로 변환해서 사용
        // ApplyElementRotation(p, elem_);
    }

    float uv01[4];
    if (face.bHasUV) 
    {
        uv01[0] = face.uv[0] / 16.0f;
        uv01[1] = face.uv[1] / 16.0f;
        uv01[2] = face.uv[2] / 16.0f;
        uv01[3] = face.uv[3] / 16.0f;
    }
    else {
        ComputeFaceUV_Default(modelElem, eDir, uv01);
    }
    ApplyUVRotation(uv01, face.rotation);

    BakedQuad q{};
    //q.textureID = 
    q.cullDir = face.bHasCull ? face.cullDir : static_cast<uint8_t>(FACE_DIR::COUNT);

    XMFLOAT3 faceNorm = FaceNormal(eDir);

    const float u0 = uv01[0];
    const float v0 = uv01[1];
    const float u1 = uv01[2];
    const float v1 = uv01[3];

    q.vert[0] = { p[0], faceNorm, {u0, v0}, 0xFFFFFFFF };
    q.vert[1] = { p[1], faceNorm, {u1, v0}, 0xFFFFFFFF };
    q.vert[2] = { p[2], faceNorm, {u1, v1}, 0xFFFFFFFF };
    q.vert[3] = { p[3], faceNorm, {u0, v1}, 0xFFFFFFFF };

    bakedModel.quads.push_back(std::move(q));
}

string CModelDB::_BuildModelPath(string resourceRoot, string modelKey)
{
    size_t colon = modelKey.find(':');
    if (colon == string::npos) return std::string();

    string nameSpace = modelKey.substr(0, colon);
    string path = modelKey.substr(colon + 1);

    return resourceRoot + "assets/" + nameSpace + "/models/" + path + ".json";
}
