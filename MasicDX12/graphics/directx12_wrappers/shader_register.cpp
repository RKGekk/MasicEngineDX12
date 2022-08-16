#include "shader_register.h"

#include <limits>
#include <type_traits>

size_t LocationHasher::operator()(const SignatureRegisters& key) const {
    size_t hashValue = 0;
    hashValue |= key.BaseRegister;
    hashValue <<= std::numeric_limits<decltype(key.BaseRegister)>::digits;
    hashValue |= key.RegisterSpace;
    hashValue <<= std::numeric_limits<decltype(key.RegisterSpace)>::digits;
    hashValue |= std::underlying_type_t<ShaderRegister>(key.RegisterType);
    return hashValue;
}

size_t LocationEquality::operator()(const SignatureRegisters& left, const SignatureRegisters& right) const {
    return left.BaseRegister == right.BaseRegister && left.RegisterSpace == right.RegisterSpace && left.RegisterType == right.RegisterType;
}

D3D12_DESCRIPTOR_RANGE_TYPE SignatureRegisters::GetType() {
    D3D12_DESCRIPTOR_RANGE_TYPE result = D3D12_DESCRIPTOR_RANGE_TYPE();
    switch (RegisterType) {
        case ShaderRegister::ShaderResource: {
            result = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        }
        break;
        case ShaderRegister::ConstantBuffer: {
            result = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
        }
        break;
        case ShaderRegister::UnorderedAccess: {
            result = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
        }
        break;
        case ShaderRegister::Sampler: {
            result = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
        }
        break;
    }
    return result;
}

D3D12_ROOT_PARAMETER_TYPE SignatureRegisters::GetParameterType() {
    D3D12_ROOT_PARAMETER_TYPE result = D3D12_ROOT_PARAMETER_TYPE();
    switch (RegisterType) {
        case ShaderRegister::ShaderResource: {
            result = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_SRV;
        }
        break;
        case ShaderRegister::ConstantBuffer: {
            result = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_CBV;
        }
        break;
        case ShaderRegister::UnorderedAccess: {
            result = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_UAV;
        }
        break;
    }
    return result;
}
