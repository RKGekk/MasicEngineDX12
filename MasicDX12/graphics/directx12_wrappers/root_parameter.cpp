#include "root_parameter.h"

#include <cassert>

RootParameter::RootParameter(D3D12_ROOT_PARAMETER_TYPE type, D3D12_SHADER_VISIBILITY visibility) {
	m_parameter.ParameterType = type;
	m_parameter.ShaderVisibility = visibility;
}

RootParameter::RootParameter(D3D12_ROOT_PARAMETER_TYPE type) {
	m_parameter.ParameterType = type;
	m_parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
}

RootParameter::~RootParameter() {}

const D3D12_ROOT_PARAMETER1& RootParameter::GetParameter() const {
	return m_parameter;
}

const std::vector<SignatureRegisters>& RootParameter::SignatureLocations() const {
	return m_signature_registers;
}

void RootParameter::AddSignatureLocation(const SignatureRegisters& location) {
	m_signature_registers.push_back(location);
}

RootDescriptorTableParameter::RootDescriptorTableParameter() : RootParameter(D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {}

RootDescriptorTableParameter::RootDescriptorTableParameter(D3D12_SHADER_VISIBILITY visibility) : RootParameter(D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE, visibility) {}

RootDescriptorTableParameter::RootDescriptorTableParameter(D3D12_ROOT_DESCRIPTOR_TABLE1 root_table, D3D12_SHADER_VISIBILITY visibility) : RootParameter(D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE, visibility) {
	UINT num_descriptor_ranges = root_table.NumDescriptorRanges;
	for (UINT j = 0; j < num_descriptor_ranges; ++j) {
		switch (root_table.pDescriptorRanges[j].RangeType) {
			case D3D12_DESCRIPTOR_RANGE_TYPE_CBV: AddDescriptorRange(CBDescriptorTableRange(root_table.pDescriptorRanges[j])); break;
			case D3D12_DESCRIPTOR_RANGE_TYPE_SRV: AddDescriptorRange(SRDescriptorTableRange(root_table.pDescriptorRanges[j])); break;
			case D3D12_DESCRIPTOR_RANGE_TYPE_UAV: AddDescriptorRange(UADescriptorTableRange(root_table.pDescriptorRanges[j])); break;
			case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER: AddDescriptorRange(SamplerDescriptorTableRange(root_table.pDescriptorRanges[j])); break;
		}
	}
}

RootDescriptorTableParameter::RootDescriptorTableParameter(const RootDescriptorTableParameter& other) : RootParameter(other) {
	m_ranges = other.m_ranges;
	m_parameter.DescriptorTable.NumDescriptorRanges = (UINT)m_ranges.size();
	m_parameter.DescriptorTable.pDescriptorRanges = m_ranges.data();
}

RootDescriptorTableParameter::RootDescriptorTableParameter(RootDescriptorTableParameter&& other) : RootParameter(std::move(other)) {
	m_ranges = std::move(other.m_ranges);
	m_parameter.DescriptorTable.NumDescriptorRanges = (UINT)m_ranges.size();
	m_parameter.DescriptorTable.pDescriptorRanges = m_ranges.data();
}

RootDescriptorTableParameter& RootDescriptorTableParameter::operator=(const RootDescriptorTableParameter& right) {
	if (this == &right) return *this;

	m_ranges = right.m_ranges;
	m_parameter.DescriptorTable.NumDescriptorRanges = (UINT)m_ranges.size();
	m_parameter.DescriptorTable.pDescriptorRanges = m_ranges.data();

	return *this;
}

RootDescriptorTableParameter& RootDescriptorTableParameter::operator=(RootDescriptorTableParameter&& right) {
	if (this == &right) return *this;

	m_ranges = std::move(right.m_ranges);
	m_parameter.DescriptorTable.NumDescriptorRanges = (UINT)m_ranges.size();
	m_parameter.DescriptorTable.pDescriptorRanges = m_ranges.data();

	return *this;
}

void RootDescriptorTableParameter::AddDescriptorRange(const RootDescriprorTableRange& range) {
	assert(!(!m_ranges.empty() && m_ranges.back().NumDescriptors == RootDescriprorTableRange::UNBOUNDED_RANGE_SIZE)); // Cannot insert any ranges in a table after an unbounded range

	uint32_t num_ranges = range.GetRange().NumDescriptors;
	SignatureRegisters reg = range.SignatureRegistersType();
	for (uint32_t i = 0; i < num_ranges; ++i) {
		AddSignatureLocation({ (uint16_t)(reg.BaseRegister + i), (uint16_t)reg.RegisterSpace, reg.RegisterType });
	}

	m_ranges.push_back(range.GetRange());
	m_parameter.DescriptorTable.NumDescriptorRanges = (UINT)m_ranges.size();
	m_parameter.DescriptorTable.pDescriptorRanges = m_ranges.data();
}

RootConstantsParameter::RootConstantsParameter(uint16_t numberOf32BitValues, uint16_t shaderRegister, uint16_t registerSpace, D3D12_SHADER_VISIBILITY visibility) : RootParameter(D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS, visibility) {
	AddSignatureLocation({ shaderRegister, registerSpace, ShaderRegister::ConstantBuffer });
	m_parameter.Constants = { shaderRegister, registerSpace, numberOf32BitValues };
}

RootConstantsParameter::RootConstantsParameter(D3D12_ROOT_CONSTANTS root_const, D3D12_SHADER_VISIBILITY visibility) : RootParameter(D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS, visibility) {
	AddSignatureLocation({ (uint16_t)root_const.ShaderRegister, (uint16_t)root_const.RegisterSpace, ShaderRegister::ConstantBuffer });
	m_parameter.Constants = root_const;
}

RootDescriptorParameter::RootDescriptorParameter(D3D12_ROOT_PARAMETER_TYPE type, SignatureRegisters register_type, D3D12_SHADER_VISIBILITY visibility) : RootParameter(type, visibility) {
	AddSignatureLocation(register_type);
	m_parameter.Descriptor = { register_type.BaseRegister, register_type.RegisterSpace };
}

RootDescriptorParameter::RootDescriptorParameter(D3D12_ROOT_PARAMETER_TYPE type, SignatureRegisters register_type, D3D12_ROOT_DESCRIPTOR_FLAGS flags, D3D12_SHADER_VISIBILITY visibility) : RootParameter(type, visibility) {
	AddSignatureLocation(register_type);
	m_parameter.Descriptor = { register_type.BaseRegister, register_type.RegisterSpace, flags };
}

RootDescriptorParameter::RootDescriptorParameter(D3D12_ROOT_PARAMETER_TYPE type, D3D12_ROOT_DESCRIPTOR1 root_desc, D3D12_SHADER_VISIBILITY visibility) : RootParameter(type, visibility) {
	switch (type) {
		case D3D12_ROOT_PARAMETER_TYPE_CBV:
			AddSignatureLocation({ (uint16_t)root_desc.ShaderRegister, (uint16_t)root_desc.RegisterSpace, ShaderRegister::ConstantBuffer });
			break;
		case D3D12_ROOT_PARAMETER_TYPE_SRV:
			AddSignatureLocation({ (uint16_t)root_desc.ShaderRegister, (uint16_t)root_desc.RegisterSpace, ShaderRegister::ShaderResource });
			break;
		case D3D12_ROOT_PARAMETER_TYPE_UAV:
			AddSignatureLocation({ (uint16_t)root_desc.ShaderRegister, (uint16_t)root_desc.RegisterSpace, ShaderRegister::UnorderedAccess });
			break;
		default:
			break;
	}
	m_parameter.Descriptor = root_desc;
}

RootConstantBufferParameter::RootConstantBufferParameter(uint16_t shaderRegister, uint16_t registerSpace, D3D12_SHADER_VISIBILITY visibility) : RootDescriptorParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, { shaderRegister, registerSpace, ShaderRegister::ConstantBuffer }, visibility) {}

