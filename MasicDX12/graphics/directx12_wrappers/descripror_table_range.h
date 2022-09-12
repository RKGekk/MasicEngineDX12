#pragma once

#include <cstdint>
#include <limits>

#include <d3d12.h>

#include "shader_register.h"

class RootDescriprorTableRange {
public:
    inline static uint32_t UNBOUNDED_RANGE_SIZE = UINT_MAX; //D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND

    RootDescriprorTableRange(SignatureRegisters register_type, uint32_t descriptor_heap_index, uint32_t range_size, D3D12_DESCRIPTOR_RANGE_FLAGS flags);
    RootDescriprorTableRange(const D3D12_DESCRIPTOR_RANGE1& range);

    virtual ~RootDescriprorTableRange();

    const D3D12_DESCRIPTOR_RANGE1& GetRange() const;
    ShaderRegister ShaderRegisterType() const;
    SignatureRegisters SignatureRegistersType() const;

private:
    D3D12_DESCRIPTOR_RANGE1 m_range;
    ShaderRegister m_register_type;
};

class CBDescriptorTableRange : public RootDescriprorTableRange {
public:
    CBDescriptorTableRange(uint16_t base_register, uint16_t register_space, uint32_t range_size = UNBOUNDED_RANGE_SIZE, D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE);
    CBDescriptorTableRange(const D3D12_DESCRIPTOR_RANGE1& range);
    ~CBDescriptorTableRange() = default;
};

class SRDescriptorTableRange : public RootDescriprorTableRange {
public:
    SRDescriptorTableRange(uint16_t base_register, uint16_t register_space, uint32_t range_size = UNBOUNDED_RANGE_SIZE, D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE);
    SRDescriptorTableRange(const D3D12_DESCRIPTOR_RANGE1& range);
    ~SRDescriptorTableRange() = default;
};

class UADescriptorTableRange : public RootDescriprorTableRange {
public:
    UADescriptorTableRange(uint16_t base_register, uint16_t register_space, uint32_t range_size = UNBOUNDED_RANGE_SIZE, D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE);
    UADescriptorTableRange(const D3D12_DESCRIPTOR_RANGE1& range);
    ~UADescriptorTableRange() = default;
};

class SamplerDescriptorTableRange : public RootDescriprorTableRange {
public:
    SamplerDescriptorTableRange(uint16_t base_register, uint16_t register_space, uint32_t range_size = UNBOUNDED_RANGE_SIZE, D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE);
    SamplerDescriptorTableRange(const D3D12_DESCRIPTOR_RANGE1& range);
    ~SamplerDescriptorTableRange() = default;
};