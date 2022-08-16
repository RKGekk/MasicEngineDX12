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
	assert(!m_ranges.empty() && m_ranges.back().NumDescriptors == RootDescriprorTableRange::UNBOUNDED_RANGE_SIZE); // Cannot insert any ranges in a table after an unbounded range

	AddSignatureLocation(range.SignatureRegistersType());

	m_ranges.push_back(range.GetRange());
	m_parameter.DescriptorTable.NumDescriptorRanges = (UINT)m_ranges.size();
	m_parameter.DescriptorTable.pDescriptorRanges = m_ranges.data();
}

RootConstantsParameter::RootConstantsParameter(uint16_t numberOf32BitValues, uint16_t shaderRegister, uint16_t registerSpace) : RootParameter(D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS) {
	AddSignatureLocation({ shaderRegister, registerSpace, ShaderRegister::ConstantBuffer });
	m_parameter.Constants = { shaderRegister, registerSpace, numberOf32BitValues };
}

RootDescriptorParameter::RootDescriptorParameter(D3D12_ROOT_PARAMETER_TYPE type, SignatureRegisters register_type) : RootParameter(type) {}

RootConstantBufferParameter::RootConstantBufferParameter(uint16_t shaderRegister, uint16_t registerSpace) : RootDescriptorParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, { shaderRegister, registerSpace, ShaderRegister::ConstantBuffer }) {}

RootShaderResourceParameter::RootShaderResourceParameter(uint16_t shaderRegister, uint16_t registerSpace) : RootDescriptorParameter(D3D12_ROOT_PARAMETER_TYPE_SRV, { shaderRegister, registerSpace, ShaderRegister::ShaderResource }) {}

RootUnorderedAccessParameter::RootUnorderedAccessParameter(uint16_t shaderRegister, uint16_t registerSpace) : RootDescriptorParameter(D3D12_ROOT_PARAMETER_TYPE_UAV, { shaderRegister, registerSpace, ShaderRegister::UnorderedAccess }) {}