#include "descripror_table_range.h"

#include <cassert>

RootDescriprorTableRange::RootDescriprorTableRange(SignatureRegisters register_type, uint32_t descriptor_heap_index, uint32_t range_size, D3D12_DESCRIPTOR_RANGE_FLAGS flags) : m_register_type(register_type.RegisterType) {
    m_range.RangeType = register_type.GetType();
    m_range.NumDescriptors = range_size;
    m_range.OffsetInDescriptorsFromTableStart = descriptor_heap_index;
    m_range.BaseShaderRegister = register_type.BaseRegister;
    m_range.RegisterSpace = register_type.RegisterSpace;
    m_range.Flags = flags;
}

RootDescriprorTableRange::RootDescriprorTableRange(const D3D12_DESCRIPTOR_RANGE1& range) : m_range(range) {
    m_register_type = SignatureRegisters::GetType(range.RangeType);
}

RootDescriprorTableRange::~RootDescriprorTableRange() {}

const D3D12_DESCRIPTOR_RANGE1& RootDescriprorTableRange::GetRange() const {
	return m_range;
}

ShaderRegister RootDescriprorTableRange::ShaderRegisterType() const {
	return m_register_type;
}

SignatureRegisters RootDescriprorTableRange::SignatureRegistersType() const {
    return { (uint16_t)m_range.BaseShaderRegister, (uint16_t)m_range.RegisterSpace, m_register_type };
}

CBDescriptorTableRange::CBDescriptorTableRange(uint16_t base_register, uint16_t register_space, uint32_t range_size, D3D12_DESCRIPTOR_RANGE_FLAGS flags) : RootDescriprorTableRange(SignatureRegisters{ base_register, register_space, ShaderRegister::ConstantBuffer }, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND, range_size, flags) {}

CBDescriptorTableRange::CBDescriptorTableRange(const D3D12_DESCRIPTOR_RANGE1& range) : RootDescriprorTableRange(range) {
    assert(range.RangeType != D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_CBV);
}

SRDescriptorTableRange::SRDescriptorTableRange(uint16_t base_register, uint16_t register_space, uint32_t range_size, D3D12_DESCRIPTOR_RANGE_FLAGS flags) : RootDescriprorTableRange(SignatureRegisters{ base_register, register_space, ShaderRegister::ShaderResource }, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND, range_size, flags) {}

SRDescriptorTableRange::SRDescriptorTableRange(const D3D12_DESCRIPTOR_RANGE1& range) : RootDescriprorTableRange(range) {
    assert(range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
}

UADescriptorTableRange::UADescriptorTableRange(uint16_t base_register, uint16_t register_space, uint32_t range_size, D3D12_DESCRIPTOR_RANGE_FLAGS flags) : RootDescriprorTableRange(SignatureRegisters{ base_register, register_space, ShaderRegister::UnorderedAccess }, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND, range_size, flags) {}

UADescriptorTableRange::UADescriptorTableRange(const D3D12_DESCRIPTOR_RANGE1& range) : RootDescriprorTableRange(range) {
    assert(range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_UAV);
}

SamplerDescriptorTableRange::SamplerDescriptorTableRange(uint16_t base_register, uint16_t register_space, uint32_t range_size, D3D12_DESCRIPTOR_RANGE_FLAGS flags) : RootDescriprorTableRange(SignatureRegisters{ base_register, register_space, ShaderRegister::Sampler }, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND, range_size, flags) {}

SamplerDescriptorTableRange::SamplerDescriptorTableRange(const D3D12_DESCRIPTOR_RANGE1& range) : RootDescriprorTableRange(range) {
    assert(range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER);
}
