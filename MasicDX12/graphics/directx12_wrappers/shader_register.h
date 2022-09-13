#pragma once

#include <cstdint>

#include <d3d12.h>

enum class ShaderRegister : uint8_t {
    ShaderResource = 1u,
    ConstantBuffer = 2u,
    UnorderedAccess = 4u,
    Sampler = 8u
};

struct SignatureRegisters {
    uint16_t BaseRegister = 0u;
    uint16_t RegisterSpace = 0u;
    ShaderRegister RegisterType;
    D3D12_DESCRIPTOR_RANGE_TYPE GetType();
    D3D12_ROOT_PARAMETER_TYPE GetParameterType();
    static ShaderRegister GetType(D3D12_DESCRIPTOR_RANGE_TYPE rt);
};

struct LocationHasher {
    size_t operator()(const SignatureRegisters& key) const;
};

struct LocationEquality {
    size_t operator()(const SignatureRegisters& left, const SignatureRegisters& right) const;
};
