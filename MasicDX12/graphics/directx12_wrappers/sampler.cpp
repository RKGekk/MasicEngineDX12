#include "sampler.h"

Sampler::Sampler(UnifiedAlgorithm algorithm, AddressMode addressU, AddressMode addressV, AddressMode addressW, FLOAT mip_lod_bias, UINT max_anisotropy, Comparator comparison_func, FLOAT min_lod, FLOAT max_lod) {
    D3D12_FILTER filter{};
    switch (algorithm) {
        case Sampler::UnifiedAlgorithm::Anisotropic: filter = D3D12_FILTER_ANISOTROPIC; break;
        case Sampler::UnifiedAlgorithm::Linear: filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; break;
        case Sampler::UnifiedAlgorithm::Point: filter = D3D12_FILTER_MIN_MAG_MIP_POINT; break;
        default: filter = D3D12_FILTER_ANISOTROPIC; break;
    }

    D3D12_TEXTURE_ADDRESS_MODE address_mode_u = GetAddressMode(addressU);
    D3D12_TEXTURE_ADDRESS_MODE address_mode_v = GetAddressMode(addressV);
    D3D12_TEXTURE_ADDRESS_MODE address_mode_w = GetAddressMode(addressW);

    m_sampler.Filter = filter;
    m_sampler.AddressU = address_mode_u;
    m_sampler.AddressV = address_mode_v;
    m_sampler.AddressW = address_mode_w;
    m_sampler.MipLODBias = mip_lod_bias;
    m_sampler.MaxAnisotropy = max_anisotropy;
    m_sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    m_sampler.BorderColor[0] = 0;
    m_sampler.BorderColor[1] = 0;
    m_sampler.BorderColor[2] = 0;
    m_sampler.BorderColor[3] = 0;
    m_sampler.MinLOD = min_lod;
    m_sampler.MaxLOD = max_lod;
}

Sampler::Sampler(AddressMode address_mode, UnifiedAlgorithm algorithm) {
    D3D12_FILTER filter{};
    UINT anisotropy = 0u;

    switch (algorithm) {
        case Sampler::UnifiedAlgorithm::Anisotropic: filter = D3D12_FILTER_ANISOTROPIC; anisotropy = 16u; break;
        case Sampler::UnifiedAlgorithm::Linear: filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; break;
        case Sampler::UnifiedAlgorithm::Point: filter = D3D12_FILTER_MIN_MAG_MIP_POINT; break;
        default: filter = D3D12_FILTER_ANISOTROPIC; break;
    }

    D3D12_TEXTURE_ADDRESS_MODE address_mode_i = GetAddressMode(address_mode);

    m_sampler.Filter = filter;
    m_sampler.AddressU = address_mode_i;
    m_sampler.AddressV = address_mode_i;
    m_sampler.AddressW = address_mode_i;
    m_sampler.MipLODBias = 0.0f;
    m_sampler.MaxAnisotropy = anisotropy;
    m_sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    m_sampler.BorderColor[0] = 0.0f;
    m_sampler.BorderColor[1] = 0.0f;
    m_sampler.BorderColor[2] = 0.0f;
    m_sampler.BorderColor[3] = 0.0f;
    m_sampler.MinLOD = 0.0f;
    m_sampler.MaxLOD = D3D12_FLOAT32_MAX;
}

Sampler::Sampler(AddressMode address_mode, Comparator comparison_func) {
    D3D12_TEXTURE_ADDRESS_MODE address_mode_i = GetAddressMode(address_mode);
    D3D12_COMPARISON_FUNC comparator = GetComparisonFunc(comparison_func);

    m_sampler.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
    m_sampler.AddressU = address_mode_i;
    m_sampler.AddressV = address_mode_i;
    m_sampler.AddressW = address_mode_i;
    m_sampler.MipLODBias = 0.0f;
    m_sampler.MaxAnisotropy = 0u;
    m_sampler.ComparisonFunc = comparator;
    m_sampler.BorderColor[0] = 0.0f;
    m_sampler.BorderColor[1] = 0.0f;
    m_sampler.BorderColor[2] = 0.0f;
    m_sampler.BorderColor[3] = 0.0f;
    m_sampler.MinLOD = 0.0f;
    m_sampler.MaxLOD = D3D12_FLOAT32_MAX;
}

