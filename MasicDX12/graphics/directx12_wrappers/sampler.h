#pragma once

#include <vector>

#include <d3d12.h>
#include <cstdint>

#include "shader_register.h"

class Sampler {
public:
    enum class MinMaxFilter { Minimum, Maximum };
    enum class UnifiedAlgorithm { Anisotropic, Linear, Point };
    enum class SeparableAlgorithm { Linear, Point };
    enum class Comparator { Never, Less, Equal, LessOrEqual, Greater, NotEqual, GreaterOrEqual, Always };
    enum class AddressMode { Clamp, Wrap, Mirror };

    Sampler(
        UnifiedAlgorithm algorithm = UnifiedAlgorithm::Anisotropic,
        AddressMode addressU = AddressMode::Wrap,
        AddressMode addressV = AddressMode::Wrap,
        AddressMode addressW = AddressMode::Wrap,
        FLOAT mip_lod_bias = 0.0f,
        UINT max_anisotropy = 16u,
        Comparator comparison_func = Comparator::LessOrEqual,
        FLOAT min_lod = 0.0f,
        FLOAT max_lod = D3D12_FLOAT32_MAX
    );

    Sampler(AddressMode address_mode, UnifiedAlgorithm algorithm);
    Sampler(AddressMode address_mode, Comparator comparison_func);
    Sampler(AddressMode address_mode, MinMaxFilter filter);

    const D3D12_SAMPLER_DESC& GetSampler() const;

    static D3D12_TEXTURE_ADDRESS_MODE GetAddressMode(AddressMode mode);
    static D3D12_COMPARISON_FUNC GetComparisonFunc(Comparator comparator);

private:
    D3D12_SAMPLER_DESC m_sampler{};
};

class RootSaticSampler {
public:
    RootSaticSampler(SignatureRegisters register_type, Sampler type, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);

    const D3D12_STATIC_SAMPLER_DESC& GetStaticSampler() const;
    SignatureRegisters GetSignatureLocation() const;

protected:
    D3D12_STATIC_SAMPLER_DESC m_static_sampler;
};