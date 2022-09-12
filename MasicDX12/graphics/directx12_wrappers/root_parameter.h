#pragma once

#include <cstdint>
#include <vector>

#include <d3d12.h>

#include "shader_register.h"
#include "descripror_table_range.h"

class RootParameter {
public:
    RootParameter(D3D12_ROOT_PARAMETER_TYPE type, D3D12_SHADER_VISIBILITY visibility);
    RootParameter(D3D12_ROOT_PARAMETER_TYPE type);
    virtual ~RootParameter();

    const D3D12_ROOT_PARAMETER1& GetParameter() const;
    const std::vector<SignatureRegisters>& SignatureLocations() const;

protected:
    void AddSignatureLocation(const SignatureRegisters& location);

    D3D12_ROOT_PARAMETER1 m_parameter;

private:
    std::vector<SignatureRegisters> m_signature_registers;
};

class RootDescriptorTableParameter : public RootParameter {
public:
    RootDescriptorTableParameter();
    RootDescriptorTableParameter(D3D12_SHADER_VISIBILITY visibility);
    RootDescriptorTableParameter(D3D12_ROOT_DESCRIPTOR_TABLE1 root_table, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
    RootDescriptorTableParameter(const RootDescriptorTableParameter& that);
    RootDescriptorTableParameter(RootDescriptorTableParameter&& that);
    ~RootDescriptorTableParameter() = default;

    RootDescriptorTableParameter& operator=(const RootDescriptorTableParameter& right);
    RootDescriptorTableParameter& operator=(RootDescriptorTableParameter&& right);

    void AddDescriptorRange(const RootDescriprorTableRange& range);

private:
    std::vector<D3D12_DESCRIPTOR_RANGE1> m_ranges;
};

class RootConstantsParameter : public RootParameter {
public:
    RootConstantsParameter(uint16_t numberOf32BitValues, uint16_t shaderRegister, uint16_t registerSpace, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
    RootConstantsParameter(D3D12_ROOT_CONSTANTS root_const, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
    ~RootConstantsParameter() = default;
};

class RootDescriptorParameter : public RootParameter {
public:
    RootDescriptorParameter(D3D12_ROOT_PARAMETER_TYPE type, SignatureRegisters register_type, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
    RootDescriptorParameter(D3D12_ROOT_PARAMETER_TYPE type, SignatureRegisters register_type, D3D12_ROOT_DESCRIPTOR_FLAGS flags, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
    RootDescriptorParameter(D3D12_ROOT_PARAMETER_TYPE type, D3D12_ROOT_DESCRIPTOR1 root_desc, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
};

class RootConstantBufferParameter : public RootDescriptorParameter {
public:
    RootConstantBufferParameter(uint16_t shaderRegister, uint16_t registerSpace, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
    RootConstantBufferParameter(uint16_t shaderRegister, uint16_t registerSpace, D3D12_ROOT_DESCRIPTOR_FLAGS flags, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
    ~RootConstantBufferParameter() = default;
};

class RootShaderResourceParameter : public RootDescriptorParameter {
public:
    RootShaderResourceParameter(uint16_t shaderRegister, uint16_t registerSpace, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
    RootShaderResourceParameter(uint16_t shaderRegister, uint16_t registerSpace, D3D12_ROOT_DESCRIPTOR_FLAGS flags, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
    ~RootShaderResourceParameter() = default;
};

class RootUnorderedAccessParameter : public RootDescriptorParameter {
public:
    RootUnorderedAccessParameter(uint16_t shaderRegister, uint16_t registerSpace, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
    RootUnorderedAccessParameter(uint16_t shaderRegister, uint16_t registerSpace, D3D12_ROOT_DESCRIPTOR_FLAGS flags, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
    ~RootUnorderedAccessParameter() = default;
};