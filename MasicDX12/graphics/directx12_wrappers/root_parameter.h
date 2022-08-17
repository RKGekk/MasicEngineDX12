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
    virtual ~RootParameter() = 0;

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
    RootDescriptorTableParameter(D3D12_ROOT_DESCRIPTOR_TABLE1 root_table);
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
    RootConstantsParameter(uint16_t numberOf32BitValues, uint16_t shaderRegister, uint16_t registerSpace);
    RootConstantsParameter(D3D12_ROOT_CONSTANTS root_const);
    ~RootConstantsParameter() = default;
};

class RootDescriptorParameter : public RootParameter {
public:
    RootDescriptorParameter(D3D12_ROOT_PARAMETER_TYPE type, SignatureRegisters register_type);
    RootDescriptorParameter(D3D12_ROOT_PARAMETER_TYPE type, SignatureRegisters register_type, D3D12_ROOT_DESCRIPTOR_FLAGS flags);
    RootDescriptorParameter(D3D12_ROOT_PARAMETER_TYPE type, D3D12_ROOT_DESCRIPTOR1 root_desc);
};

class RootConstantBufferParameter : public RootDescriptorParameter {
public:
    RootConstantBufferParameter(uint16_t shaderRegister, uint16_t registerSpace);
    RootConstantBufferParameter(uint16_t shaderRegister, uint16_t registerSpace, D3D12_ROOT_DESCRIPTOR_FLAGS flags);
    ~RootConstantBufferParameter() = default;
};

class RootShaderResourceParameter : public RootDescriptorParameter {
public:
    RootShaderResourceParameter(uint16_t shaderRegister, uint16_t registerSpace);
    RootShaderResourceParameter(uint16_t shaderRegister, uint16_t registerSpace, D3D12_ROOT_DESCRIPTOR_FLAGS flags);
    ~RootShaderResourceParameter() = default;
};

class RootUnorderedAccessParameter : public RootDescriptorParameter {
public:
    RootUnorderedAccessParameter(uint16_t shaderRegister, uint16_t registerSpace);
    RootUnorderedAccessParameter(uint16_t shaderRegister, uint16_t registerSpace, D3D12_ROOT_DESCRIPTOR_FLAGS flags);
    ~RootUnorderedAccessParameter() = default;
};