#include "descripror_table_range.h"

RootDescriprorTableRange::RootDescriprorTableRange(SignatureRegisters register_type, uint32_t descriptor_heap_index, uint32_t range_size, D3D12_DESCRIPTOR_RANGE_FLAGS flags) {
    m_range.RangeType = register_type.GetType();
    m_range.NumDescriptors = range_size;
    m_range.OffsetInDescriptorsFromTableStart = descriptor_heap_index;
    m_range.BaseShaderRegister = register_type.BaseRegister;
    m_range.RegisterSpace = register_type.RegisterSpace;
    m_range.Flags = flags;
}

const D3D12_DESCRIPTOR_RANGE1& RootDescriprorTableRange::GetRange() const {
	m_range;
}

ShaderRegister RootDescriprorTableRange::ShaderRegisterType() const {
	return m_register_type;
}

SignatureRegisters RootDescriprorTableRange::SignatureRegistersType() const {
    return { (uint16_t)m_range.BaseShaderRegister, (uint16_t)m_range.RegisterSpace, m_register_type };
}

CBDescriptorTableRange::CBDescriptorTableRange(uint16_t base_register, uint16_t register_space, uint32_t range_size, D3D12_DESCRIPTOR_RANGE_FLAGS flags) : RootDescriprorTableRange(SignatureRegisters{ base_register, register_space, ShaderRegister::ConstantBuffer }, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND, range_size, flags) {}

SRDescriptorTableRange::SRDescriptorTableRange(uint16_t base_register, uint16_t register_space, uint32_t range_size, D3D12_DESCRIPTOR_RANGE_FLAGS flags) : RootDescriprorTableRange(SignatureRegisters{ base_register, register_space, ShaderRegister::ShaderResource }, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND, range_size, flags) {}

UADescriptorTableRange::UADescriptorTableRange(uint16_t base_register, uint16_t register_space, uint32_t range_size, D3D12_DESCRIPTOR_RANGE_FLAGS flags) : RootDescriprorTableRange(SignatureRegisters{ base_register, register_space, ShaderRegister::UnorderedAccess }, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND, range_size, flags) {}

SamplerDescriptorTableRange::SamplerDescriptorTableRange(uint16_t base_register, uint16_t register_space, uint32_t range_size, D3D12_DESCRIPTOR_RANGE_FLAGS flags) : RootDescriprorTableRange(SignatureRegisters{ base_register, register_space, ShaderRegister::Sampler }, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND, range_size, flags) {}