Sampler::Sampler(AddressMode address_mode, MinMaxFilter filter) {
    D3D12_FILTER filter_i{};

    switch (filter) {
        case MinMaxFilter::Minimum: filter_i = D3D12_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR; break;
        case MinMaxFilter::Maximum: filter_i = D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR; break;
        default: filter_i = D3D12_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR; break;
    }

    D3D12_TEXTURE_ADDRESS_MODE address_mode_i = GetAddressMode(address_mode);

    m_sampler.Filter = filter_i;
    m_sampler.AddressU = address_mode_i;
    m_sampler.AddressV = address_mode_i;
    m_sampler.AddressW = address_mode_i;
    m_sampler.MipLODBias = 0.0f;
    m_sampler.MaxAnisotropy = 0u;
    m_sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    m_sampler.BorderColor[0] = 0;
    m_sampler.BorderColor[1] = 0;
    m_sampler.BorderColor[2] = 0;
    m_sampler.BorderColor[3] = 0;
    m_sampler.MinLOD = 0.0f;
    m_sampler.MaxLOD = D3D12_FLOAT32_MAX;
}

const D3D12_SAMPLER_DESC& Sampler::GetSampler() const {
	return m_sampler;
}

D3D12_TEXTURE_ADDRESS_MODE Sampler::GetAddressMode(AddressMode mode) {
    switch (mode) {
        case Sampler::AddressMode::Clamp: return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        case Sampler::AddressMode::Wrap: return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        case Sampler::AddressMode::Mirror: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        default: return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    }
}

D3D12_COMPARISON_FUNC Sampler::GetComparisonFunc(Comparator comparator) {
    switch (comparator) {
        case Comparator::Never: return D3D12_COMPARISON_FUNC_NEVER;
        case Comparator::Less: return D3D12_COMPARISON_FUNC_LESS;
        case Comparator::Equal: return D3D12_COMPARISON_FUNC_EQUAL;
        case Comparator::LessOrEqual: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
        case Comparator::Greater: return D3D12_COMPARISON_FUNC_GREATER;
        case Comparator::NotEqual: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
        case Comparator::GreaterOrEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
        case Comparator::Always: return D3D12_COMPARISON_FUNC_ALWAYS;
        default: return D3D12_COMPARISON_FUNC_ALWAYS;
    }
}

RootSaticSampler::RootSaticSampler(SignatureRegisters register_type, Sampler type, D3D12_SHADER_VISIBILITY visibility) {
    const D3D12_SAMPLER_DESC& sampler = type.GetSampler();

    m_static_sampler.ShaderRegister = register_type.BaseRegister;
    m_static_sampler.Filter = sampler.Filter;
    m_static_sampler.AddressU = sampler.AddressU;
    m_static_sampler.AddressV = sampler.AddressV;
    m_static_sampler.AddressW = sampler.AddressW;
    m_static_sampler.MipLODBias = sampler.MipLODBias;
    m_static_sampler.MaxAnisotropy = sampler.MaxAnisotropy;
    m_static_sampler.ComparisonFunc = sampler.ComparisonFunc;
    m_static_sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
    m_static_sampler.MinLOD = sampler.MinLOD;
    m_static_sampler.MaxLOD = sampler.MaxLOD;

    m_static_sampler.ShaderVisibility = visibility;
    m_static_sampler.RegisterSpace = register_type.RegisterSpace;
}

const D3D12_STATIC_SAMPLER_DESC& RootSaticSampler::GetStaticSampler() const {
    return m_static_sampler;
}

SignatureRegisters RootSaticSampler::GetSignatureLocation() const {
    return { (uint16_t)m_static_sampler.ShaderRegister, (uint16_t)m_static_sampler.RegisterSpace, ShaderRegister::Sampler };
}
