#pragma once

#include <DirectXMath.h>

#include <cstdlib>
#include <limits>

#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

const float EPSILON = std::numeric_limits<float>::epsilon();

DirectX::XMVECTOR GetTranslation(DirectX::XMMATRIX mat);
DirectX::XMVECTOR BarycentricToVec3(DirectX::FXMVECTOR v0, DirectX::FXMVECTOR v1, DirectX::FXMVECTOR v2, float u, float v);
DirectX::XMVECTOR BarycentricToVec3(const DirectX::XMFLOAT3& v0, const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2, float u, float v);
DirectX::XMVECTOR BarycentricToVec3(const DirectX::XMFLOAT4& v0, const DirectX::XMFLOAT4& v1, const DirectX::XMFLOAT4& v2, float u, float v);

const DirectX::XMFLOAT3 DEFAULT_FORWARD_VECTOR = { 0.0f, 0.0f, 1.0f };
const DirectX::XMFLOAT3 DEFAULT_BACKWARD_VECTOR = { 0.0f, 0.0f, -1.0f };
const DirectX::XMFLOAT3 DEFAULT_UP_VECTOR = { 0.0f, 1.0f, 0.0f };
const DirectX::XMFLOAT3 DEFAULT_DOWN_VECTOR = { 0.0f, -1.0f, 0.0f };
const DirectX::XMFLOAT3 DEFAULT_LEFT_VECTOR = { -1.0f, 0.0f, 0.0f };
const DirectX::XMFLOAT3 DEFAULT_RIGHT_VECTOR = { 1.0f, 0.0f, 0.0f };

template<typename T>
constexpr const T& clamp(const T& val, const T& min, const T& max) {
    return val < min ? min : val > max ? max : val;
}

namespace Math {
    constexpr float PI = 3.1415926535897932384626433832795f;
    constexpr float _2PI = 2.0f * PI;

    constexpr float Degrees(const float radians) {
        return radians * (180.0f / PI);
    }

    constexpr float Radians(const float degrees) {
        return degrees * (PI / 180.0f);
    }

    template<typename T>
    inline T Deadzone(T val, T deadzone) {
        if (std::abs(val) < deadzone) {
            return T(0);
        }

        return val;
    }

    template<typename T, typename U>
    inline T NormalizeRange(U x, U min, U max) {
        return T(x - min) / T(max - min);
    }

    template<typename T, typename U>
    inline T ShiftBias(U x, U shift, U bias) {
        return T(x * bias) + T(shift);
    }

    template <typename T>
    inline T AlignUpWithMask(T value, size_t mask) {
        return (T)(((size_t)value + mask) & ~mask);
    }

    template <typename T>
    inline T AlignDownWithMask(T value, size_t mask) {
        return (T)((size_t)value & ~mask);
    }

    template <typename T>
    inline T AlignUp(T value, size_t alignment) {
        return AlignUpWithMask(value, alignment - 1);
    }

    template <typename T>
    inline T AlignDown(T value, size_t alignment) {
        return AlignDownWithMask(value, alignment - 1);
    }

    template <typename T>
    inline bool IsAligned(T value, size_t alignment) {
        return 0 == ((size_t)value & (alignment - 1));
    }

    template <typename T>
    inline T DivideByMultiple(T value, size_t alignment) {
        return (T)((value + alignment - 1) / alignment);
    }

    inline uint32_t NextHighestPow2(uint32_t v) {
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v++;

        return v;
    }

    inline uint64_t NextHighestPow2(uint64_t v) {
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v |= v >> 32;
        v++;

        return v;
    }
}