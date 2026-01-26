#pragma once
#include <Windows.h>
#include <cstdint>
#include <string_view>

//#define INVALID_OBJECT_ID UINT_MAX
typedef uint32_t ObjectID;
constexpr ObjectID INVALID_OBJECT_ID = UINT_MAX;

static bool IsValidObject(ObjectID checkID_) {
    return (checkID_ != INVALID_OBJECT_ID);
}

// hash-base
constexpr uint64_t FNV_OFFSET_BASIS_64 = 14695981039346656037ull;
constexpr uint64_t FNV_PRIME_64 = 1099511628211ull;

// run-time string hash
constexpr uint64_t fnv1a_64(std::string_view s) {
    uint64_t hash = FNV_OFFSET_BASIS_64;
    for (unsigned char c : s) {
        hash ^= c;
        hash *= FNV_PRIME_64;
    }
    return hash;
}

// compile-time string hash
constexpr uint64_t operator""_sid(const char* s, size_t n) {
    uint64_t hash = FNV_OFFSET_BASIS_64;
    for (size_t i = 0; i < n; ++i) {
        hash ^= static_cast<unsigned char>(s[i]);
        hash *= FNV_PRIME_64;
    }
    return hash;
}

enum class VERTEX_SEMANTIC
{
    COLOR,
    NORMAL,
    POSITION,
    POSITIONT,
    PSIZE,
    TANGENT,
    TEXCOORD,
    BINORMAL,
    BLENDINDICES,
    BLENDWEIGHT,
    // PSIZE
    // TESSFACTOR
    // VFACE
    // VPOS
    // DEPTH
};

constexpr const char* GetVertexSemanticString(VERTEX_SEMANTIC eVertexSemantic_)
{
    switch (eVertexSemantic_)
    {
        case VERTEX_SEMANTIC::COLOR: return "COLOR";
        case VERTEX_SEMANTIC::NORMAL: return "NORMAL";
        case VERTEX_SEMANTIC::POSITION: return "POSITION";
        case VERTEX_SEMANTIC::POSITIONT: return "POSITIONT";
        case VERTEX_SEMANTIC::PSIZE: return "PSIZE";
        case VERTEX_SEMANTIC::TANGENT: return "TANGENT";
        case VERTEX_SEMANTIC::TEXCOORD: return "TEXCOORD";
        case VERTEX_SEMANTIC::BINORMAL: return "BINORMAL";
        case VERTEX_SEMANTIC::BLENDINDICES: return "BLENDINDICES";
        case VERTEX_SEMANTIC::BLENDWEIGHT: return "BLENDWEIGHT";
    }
    assert(false && "Invalid VERTEX_SEMANTIC");
    return "";
}

struct VertexElementDesc
{
    VERTEX_SEMANTIC eSemantic;
    uint32_t uSemanticIndex = 0;
    DXGI_FORMAT dxgiFormat;
    uint32_t uOffset;
    uint32_t uInputSlot = 0;
    D3D11_INPUT_CLASSIFICATION eInputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    uint32_t uInstanceDataStepRate = 0;

    // initailizer-helper
    VertexElementDesc(VERTEX_SEMANTIC eSemantic_, DXGI_FORMAT dxgiFormat_
        , uint32_t uOffset_ = D3D11_APPEND_ALIGNED_ELEMENT, uint32_t uSemanticIndex_ = 0
        , uint32_t uInputSlot_ = 0, D3D11_INPUT_CLASSIFICATION eInputSlotClass_ = D3D11_INPUT_PER_VERTEX_DATA
        , uint32_t uInstanceDataStepRate_ = 0
    )
        : eSemantic(eSemantic_)
        , uSemanticIndex(uSemanticIndex_)
        , dxgiFormat(dxgiFormat_)
        , uOffset(uOffset_)
        , uInputSlot(uInputSlot_)
        , eInputSlotClass(eInputSlotClass_)
        , uInstanceDataStepRate(uInstanceDataStepRate_)
    {}
};

struct VertexLayoutDesc
{
    std::vector<VertexElementDesc> vecElements;
    uint32_t uStride = 0;

    bool operator==(const VertexLayoutDesc& rhs_) const {
        if (uStride != rhs_.uStride) return false;
        if (vecElements.size() != rhs_.vecElements.size()) return false;
        
        for (size_t i = 0; i < vecElements.size(); ++i) {
            const auto& lhs = vecElements[i];
            const auto& rhs = rhs_.vecElements[i];

            if (lhs.eSemantic != rhs.eSemantic) return false;
            if (lhs.uSemanticIndex != rhs.uSemanticIndex) return false;
            if (lhs.dxgiFormat != rhs.dxgiFormat) return false;
            if (lhs.uOffset != rhs.uOffset) return false;
            if (lhs.uInputSlot != rhs.uInputSlot) return false;
        }
        return true;
    }
};