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