RootConstantBufferParameter::RootConstantBufferParameter(uint16_t shaderRegister, uint16_t registerSpace, D3D12_ROOT_DESCRIPTOR_FLAGS flags, D3D12_SHADER_VISIBILITY visibility) : RootDescriptorParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, { shaderRegister, registerSpace, ShaderRegister::ConstantBuffer }, flags, visibility) {}

RootShaderResourceParameter::RootShaderResourceParameter(uint16_t shaderRegister, uint16_t registerSpace, D3D12_SHADER_VISIBILITY visibility) : RootDescriptorParameter(D3D12_ROOT_PARAMETER_TYPE_SRV, { shaderRegister, registerSpace, ShaderRegister::ShaderResource }, visibility) {}

RootShaderResourceParameter::RootShaderResourceParameter(uint16_t shaderRegister, uint16_t registerSpace, D3D12_ROOT_DESCRIPTOR_FLAGS flags, D3D12_SHADER_VISIBILITY visibility) : RootDescriptorParameter(D3D12_ROOT_PARAMETER_TYPE_SRV, { shaderRegister, registerSpace, ShaderRegister::ShaderResource }, flags, visibility) {}

RootUnorderedAccessParameter::RootUnorderedAccessParameter(uint16_t shaderRegister, uint16_t registerSpace, D3D12_SHADER_VISIBILITY visibility) : RootDescriptorParameter(D3D12_ROOT_PARAMETER_TYPE_UAV, { shaderRegister, registerSpace, ShaderRegister::UnorderedAccess }, visibility) {}

RootUnorderedAccessParameter::RootUnorderedAccessParameter(uint16_t shaderRegister, uint16_t registerSpace, D3D12_ROOT_DESCRIPTOR_FLAGS flags, D3D12_SHADER_VISIBILITY visibility) : RootDescriptorParameter(D3D12_ROOT_PARAMETER_TYPE_UAV, { shaderRegister, registerSpace, ShaderRegister::UnorderedAccess }, flags, visibility) {}


